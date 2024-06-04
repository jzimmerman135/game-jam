#include "api.h"
#include "raylib.h"
#include <stdio.h>

struct game_state {
};

// returns true on success
void open(void)
{
    InitWindow(800, 600, "title");
}

void close(void)
{
    CloseWindow();
}

void init(game_state *state)
{
}

// returns false iff should close
bool update(game_state *state)
{
    BeginDrawing();
    if (IsKeyPressed(KEY_ESCAPE))
        return false;

    return true;
}

void render(const game_state *state)
{
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
    return 0;
}

void set_api_version_id(game_state *state, int version_id) {
    (void)state;
    (void)version_id;
    return;
}

const game_api shared_obj_api = {
    .open = open,
    .close = close,
    .init = init,
    .step = step,
    .requested_api_version_id = get_api_version_id,
    .set_api_version_id_callback = set_api_version_id,
};
