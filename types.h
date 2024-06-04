#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define MAX_TUBES 100

typedef struct {
    bool gameOver;
    bool pause;
    int api_version;
    int max_api_version;
}  Settings;

typedef struct Floppy {
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

#endif
