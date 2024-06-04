#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define MAX_TUBES 100
#define MAX_POWERUPS 32

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
} Tubes;

typedef struct {
    Tubes tubes[MAX_TUBES];
    int nTubes;
} Map;

typedef struct {
    int nPowerups;
    struct {
        Vector2 position;
        Color color;
        bool active;
        int api_version;
    } powerups[MAX_POWERUPS];
} Powerups;

#endif
