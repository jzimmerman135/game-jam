#include "api.h"
#include "morpheus.h"
#include "raymath.h"
#include "raylib.h"
#include "map.h"
#include "palette.h"
#include <stdio.h>
#include <stdlib.h>
#include "powerups.h"
#include "jump.h"
#include "types.h"
#include "intro.h"

// cheat codes
//#define ZOOMITOUT
//#define GODMODE

#define FLOPPY_RADIUS 24
#define TUBES_WIDTH 80


#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

float clamp(float value, float min, float max)
{
    return value < min ? min : value > max ? max : value;
}

void open(void)
{
    if (getenv("EDITOR") == NULL) {
        fprintf(stderr, "EDITOR environment variable must be set to play\n");
        exit(1);
    }

    InitWindow(800, 600, "florpheus");
    SetExitKey(0);
    SetTargetFPS(60);
}

void close(void)
{
    CloseWindow();
}

const Vector2 floppy_initial_position = { 0, 300 };
const Vector2 floppy_initial_velocity = { 400.0f, 0.0f };
const Vector2 camera_offset = { 200.0f, 0.0f };

Camera2D init_camera(Vector2 floppy_position) {
    return (Camera2D){
#ifndef ZOOMITOUT
        .offset = (Vector2){ 200.0f, 0 },
#else
        .offset = (Vector2){ 200.0f, 200.0 },
#endif
        .target = (Vector2){ floppy_position.x, 0.0 },
        .rotation = 0.0f,
#ifndef ZOOMITOUT
        .zoom = 1.0f,
#else
        .zoom = 0.2f,
#endif
    };
}

void init(game_state *gs)
{
    Vector2 screen = {800, 600};

    Morpheus morpheus = {
        .last_said = -1,
        .shutup = false,
        .statement_id = 0,
    };

    Settings settings = {
        .api_version = 0,
        .max_api_version = 0,
        .gameOver = false,
        .pause = false,
    };

    Floppy floppy = (Floppy){
        .radius = FLOPPY_RADIUS,
        .velocity = floppy_initial_velocity,
        .position = floppy_initial_position,
    };

    Camera2D camera = init_camera(floppy.position);
    Map map;
    init_map(&map);

    Assets assets;
    assets.textures[0] = LoadTexture("bits.png");

    Intro intro;
    intro_setup(&intro);

    *gs = (game_state){
        .screen = screen,
        .settings = settings,
        .floppy = floppy,
        .camera = camera,
        .map = map,
        .delta = 0.0,
        .elapsed = gs->elapsed,
        .powerups = (Powerups){0},
        .assets = assets,
        .intro = intro,
        .morpheus = morpheus
    };
}

static int how_did_it_collide(game_state *gs, Tubes *tb) {
    // TODO: implement
    return 0;
}

void handle_tube_collision(game_state *gs, Tubes *tb)
{
    switch (tb->type) {
        case TUBE_PLATFORM: {
            int direction = how_did_it_collide(gs, tb);
            if (direction == 0) {
                /* upper: snap to right above */
                gs->floppy.position.y = tb->rec.y - gs->floppy.radius;
                gs->floppy.velocity.y = 0;
            }
        }
            break;
        case TUBE_TOGGLE:
            tb->toggled = 1;
            gs->map.visibility = 1;
            break;
        case TUBE_BLUE:
            if (gs->map.visibility != 0)
                break;
        case TUBE_DEATH:
        default: {
#ifndef GODMODE
            gs->settings.gameOver = true;
            gs->settings.pause = true;
#endif
            break;
        }
    }
}

void reset(game_state *game)
{
    game->settings.gameOver = false;
    game->settings.pause = false;
    game->settings.api_version = 0;
    game->floppy.position = floppy_initial_position;
    game->floppy.velocity = floppy_initial_velocity;
    game->camera = init_camera(game->floppy.position);
}

Vector2 flip_y(Vector2 v) {
    return (Vector2){v.x, -v.y};
}

