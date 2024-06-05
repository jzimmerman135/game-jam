#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define MAX_TUBES 100
#define MAX_POWERUPS 32
#define N_TEXTURES 5

enum {
    TUBE_NONE = 0,
    TUBE_DEATH = 1,
    TUBE_PLATFORM = 2,
    TUBE_TOGGLE = 3,
    TUBE_RED = 4,
    TUBE_BLUE = 5,
};

typedef struct {
    bool gameOver;
    bool pause;
    int api_version;
    int max_api_version;
    bool api_changed; // either 0 or 1
}  Settings;

typedef struct Floppy {
    Vector2 velocity;
    Vector2 position;
    int radius;
    Color color;
} Floppy;

typedef struct Tubes {
    Rectangle rec;
    Color color;
    bool active;
    int type;
} Tubes;

typedef struct {
    Tubes tubes[MAX_TUBES];
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
    int radius;
    int nPowerups;
    Powerup powerup[MAX_POWERUPS];
} Powerups;

#endif
