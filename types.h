#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define MAX_TUBES 100

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

#endif
