#ifndef GAME_API_H
#define GAME_API_H

#include "game.h"
#include "stdbool.h"
#include <stdlib.h>

#define GAME_STATE_SIZE 20

typedef struct {
  // returns int for size of game_state
  size_t (*open)(void);
  bool (*close)(void);

  // creates the memory for the game state
  void (*init)(game_state *state);

  // returns false iff should close
  bool (*update)(game_state *state);
  void (*render)(const game_state *state);
} game_api;

extern const game_api api;

#endif
