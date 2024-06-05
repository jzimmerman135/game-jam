#include "api.h"
#include "raylib.h"
#include "map.h"
#include "palette.h"
#include <stdio.h>
#include <stdlib.h>
#include "powerups.h"
#include "types.h"
#include "intro.h"

#define FLOPPY_RADIUS 24
#define TUBES_WIDTH 80

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

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
#define GRAVITY 800.0
#define JUMP_VELOCITY -500.0

Camera2D init_camera(Vector2 floppy_position) {
    return (Camera2D){
        .offset = (Vector2){ 200.0f, 0 },
        .target = (Vector2){ floppy_position.x, 0.0 },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };
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

    Floppy floppy = (Floppy){
        .radius = FLOPPY_RADIUS,
        .velocity = floppy_initial_velocity,
        .position = floppy_initial_position,
    };

    Camera2D camera = init_camera(floppy.position);
    Map map;
    init_map(&map);

    Powerups powerups = {0};
    powerups.radius = 10;

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
        .powerups = powerups,
        .assets = assets,
        .intro = intro
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
                gs->floppy.position.y =
                    tb->rec.y -
                    gs->floppy.radius*1.1;
            }
        }
            break;
        case TUBE_TOGGLE:
            gs->map.visibility = 1;
            break;
        case TUBE_BLUE:
            if (gs->map.visibility != 0)
                break;
        case TUBE_DEATH:
        default: {
            gs->settings.gameOver = true;
            gs->settings.pause = true;
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

void UpdateGame(game_state *gs)
{
    if (gs->intro.running) {
        intro_update(&gs->intro);
        return;
    }
    gs->delta = GetFrameTime();
    gs->elapsed = GetTime();

    Settings *settings = &gs->settings;
    if (settings->gameOver) {
        if (IsKeyPressed(KEY_ENTER))
            reset(gs);
        return;
    }

    if (IsKeyPressed(KEY_D)) {
        printf("npowers %d, rad %d\n", gs->powerups.nPowerups, gs->powerups.radius);
        printf("api %d, maxapi %d\n", gs->settings.api_version, gs->settings.max_api_version);
    }

    if (IsKeyPressed('P')) settings->pause = !settings->pause;
    if (settings->pause)
        return;

    if (IsKeyPressed(KEY_E)) {
        int srcfile = relevant_src_file_id_from_world_pos(&gs->map, gs->floppy.position);
        char *filename = decode_fileid(srcfile);
        try_open_text_editor(filename);
    }

    if (IsKeyPressed(KEY_N) && settings->api_changed) {
        printf("started it here\n");
        place_powerup(&gs->powerups, gs->floppy.position, ++settings->max_api_version);
        printf("made it here\n");
        gs->settings.api_changed = false;
    }

    float gravity = GRAVITY;
    gs->floppy.velocity.y += gravity*gs->delta;

    gs->floppy.position = (Vector2){
        gs->floppy.position.x + gs->floppy.velocity.x * gs->delta,
        gs->floppy.position.y + gs->floppy.velocity.y * gs->delta
    };

    if (IsKeyPressed(KEY_SPACE)) {
        // TODO: CONFIGURE NICE JUMP KINEMATICS
        //gs->floppy.velocity.y = max(gs->floppy.velocity.y - 400., -300);
        //gs->floppy.velocity.y = max(gs->floppy.velocity.y - 400., -300);
        gs->floppy.velocity.y = JUMP_VELOCITY;
    }

    gs->camera.target.x = gs->floppy.position.x;

    // Check Collisions
    for (int i = 0; i < gs->powerups.nPowerups; i++) {
        Powerup *powerup = &gs->powerups.powerup[i];
        bool collided = CheckCollisionCircles(
            gs->floppy.position, gs->floppy.radius, powerup->position, gs->powerups.radius);
        if (collided) {
            gs->settings.api_version = powerup->api_version_id;
            break;
        }
    }

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
    ClearBackground(get_color(COLOR_BACKGROUND));

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


    draw_background(gs->assets, gs->camera, gs->settings.api_version);

    BeginMode2D(gs->camera);

    draw_map(&gs->map);
    draw_powerups(&gs->powerups);

    {
        DrawCircle(gs->floppy.position.x+4, gs->floppy.position.y+4, gs->floppy.radius*1.3, get_color(COLOR_TUBE_SHADOW));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius*1.3, get_color(COLOR_AVATAR_BORDER2));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius*1.2, get_color(COLOR_AVATAR_BORDER1));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius, get_color(COLOR_AVATAR));
        if (gs->settings.api_changed)
            DrawCircle(gs->floppy.position.x + 32.0, gs->floppy.position.y + 8.0, gs->powerups.radius, ORANGE);
    }

    EndMode2D();
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
