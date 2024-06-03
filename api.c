#include "api.h"
#include "raylib.h"

// returns true on success
size_t open(void) {
  // InitWindow(800, 600, "title");
  return 50;
}

bool close(void);

// creates the memory for the game state
void init(game_state *state)
{

}

// returns false iff should close
bool update(game_state *state)
{
    return true;
}

void render(const game_state *state)
{

}

const game_api api = {
    .open = open,
    .close = close,
    .init = init,
    .update = update,
    .render = render,
};
