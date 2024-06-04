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
    .update = update,
    .render = render,
    .get_api_version_id = get_api_version_id,
    .set_api_version_id = set_api_version_id,
};
