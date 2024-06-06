#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <termios.h>

#define MAX_TUBES 100
#define MAX_POWERUPS 5
#define N_TEXTURES 5
#define NLEVELS 2

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

enum {
    TUBE_NONE = 0,
    TUBE_DEATH = 1,
    TUBE_PLATFORM = 2,
    TUBE_TOGGLE = 3,
    TUBE_RED = 4,
    TUBE_BLUE = 5,
    TUBE_WINNER = 6,
};

typedef struct {
    bool gameOver;
    bool pause;
    int api_version;
    int max_api_version;
    bool api_changed; // either 0 or 1
    bool win;
    int level;
    bool new_level;
}  Settings;

typedef struct Floppy {
    Vector2 velocity;
    Vector2 position;
    int radius;
    Color color;
    bool is_bulldozin;
} Floppy;

typedef struct Tubes {
    Rectangle rec;
    Color color;
    bool active;
    int type;
    int toggled;
} Tubes;

typedef struct {
    Tubes tubes[MAX_TUBES];
    Vector2 scale;
    int nTubes;
    int visibility;
} Map;

typedef struct {
    Texture2D textures[N_TEXTURES];
} Assets;

typedef struct {
    Vector2 position;
    Color color;
    int api_version_id;
} Powerup;

typedef struct {
    int active_powerup;
    int n_powerups;
    Powerup powerup[MAX_POWERUPS];
} Powerups;

typedef struct {
    Font ibmfont;
    int running;
    int pos;
} Intro;

typedef struct {
    int api_version;
    Vector2 position;
} Checkpoint;

#endif
