#ifndef GAME_API_H
#define GAME_API_H

#include "stdbool.h"
#include <stdlib.h>
#include "raylib.h"

#define GAME_STATE_SIZE 20
#define MAX_TUBES 100
#define LOAD_NEW_API (-1)

typedef struct game_state game_state;

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

typedef struct Floppy {
    Vector2 position;
    int radius;
    Color color;
} Floppy;

typedef struct Tubes {
    Rectangle rec;
    Color color;
    bool active;
} Tubes;

struct game_state {
    int screenWidth;
    int screenHeight;

    bool gameOver;
    bool pause;
    int score;
    int hiScore;

    Floppy floppy;
    Tubes tubes[MAX_TUBES*2];
    Vector2 tubesPos[MAX_TUBES];
    int tubesSpeedX;
    bool superfx;
    int nTubes;
    float xOffset;
};

extern const game_api shared_obj_api;

void set_nTubes(game_state *gs, int nTubes);

#endif
