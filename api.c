#include "api.h"
#include "raylib.h"
#include "map.h"
#include <stdio.h>

#define FLOPPY_RADIUS 24
#define TUBES_WIDTH 80

void open(void)
{
    InitWindow(800, 600, "florpheus");
    SetTargetFPS(60);
}

void close(void)
{
    CloseWindow();
}

void init(game_state *gs)
{
    gs->screenWidth = 800;
    gs->screenHeight = 450;

    gs->gameOver = false;
    gs->pause = false;
    gs->score = 0;
    gs->hiScore = 0;

    gs->floppy = (Floppy){
        .radius = FLOPPY_RADIUS,
        .position = (Vector2){80, gs->screenHeight / 2.0 - FLOPPY_RADIUS},
        .color = (Color){0, 0, 0, 255},
    };

    for (int i = 0; i < MAX_TUBES; i++) {
        gs->tubesPos[i].x = 0;
        gs->tubesPos[i].y = 0;
    }

    for (int i = 0; i < MAX_TUBES*2; i++) {
        gs->tubes[i] = (Tubes){
            .color = (Color){0, 0, 0, 255},
            .rec = (Rectangle){0, 0, 0, 0},
            .active = false,
        };
    }

    load_map(gs, NULL);

    gs->tubesSpeedX = 2;
    gs->superfx = false;
    gs->xOffset = 0;
}

static Rectangle rectWithOffset(game_state *gs, Rectangle *r)
{
    Rectangle r_off;
    r_off = *r;
    r_off.x -= gs->xOffset;
    return r_off;
}

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
            }
        }
    } else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            init(gs);
            gs->gameOver = false;
        }
    }
}


static void DrawRectWithOffset(game_state *gs, Rectangle *r, Color clr)
{
    DrawRectangle(r->x - gs->xOffset, r->y, r->width, r->height, clr);
}

void DrawGame(game_state *gs)
{
        ClearBackground(RAYWHITE);

        if (!gs->gameOver)
        {
            // update x offset of rectangles
            gs->xOffset += gs->tubesSpeedX;

            if (IsKeyDown(KEY_SPACE) && !gs->gameOver) gs->floppy.position.y -= 3;

            else gs->floppy.position.y += 1;

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

            // DrawText(TextFormat("xoff: %i", gs->xOffset), 20, 20, 40, GRAY);
            // DrawText(TextFormat("HI-SCORE: %04i", gs->hiScore), 20, 70, 20, LIGHTGRAY);

            if (gs->pause) {
                DrawText("GAME PAUSED",
                        gs->screenWidth/2 - MeasureText("GAME PAUSED", 40)/2,
                        gs->screenHeight/2 - 40, 40, GRAY);
            }
        } else {
            DrawText("PRESS [ENTER] TO PLAY AGAIN",
                    GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2,
                    GetScreenHeight()/2 - 50, 20, GRAY);
        }
}

bool step(game_state *state) {
    BeginDrawing();
    if (IsKeyPressed(KEY_ESCAPE))
        return false;

    UpdateGame(state);
    DrawGame(state);

    EndDrawing();
    return true;
}

int get_api_version_id(const game_state *state)
{
    return -1;
}

void set_api_version_id(game_state *state, int version_id) {
    (void)state;
    (void)version_id;
    return;
}

const game_api shared_obj_api = {
    .open = open,
    .close = close,
    .init = init,
    .step = step,
    .requested_api_version_id = get_api_version_id,
    .set_api_version_id_callback = set_api_version_id,
    .game_state_size = sizeof(game_state)
};
