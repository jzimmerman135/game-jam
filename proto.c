#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define MAX_TUBES 100
#define FLOPPY_RADIUS 24
#define TUBES_WIDTH 80

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------

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
} game_state;

game_state G;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(game_state *);         // Initialize game
static void UpdateGame(game_state *);       // Update game (one frame)
static void DrawGame(game_state *);         // Draw game (one frame)
static void UnloadGame(game_state *);       // Unload game
static void UpdateDrawFrame(game_state *);  // Update and Draw (one frame)

void floppy_init(Floppy *flop)
{
    //Vector2 position;
    flop->radius = 0;
    flop->color.r = 0;
    flop->color.g = 0;
    flop->color.b = 0;
    flop->color.a = 255;
}

void tubes_init(Tubes *tb)
{
    tb->rec.x = 0;
    tb->rec.y = 0;
    tb->rec.width = 0;
    tb->rec.height = 0;

    tb->color.r = 0;
    tb->color.g = 0;
    tb->color.b = 0;
    tb->color.a = 255;

    tb->active = false;
}

void init_game_state(game_state *gs)
{
    int i;
    gs->screenWidth = 800;
    gs->screenHeight = 450;

    gs->gameOver = false;
    gs->pause = false;
    gs->score = 0;
    gs->hiScore = 0;

    floppy_init(&gs->floppy);

    for (i = 0; i < MAX_TUBES; i++) {
        gs->tubesPos[i].x = 0;
        gs->tubesPos[i].y = 0;
    }

    for (i = 0; i < MAX_TUBES*2; i++) {
        tubes_init(&gs->tubes[i]);
    }

    gs->tubesSpeedX = 0;
    gs->superfx = false;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    init_game_state(&G);
    InitWindow(G.screenWidth, G.screenHeight, "worldshaper");

    InitGame(&G);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame(&G);
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame(&G);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


// Initialize game variables
void InitGame(game_state *gs)
{
    gs->floppy.radius = FLOPPY_RADIUS;
    gs->floppy.position = (Vector2){80, gs->screenHeight/2 - gs->floppy.radius};
    gs->tubesSpeedX = 2;

    for (int i = 0; i < MAX_TUBES; i++)
    {
        gs->tubesPos[i].x = 400 + 280*i;
        gs->tubesPos[i].y = -GetRandomValue(0, 120);
    }

    for (int i = 0; i < MAX_TUBES*2; i += 2)
    {
        gs->tubes[i].rec.x = gs->tubesPos[i/2].x;
        gs->tubes[i].rec.y = gs->tubesPos[i/2].y;
        gs->tubes[i].rec.width = TUBES_WIDTH;
        gs->tubes[i].rec.height = 255;

        gs->tubes[i+1].rec.x = gs->tubesPos[i/2].x;
        gs->tubes[i+1].rec.y = 600 + gs->tubesPos[i/2].y - 255;
        gs->tubes[i+1].rec.width = TUBES_WIDTH;
        gs->tubes[i+1].rec.height = 255;

        gs->tubes[i/2].active = true;
    }

    gs->score = 0;

    gs->gameOver = false;
    gs->superfx = false;
    gs->pause = false;
}

// Update game (one frame)
void UpdateGame(game_state *gs)
{
    if (!gs->gameOver)
    {
        if (IsKeyPressed('P')) gs->pause = !gs->pause;

        if (!gs->pause)
        {
            for (int i = 0; i < MAX_TUBES; i++) gs->tubesPos[i].x -= gs->tubesSpeedX;

            for (int i = 0; i < MAX_TUBES*2; i += 2)
            {
                gs->tubes[i].rec.x = gs->tubesPos[i/2].x;
                gs->tubes[i+1].rec.x = gs->tubesPos[i/2].x;
            }

            if (IsKeyDown(KEY_SPACE) && !gs->gameOver) gs->floppy.position.y -= 3;
            else gs->floppy.position.y += 1;

            // Check Collisions
            for (int i = 0; i < MAX_TUBES*2; i++)
            {
                if (CheckCollisionCircleRec(gs->floppy.position, gs->floppy.radius, gs->tubes[i].rec)) {
                    gs->gameOver = true;
                    gs->pause = false;
                } else if ((gs->tubesPos[i/2].x < gs->floppy.position.x) && gs->tubes[i/2].active && !gs->gameOver) {
                    gs->score += 100;
                    gs->tubes[i/2].active = false;

                    gs->superfx = true;

                    if (gs->score > gs->hiScore) gs->hiScore = gs->score;
                }
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame(&G);
            gs->gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(game_state *gs)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!gs->gameOver)
        {
            DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius, DARKGRAY);

            // Draw tubes
            for (int i = 0; i < MAX_TUBES; i++) {

                DrawRectangle(gs->tubes[i*2].rec.x,
                        gs->tubes[i*2].rec.y,
                        gs->tubes[i*2].rec.width,
                        gs->tubes[i*2].rec.height, GRAY);
                DrawRectangle(gs->tubes[i*2 + 1].rec.x,
                        gs->tubes[i*2 + 1].rec.y,
                        gs->tubes[i*2 + 1].rec.width,
                        gs->tubes[i*2 + 1].rec.height, GRAY);
            }

            // Draw flashing fx (one frame only)
            if (gs->superfx) {
                DrawRectangle(0, 0, gs->screenWidth, gs->screenHeight, WHITE);
                gs->superfx = false;
            }

            DrawText(TextFormat("%04i", gs->score), 20, 20, 40, GRAY);
            DrawText(TextFormat("HI-SCORE: %04i", gs->hiScore), 20, 70, 20, LIGHTGRAY);

            if (gs->pause) DrawText("GAME PAUSED", gs->screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, gs->screenHeight/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

    EndDrawing();
}

// Unload game variables
void UnloadGame(game_state *gs)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(game_state *gs)
{
    UpdateGame(gs);
    DrawGame(gs);
}
