#ifndef GAME_API_H
#define GAME_API_H

#include "stdbool.h"
#include <stdlib.h>
#include "raylib.h"
#include "types.h"

#define GAME_STATE_SIZE 20

typedef struct game_state game_state;

struct game_state {
    int screenWidth;
    int screenHeight;

    int score;
    int hiScore;

    Settings settings;
    Vector2 screen;
    Camera2D camera;
    Floppy floppy;
    Map map;

    float delta;
    float elapsed;

    bool superfx;
};

typedef struct {
  // returns int for size of game_state
  void (*open)(void);
  void (*close)(void);

  // creates the memory for the game state
  void (*init)(game_state *state);

  // returns false iff should close
  bool (*step)(game_state *state);

  int (*requested_api_version_id)(const game_state *state);
  void (*set_api_version_id_callback)(game_state *state, int version_id);

  const size_t game_state_size;
} game_api;

extern const game_api shared_obj_api;

#endif
