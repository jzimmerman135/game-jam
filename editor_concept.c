#include "api.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include "editor_concept_user_code.h"
#include <unistd.h>

struct game_state
{
    int var;
    int api_version_id; // either number or LOAD_NEW_API
    bool is_paused;
    bool is_editing;
};

void opengame(void)
{
    if (getenv("EDITOR") == NULL) {
        perror("cannot play game without EDITOR environment variable set\n");
        exit(1);
    }

    InitWindow(800, 600, "title");
    SetTargetFPS(30);
}

void closegame(void)
{
    CloseWindow();
}

void init(game_state *state)
{
    state->var = -1;
    state->api_version_id = 0;
    state->is_paused = false;
    state->is_editing = false;
}

bool try_open_editor(void)
{
    printf("trying to open editor\n");

    int pid = fork();
    if (pid == 0) {
        execlp(getenv("EDITOR"), getenv("EDITOR"), "editor_concept_user_code.c", NULL);
        perror("execlp");
        exit(1);
    }

    // FILE *fp = popen("$EDITOR editor_concept_user_code.c", "r");
    return true;
}


bool update(game_state *state)
{
    BeginDrawing();

    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
        return false;

    bool user_started_editing = !state->is_editing && IsKeyPressed(KEY_E);
    if (user_started_editing) {
        state->is_paused = true;
        state->is_editing = true;
        assert(try_open_editor());
        return true;
    }

    bool user_stopped_editing = state->is_paused && state->is_editing && IsKeyPressed(KEY_E);
    if (user_stopped_editing) {
        state->is_editing = false;
        state->is_paused = false;
        state->api_version_id = LOAD_NEW_API; // ask for new api to be dlopen'd
        return true;
    }

    if (IsKeyPressed(KEY_LEFT)) {
        state->api_version_id--;
    } else if (IsKeyPressed(KEY_RIGHT)) {
        state->api_version_id++;
    }

    if (state->is_paused) {
        return true;
    }

    state->var = user_var_value();

    return true;
}

void render(const game_state *state)
{
    ClearBackground(WHITE);

    if (state->is_paused) {
        DrawText("paused", 200, 200, 50, RED);
        goto skipdraw;
    }

    char text[80];
    snprintf(text, 80, "Var: %d", state->var);
    DrawText(text, 200, 200, 50, BLUE);

    snprintf(text, 80, "API Version Id: %d", state->api_version_id);
    DrawText(text, 200, 100, 20, GREEN);

skipdraw:
    EndDrawing();
}

bool step(game_state *state) {
    if (!update(state))
        return false;
    render(state);
    return true;
}

int get_api_version_id(const game_state *state)
{
    return state->api_version_id;
}

void set_api_version_id(game_state *state, int version_id)
{
    state->api_version_id = version_id;
}

const game_api shared_obj_api = {
    .game_state_size = sizeof(game_state),
    .open = opengame,
    .close = closegame,
    .init = init,
    .step = step,
    .requested_api_version_id = get_api_version_id,
    .set_api_version_id_callback = set_api_version_id,
};
