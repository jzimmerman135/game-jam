#ifndef GAME_API_H
#define GAME_API_H

#include "stdbool.h"
#include <stdlib.h>

#define GAME_STATE_SIZE 20

#define LOAD_NEW_API (-1)

struct game_state;
typedef struct game_state game_state;

typedef struct {
  // returns int for size of game_state
  void (*open)(void);
  void (*close)(void);

  // creates the memory for the game state
  void (*init)(game_state *state);

  // returns false iff should close
  bool (*update)(game_state *state);
  void (*render)(const game_state *state);

  int (*get_api_version_id)(const game_state *state);
  void (*set_api_version_id)(game_state *state, int version_id);

  const size_t game_state_size;
} game_api;

extern const game_api shared_obj_api;

#endif
