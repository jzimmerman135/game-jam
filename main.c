#include "api.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>
#include <sys/_types/_ino_t.h>
#include <sys/stat.h>

// this name must be used in makefile
#define API_SO_FILENAME "game_api.dylib"

typedef struct {
    int curr_version_id;
    int n_apis;
    ino_t last_st_ino;
    const game_api **apis;
    void **handles;
} api_manager;

const game_api *register_api(api_manager *manager, const game_api *api, void *handle, ino_t ino)
{
    manager->curr_version_id = manager->n_apis;
    manager->n_apis++;

    manager->last_st_ino = ino;
    manager->apis = realloc(manager->apis, sizeof(manager->apis[0]) * manager->n_apis);
    manager->handles = realloc(manager->handles, sizeof(manager->handles[0]) * manager->n_apis);

    manager->apis[manager->curr_version_id] = api;
    manager->handles[manager->curr_version_id] = handle;

    return api;
}

const game_api *swap_api(api_manager *manager, int api_version)
{
    if (api_version == LOAD_NEW_API) {
        struct stat attr;
        bool stat_success = stat(API_SO_FILENAME, &attr) == 0;
        printf("curr version %d\n", manager->curr_version_id);

        if (manager->handles[manager->curr_version_id]) {
            printf("closing handler %p\n", (void*)manager->handles[manager->curr_version_id]);
            dlclose(manager->handles[manager->curr_version_id]);
        }

        if (stat_success && manager->last_st_ino != attr.st_ino) {
            void *handle = dlopen(API_SO_FILENAME, RTLD_LAZY);
            void *api = dlsym(handle, "shared_obj_api");
            if (!api) {
                fprintf(stderr, "Failed to find symbol 'game_api' in %s\n", API_SO_FILENAME);
                exit(1);
            }
            printf("ohnodebug: %p\n", (void*)api);
            return register_api(manager, api, handle, attr.st_ino);
            printf("curr version %d\n", manager->curr_version_id);
        } else if (!stat_success) {
            fprintf(stderr, "failed to stat %s\n", API_SO_FILENAME);
            exit(1);
        }
        return manager->apis[manager->curr_version_id];
    } else if (api_version >= manager->n_apis) {
        fprintf(stderr, "%s::%d bad api version id passed %d. Should be less than %d ",
                __FILE__, __LINE__, api_version, manager->n_apis);
        exit(1);
    }

    return manager->apis[api_version];
}

int main(void) {
    api_manager api_manager = {0};

    const game_api *api = register_api(&api_manager, &shared_obj_api, NULL, -1);
    api->open();

    void *game_state = malloc(api->game_state_size);
    assert(game_state);

    api->init(game_state);

    int i=0;
    while (true) {
        int requested_api_version = api->get_api_version_id(game_state);
        if (api_manager.curr_version_id != requested_api_version) {
            int old_game_state_size = api->game_state_size;
            api = swap_api(&api_manager, requested_api_version);
            api->set_api_version_id(game_state, api_manager.curr_version_id);
            if (old_game_state_size != api->game_state_size) {
                game_state = realloc(game_state, api->game_state_size);
                api->init(game_state);
            }
            assert(game_state);
        }

        if ((i++ % 30) == 0) {
           printf("hellodebug: %p\n", (void*)api);
        }

        if (!api->update(game_state))
            break;


        api->render(game_state);
    }

    api->close();

    free(game_state);
    return 0;
}
