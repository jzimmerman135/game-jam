#include "api.h"
#include "raylib.h"
#include "map.h"
#include <stdio.h>

#define FLOPPY_RADIUS 24
#define TUBES_WIDTH 80

#define max(a, b) ((a) > (b) ? (a) : (b))

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
    Vector2 screen = {800, 600};

    Settings settings = {
        .api_version = 0,
        .max_api_version = 0,
        .gameOver = false,
        .pause = false,
    };

   Floppy floppy = {
        .radius = FLOPPY_RADIUS,
        .position = (Vector2){ 0, screen.y / 2.0f },
        .color = (Color){0, 0, 0, 255},
    };

    printf("initialized floppy to pos %f %f\n", floppy.position.x, floppy.position.y);

    Camera2D camera = {0};
    camera.target = (Vector2){ floppy.position.x - 200.f, floppy.position.y + 20.0f };
    camera.offset = (Vector2){ 200.0, screen.y / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Map map;

    // only initialize the map once
    init_map(&map);

    // TODO: temporary needs to fix tube positions
    // for (int i = 0; i < map.nTubes; i++) {
    //     map.tubes[i].rec.y *= 200;
    // }

    *gs = (game_state){
        .screen = screen,
        .settings = settings,
        .floppy = floppy,
        .camera = camera,
        .superfx = false,
        .score = 0,
        .map = map,
        .hiScore = 0,
    };
}

void UpdateGame(game_state *gs)
{
    Settings *settings = &gs->settings;
    if (settings->gameOver)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            printf("RESTARTING\n");
            init(gs);
            return;
        }
        return;
    }

    if (IsKeyPressed('P')) settings->pause = !settings->pause;

    if (settings->pause)
        return;

    // update x offset of rectangles
    gs->floppy.position.x += 2;
    gs->camera.target.x = gs->floppy.position.x;

    if (IsKeyDown(KEY_SPACE))
        gs->floppy.position.y -= 3;
    else
        gs->floppy.position.y += 1;

    // Check Collisions
    for (int i = 0; i < gs->map.nTubes; i++)
    {
        Tubes *tube = &gs->map.tubes[i];
        bool collided = CheckCollisionCircleRec(
            gs->floppy.position, gs->floppy.radius, tube->rec);
        if (collided) {
            settings->gameOver = true;
            settings->pause = false;
            break;
        }
    }
}


void DrawGame(game_state *gs)
{
    ClearBackground(RAYWHITE);

    if (gs->settings.gameOver)
    {
        DrawText("PRESS [ENTER] TO PLAY AGAIN",
                GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2,
                GetScreenHeight()/2 - 50, 20, GRAY);
        return;
    }

    if (gs->settings.pause) {
        DrawText("GAME PAUSED",
                gs->screenWidth/2 - MeasureText("GAME PAUSED", 40)/2,
                gs->screenHeight/2 - 40, 40, GRAY);
        return;
    }

    BeginMode2D(gs->camera);

    DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius, DARKGRAY);
    draw_map(&gs->map);

    EndMode2D();
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

int requested_api_version_id(const game_state *state)
{
    return state->settings.api_version;
}

void set_api_version_id(game_state *state, int version_id) {
    state->settings.api_version = version_id;
    state->settings.max_api_version = max(version_id, state->settings.max_api_version);
}

const game_api shared_obj_api = {
    .open = open,
    .close = close,
    .init = init,
    .step = step,
    .requested_api_version_id = requested_api_version_id,
    .set_api_version_id_callback = set_api_version_id,
    .game_state_size = sizeof(game_state)
};
