#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

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
    gs->xOffset = 0;
}

int main(void)
{
    init_game_state(&G);
    InitWindow(G.screenWidth, G.screenHeight, "worldshaper");

    InitGame(&G);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateDrawFrame(&G);
    }
    UnloadGame(&G);

    CloseWindow();
    return 0;
}

const int vals[] = {2, 2, 50, 100, 120};
int valpos = 0;

// Initialize game variables
void InitGame(game_state *gs)
{
    gs->floppy.radius = FLOPPY_RADIUS;
    gs->floppy.position = (Vector2){80, gs->screenHeight / 2.0 - gs->floppy.radius};
    gs->tubesSpeedX = 2;

    for (int i = 0; i < MAX_TUBES; i++) {
        gs->tubesPos[i].x = 0;
        gs->tubesPos[i].y = 0;
    }

    //load_map(gs);

    // for (int i = 0; i < gs->nTubes*2; i += 2)
    // {
    //     // gs->tubes[i].rec.x = gs->tubesPos[i/2].x;
    //     // gs->tubes[i].rec.y = gs->tubesPos[i/2].y;
    //     // gs->tubes[i].rec.width = TUBES_WIDTH;
    //     // gs->tubes[i].rec.height = 255;

    //     // gs->tubes[i+1].rec.x = gs->tubesPos[i/2].x;
    //     // gs->tubes[i+1].rec.y = 600 + gs->tubesPos[i/2].y - 255;
    //     // gs->tubes[i+1].rec.width = TUBES_WIDTH;
    //     // gs->tubes[i+1].rec.height = 255;

    //     // gs->tubes[i/2].active = true;
    // }

    gs->score = 0;

    gs->gameOver = false;
    gs->superfx = false;
    gs->pause = false;
    gs->xOffset = 0;
}

static Rectangle rectWithOffset(game_state *gs, Rectangle *r)
{
    Rectangle r_off;
    r_off = *r;
    r_off.x -= gs->xOffset;
    return r_off;
}

// Update game (one frame)
void UpdateGame(game_state *gs)
{
    if (!gs->gameOver)
    {
        if (IsKeyPressed('P')) gs->pause = !gs->pause;

        if (!gs->pause)
        {
            // update x offset of rectangles
            gs->xOffset += gs->tubesSpeedX;

            if (IsKeyDown(KEY_SPACE) && !gs->gameOver) gs->floppy.position.y -= 3;
            else gs->floppy.position.y += 1;

            // Check Collisions
            for (int i = 0; i < gs->nTubes; i++)
            {
                int collide;

                collide = CheckCollisionCircleRec(gs->floppy.position,
                        gs->floppy.radius,
                        rectWithOffset(gs, &gs->tubes[i].rec));

                if (collide) {
                    gs->gameOver = true;
                    gs->pause = false;
                }

                // else if ((gs->tubesPos[i/2].x < gs->floppy.position.x) && gs->tubes[i/2].active && !gs->gameOver) {
                //     gs->score += 100;
                //     gs->tubes[i/2].active = false;

                //     //gs->superfx = true;

                //     if (gs->score > gs->hiScore) gs->hiScore = gs->score;
                // }
            }
        }
    } else
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
            for (int i = 0; i < gs->nTubes; i++) {
                DrawRectWithOffset(gs, &gs->tubes[i].rec, GRAY);
                //DrawRectWithOffset(gs, &gs->tubes[i*2 + 1].rec, GRAY);
            }

            // Draw flashing fx (one frame only)
            // if (gs->superfx) {
            //     DrawRectangle(0, 0, gs->screenWidth, gs->screenHeight, WHITE);
            //     gs->superfx = false;
            // }

            //DrawText(TextFormat("xoff: %i", gs->xOffset), 20, 20, 40, GRAY);


        } else {
            DrawText("PRESS [ENTER] TO PLAY AGAIN",
                    GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2,
                    GetScreenHeight()/2 - 50, 20, GRAY);
        }

    EndDrawing();
}

void UnloadGame(game_state *gs)
{

}

// Update and Draw (one frame)
void UpdateDrawFrame(game_state *gs)
{
    UpdateGame(gs);
    DrawGame(gs);
}
