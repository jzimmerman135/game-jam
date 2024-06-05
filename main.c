#include "api.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#define MADEIT() perror(stderr, "%s::%d made it\n", __FILE__, __LINE__)

// this name must be used in makefile
const char TMP_API_PATH[] = ".game_api_VXXXXXX";
const char API_PATH[] = "game_api.dylib";

typedef struct {
    char path[sizeof(TMP_API_PATH)];
    ino_t st_ino;
    void *handle;
    const game_api *api;
} api_instance;

typedef struct {
    int curr_version_id;
    int n_versions;
    api_instance *versions;
} api_manager;

void mktempcpy(const char origpath[], char newpath[], size_t size, ssize_t pathlen) {
    strncpy(newpath, TMP_API_PATH, sizeof(TMP_API_PATH));
    assert(mkstemp(newpath) != -1);
    FILE *org_api_fp = fopen(origpath, "r");
    FILE *tmp_api_fp = fopen(newpath, "w");
    assert(org_api_fp && tmp_api_fp);

    void *tmp = malloc(size);
    assert(tmp);

    size_t n_read = fread(tmp, 1,  size, org_api_fp);
    assert(n_read == size);
    size_t n_wrote = fwrite(tmp, 1, size, tmp_api_fp);
    assert(n_wrote == size);

    free(tmp);

    fflush(tmp_api_fp);
    fclose(org_api_fp);
    fclose(tmp_api_fp);
}

// if api_version == manger->n_versions, then a new version is created
const game_api *use_api(api_manager *manager, struct stat api_attr, int api_version)
{
    if (api_version >= manager->n_versions + 1 || api_version < 0) {
        fprintf(stderr, "api_version[%d] invalid with n_dylibs[%d]\n", api_version, manager->n_versions);
        exit(1);
    }

    if (api_version < manager->n_versions) {
        manager->curr_version_id = api_version;
        return manager->versions[manager->curr_version_id].api;
    }

    manager->curr_version_id = manager->n_versions++;
    manager->versions = realloc(manager->versions, sizeof(manager->versions[0]) * manager->n_versions);

    assert(stat(API_PATH, &api_attr) == 0);
    api_instance *new_api_version = &manager->versions[manager->curr_version_id];
    mktempcpy(API_PATH, new_api_version->path, api_attr.st_size, sizeof(API_PATH));

    new_api_version->st_ino = api_attr.st_ino;
    new_api_version->handle = dlopen(new_api_version->path, RTLD_LAZY);
    new_api_version->api = dlsym(new_api_version->handle, "shared_obj_api");
    if (!new_api_version->api) {
        fprintf(stderr, "Failed to find symbol 'game_api' in %s\n", API_PATH);
        exit(1);
    }

    return new_api_version->api;
}

void clean_up_apis(api_manager *manager) {
    for (int i = 0; i < manager->n_versions; i++) {
        dlclose(manager->versions[i].handle);
        remove(manager->versions[i].path);
    }
}

#include <dirent.h>

void delete_files_with_prefix(const char *directory, const char *prefix) {
    struct dirent *entry;
    DIR *dp = opendir(directory);
    if (dp == NULL) {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dp))) {
        if (entry->d_type != DT_REG || strncmp(entry->d_name, prefix, strlen(prefix)))
            continue;
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);
        remove(filepath) ? perror("remove") : (void)printf("Deleted: %s\n", filepath);
    }
   closedir(dp);
}

int main(void) {
    api_manager api_manager = {0};

    delete_files_with_prefix(".", ".game_api_V");

    struct stat api_attr;
    assert(stat(API_PATH, &api_attr) == 0);
    const game_api *api = use_api(&api_manager, api_attr, 0);

    printf("game state size %ld\n", api->game_state_size);

    api->open();
    void *game_state = malloc(api->game_state_size);
    assert(game_state);
    memset(game_state, 0, api->game_state_size);
    api->init(game_state);

    int i = 0;
    do {
        // throttle check for a fresh api dylib every 10 frames
        if (i++ % 10 == 0) {
            ino_t prev_ino = api_attr.st_ino;
            assert(stat(API_PATH, &api_attr) == 0);
            if (api_attr.st_ino != prev_ino) {
                api->api_changed_callback(game_state);
            }
        }

        int requested_api_version = api->requested_api_version_id(game_state);
        if (api_manager.curr_version_id != requested_api_version) {
            int old_game_state_size = api->game_state_size;
            api = use_api(&api_manager, api_attr, requested_api_version);
            if (old_game_state_size != api->game_state_size) {
                game_state = realloc(game_state, api->game_state_size);
                memset(game_state, 0, api->game_state_size);
                assert(game_state);
                api->init(game_state);
            }
        }
    } while (api->step(game_state));

    api->close();

    clean_up_apis(&api_manager);
    free(game_state);
    return 0;
}
