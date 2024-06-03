#define MAX_TUBES 100
#define FLOPPY_RADIUS 24
#define TUBES_WIDTH 80

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


#ifndef GAME_STATE_H
#define GAME_STATE_H
typedef struct {
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
} game_state;
#endif
