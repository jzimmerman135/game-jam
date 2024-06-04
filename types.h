#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define MAX_TUBES 100
#define MAX_POWERUPS 32

#define LOAD_NEW_API (-1)

typedef struct {
    bool gameOver;
    bool pause;
    int api_version;
    int max_api_version;
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
} Map;

typedef struct {
    Vector2 position;
    Color color;
    bool active;
    int api_version_id;
} Powerup;

typedef struct {
    int nPowerups;
     Powerup powerups[MAX_POWERUPS];
} Powerups;

#endif