void UpdateGame(game_state *gs)
{
    if (gs->intro.running) {
        intro_update(&gs->intro);
        return;
    }

    gs->delta = GetFrameTime();
    gs->elapsed = GetTime();
    update_morpheus(&gs->morpheus);

    Settings *settings = &gs->settings;

    if (settings->gameOver) {
        if (IsKeyPressed(KEY_ENTER))
            reset(gs);
        return;
    }

    if (IsKeyPressed(KEY_E)) {
        gs->morpheus.shutup = true;
        int srcfile = relevant_src_file_id_from_world_pos(&gs->map, gs->floppy.position);
        char *filename = decode_fileid(srcfile);
        try_open_text_editor(&gs->settings, filename);
    }

    if (IsKeyPressed('P')) settings->pause = !settings->pause;
    if (settings->pause)
        return;

    if (IsKeyPressed(KEY_K))
        settings->gameOver = true;

    if (IsKeyPressed('F') && settings->api_changed) {
        place_powerup(&gs->powerups, gs->floppy.position, ++settings->max_api_version);
        gs->settings.api_changed = false;
    }


    // Positive real velocity unintuitively goes downward, so we abstract this away to user
    gs->floppy.velocity = flip_y(gs->floppy.velocity);
    gs->floppy.velocity = update_floppy_velocity(gs->floppy.velocity, gs->delta, IsKeyPressed(KEY_SPACE));
    gs->floppy.velocity = flip_y(gs->floppy.velocity);

    gs->floppy.position = (Vector2){
        gs->floppy.position.x + gs->floppy.velocity.x * gs->delta,
        gs->floppy.position.y + gs->floppy.velocity.y * gs->delta
    };


    gs->camera.target.x = gs->floppy.position.x;

    // Check Pill Collisions
    for (int i = 0; i < gs->powerups.nPowerups; i++) {
        Powerup *powerup = &gs->powerups.powerup[i];
        bool collided = CheckCollisionCircles(
            gs->floppy.position, gs->floppy.radius, powerup->position, powerup_radius);
        if (collided) {
            gs->settings.api_version = powerup->api_version_id;
            break;
        }
    }

    // Check Tube Collisions
    for (int i = 0; i < gs->map.nTubes; i++) {
        Tubes *tube = &gs->map.tubes[i];
        bool collided = CheckCollisionCircleRec(
            gs->floppy.position, gs->floppy.radius, tube->rec);
        if (collided) {
            handle_tube_collision(gs, tube);
        }
    }
}

void DrawGame(game_state *gs)
{
    if (gs->intro.running) {
        draw_intro_screen(&gs->intro);
        return;
    }

    draw_background(gs->assets, gs->camera, gs->settings.api_version);

    char buf[256];
    snprintf(buf, 256, "api_version %d", gs->settings.api_version);
    DrawText(buf, 0, 0, 20, gs->settings.api_changed ? BLUE : RED);

    if (gs->settings.gameOver) {
        DrawText("PRESS [ENTER] TO PLAY AGAIN",
            gs->screen.x / 2.0 - (float)MeasureText("PRESS [ENTER] TO PLAY AGAIN", 40)/2,
            gs->screen.y / 2.0 - 40, 40, RED);
        return;
    }

    if (gs->settings.pause) {
        DrawText("GAME PAUSED",
                gs->screen.x / 2.0 - (float)MeasureText("GAME PAUSED", 40)/2,
                gs->screen.y / 2.0 - 40, 40, RED);
    }

    BeginMode2D(gs->camera);

    draw_map(&gs->map);

    for (int i = 0; i < gs->powerups.nPowerups; i++) {
        draw_powerup(gs->powerups.powerup[i]);
    }

    {
        DrawCircle(gs->floppy.position.x+4, gs->floppy.position.y+4, gs->floppy.radius*1.3, get_color(COLOR_TUBE_SHADOW));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius*1.3, get_color(COLOR_AVATAR_BORDER2));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius*1.2, get_color(COLOR_AVATAR_BORDER1));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius, get_color(COLOR_AVATAR));
        if (gs->settings.api_changed) {
            Powerup p = (Powerup){
                .position = Vector2Add(gs->floppy.position, (Vector2){24, 10}),
                .color = RED,
            };
            draw_powerup(p);
        }
    }

    EndMode2D();

    draw_secret_message(&gs->morpheus, gs->elapsed);
}

bool step(game_state *state) {
    BeginDrawing();
    if (WindowShouldClose())
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

void set_api_changed(game_state *state)
{
    state->settings.api_changed = true;
}

const game_api shared_obj_api = {
    .open = open,
    .close = close,
    .init = init,
    .step = step,
    .requested_api_version_id = requested_api_version_id,
    .game_state_size = sizeof(game_state),
    .api_changed_callback = set_api_changed,
};
