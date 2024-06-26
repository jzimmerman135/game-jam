#include "api.h"
#include "morpheus.h"
#include "raymath.h"
#include "raylib.h"
#include "map.h"
#include "palette.h"
#include <stdio.h>
#include <stdlib.h>
#include "powerups.h"
#include "types.h"
#include "intro.h"

#define DEATH_BOUNDARY 4908.29
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

static void draw_win(game_state *gs);


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
    SetTargetFPS(60);

    while (!IsKeyPressed(KEY_SPACE)) {
        BeginDrawing();

        if (WindowShouldClose())
            return;

        ClearBackground(get_color(COLOR_AVATAR));
        float h = GetScreenHeight();
        float w = GetScreenWidth();
        Vector2 ctr = (Vector2){w * 0.25, h * 0.25};
        DrawText("THE", ctr.x / 2. + 4, ctr.y + 4, 100, get_color(COLOR_TUBE_SHADOW));
        DrawText("THE", ctr.x / 2., ctr.y, 100, WHITE);
        DrawText("FLAPTRIX", ctr.x / 2. + 4, ctr.y + 4 + 80, 100, get_color(COLOR_TUBE_SHADOW));
        DrawText("FLAPTRIX", ctr.x / 2., ctr.y + 80, 100, WHITE);
        DrawText("Press SPACE to start", ctr.x + 180, ctr.y * 2.2, 20, GRAY);

        DrawText("Controls:", ctr.x - 100 + 2, ctr.y * 2.9 + 2, 20, get_color(COLOR_TUBE_SHADOW));
        DrawText("Controls:", ctr.x - 100, ctr.y * 2.9, 20, WHITE);
        DrawText("'SPACE' to jump", ctr.x - 100, ctr.y * 3.1, 20, GRAY);
        DrawText("'P' to pause", ctr.x - 100, ctr.y * 3.3, 20, GRAY);
        DrawText("'C' to set checkpoint", ctr.x - 100, ctr.y * 3.5, 20, GRAY);

        EndDrawing();
    }

    SetExitKey(0);
}

void close(void)
{
    CloseWindow();
}

const Vector2 floppy_initial_position = { 0, 300 };
const Vector2 floppy_initial_velocity = { 400.0f, 0.0f };
const Vector2 camera_offset = { 200.0f, 0.0f };

const Checkpoint initial_checkpoint = {
    .position = { 0, 300 },
    .api_version = 0,
};

Camera2D init_camera(Vector2 floppy_position) {
    return (Camera2D){
#ifndef ZOOMITOUT
        .offset = (Vector2){ 200.0f, 0 },
#else
        .offset = (Vector2){ 200.0f, 300.0 },
#endif
        .target = (Vector2){ floppy_position.x, 0.0 },
        .rotation = 0.0f,
#ifndef ZOOMITOUT
        .zoom = 1.0f,
#else
        .zoom = 0.1f,
#endif
    };
}

void init(game_state *gs)
{
    Vector2 screen = {800, 600};

    Morpheus morpheus = {
        .last_said = -1,
        .shutup = false,
        .statement_id = -1,
    };

    Settings settings = {
        .api_version = 0,
        .max_api_version = 0,
        .gameOver = false,
        .pause = false,
        .win = false,
        .level = 0,
        .new_level = false,
    };

    Floppy floppy = (Floppy){
        .radius = FLOPPY_RADIUS,
        .velocity = floppy_initial_velocity,
        .position = floppy_initial_position,
        .is_bulldozin = false,
    };

    Camera2D camera = init_camera(floppy.position);
    Map map;
    init_map(&map, 0);

    Assets assets;
    assets.textures[0] = LoadTexture("bits.png");

    Intro intro;
    intro_setup(&intro);

    Powerups powerups = {
        .n_powerups = 1,
        .active_powerup = 0,
        .powerup = {0},
    };

    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups.powerup[i].api_version_id = -1000;
        powerups.powerup[i].color = powerup_palette[i % N_POWERUP_PALETTES];
    }
    powerups.powerup[0].api_version_id = 0;
    powerups.powerup[0].color = BEIGE;


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
        .intro = intro,
        .morpheus = morpheus,
        .last_checkpoint = initial_checkpoint,
        .n_deaths_total = 0,
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
        case TUBE_WINNER:
            gs->settings.win = true;
                break;
        case TUBE_DEATH:
        default: {
#ifndef GODMODE
            gs->settings.gameOver = true;
            gs->settings.pause = true;
            gs->n_deaths_total++;

            if (gs->floppy.position.x >= DEATH_BOUNDARY) {
                gs->morpheus.fleo_is_ready = true;
            }
#endif
            break;
        }
    }
}

void reset(game_state *game)
{
    game->map.scale = (Vector2){1.0, 1.0};
    game->settings.gameOver = false;
    game->settings.pause = false;
    game->floppy.position = floppy_initial_position;
    game->floppy.velocity = floppy_initial_velocity;
    game->camera = init_camera(game->floppy.position);
    game->settings.api_version = 0;
    game->powerups.active_powerup = 0;
    game->floppy.position = game->last_checkpoint.position;
}

Vector2 flip_y(Vector2 v) {
    return (Vector2){v.x, -v.y};
}

void update_camera(game_state *gs);

void UpdateGame(game_state *gs)
{
    Settings *settings = &gs->settings;

    if (gs->intro.running) {
        intro_update(&gs->intro);
        return;
    }

    if (settings->win) {
        settings->win = false;
        settings->level++;

        if (settings->level >= NLEVELS) {
            settings->level = NLEVELS;
            settings->win = true;
            return;
        } else {
            init_map(&gs->map, settings->level);
            gs->last_checkpoint = (Checkpoint){.position = floppy_initial_position, .api_version = 0};
            reset(gs);
        }
    }

    if (gs->settings.win) return;

    gs->delta = GetFrameTime();
    gs->elapsed = GetTime();
    update_morpheus(&gs->morpheus);


    // if (IsKeyPressed(KEY_K)) { // kill switch
    //     settings->gameOver = true;
    //     gs->last_checkpoint = initial_checkpoint;
    // }

    /* Are effective when pausing */

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

    gs->map.scale = adjust_scale(gs->map.scale, &gs->floppy.velocity);

    if (IsKeyPressed('C')) {
        gs->last_checkpoint.position = gs->floppy.position;
        gs->last_checkpoint.api_version = gs->settings.api_version;
    }

    if (IsKeyPressed('J')) {
        gs->powerups.active_powerup = max(gs->powerups.active_powerup - 1, 0);
        gs->settings.api_version = gs->powerups.powerup[gs->powerups.active_powerup].api_version_id;
    } else if (IsKeyPressed('K')) {
        gs->powerups.active_powerup = min(gs->powerups.active_powerup + 1, gs->powerups.n_powerups - 1);
        gs->settings.api_version = gs->powerups.powerup[gs->powerups.active_powerup].api_version_id;
    }

    if (IsKeyPressed('F') && settings->api_changed) {
        place_powerup(&gs->powerups, gs->floppy.position, ++settings->max_api_version);
        gs->settings.api_changed = false;
    }

    if (IsKeyPressed('P'))
        settings->pause = !settings->pause;

    if (settings->pause)
        return;

    /* Rest are unaffected by pausing */

    bool start_secret_message =
        gs->morpheus.last_said == -1 &&
        gs->n_deaths_total >= 1 &&
        // gs->elapsed >= 20.0 &&
        gs->morpheus.fleo_is_ready &&
        gs->floppy.position.x > 300.0;

    if (start_secret_message) {
        gs->morpheus.statement_id = 0;
    }

    // Positive real velocity unintuitively goes downward, so we abstract this away to user
    gs->floppy.velocity = flip_y(gs->floppy.velocity);
    gs->floppy.velocity = update_floppy_velocity(gs->floppy.velocity, gs->delta, IsKeyPressed(KEY_SPACE));
    gs->floppy.velocity = flip_y(gs->floppy.velocity);

    gs->floppy.position = (Vector2){
        gs->floppy.position.x + gs->floppy.velocity.x * gs->delta,
        gs->floppy.position.y + gs->floppy.velocity.y * gs->delta
    };

    update_camera(gs);

    // Check Pill Collisions
    // for (int i = 0; i < gs->powerups.nPowerups; i++) {
    //     Powerup *powerup = &gs->powerups.powerup[i];
    //     bool collided = CheckCollisionCircles(
    //         gs->floppy.position, gs->floppy.radius, powerup->position, powerup_radius);
    //     if (!collided)
    //         continue;
    //     if (!gs->floppy.is_bulldozin) { // TODO: REMOVE IS BULLDOZIN EVERYWHERE
    //         gs->settings.api_version = powerup->api_version_id;
    //     } else {
    //         destroy_powerup(&gs->powerups, powerup->api_version_id);
    //     }
    //     break;
    // }

    // Check Tube Collisions
    for (int i = 0; i < gs->map.nTubes; i++) {
        Tubes *tube = &gs->map.tubes[i];
        bool collided = CheckCollisionCircleRec(
            gs->floppy.position, gs->floppy.radius, transform_rec(tube->rec, gs->map.scale, gs->floppy.position));
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

    //if (gs->settings.win) {
    //    draw_win(gs);
    //    return;
    //}

    draw_background(gs->assets, gs->camera, gs->settings.api_version, gs->map.scale);

    // char buf[256];
    // snprintf(buf, 256, "api_version %d", gs->settings.api_version);
    // DrawText(buf, 0, 0, 20, gs->settings.api_changed ? BLUE : RED);

    BeginMode2D(gs->camera);

    Vector2 origin = (Vector2){gs->floppy.position.x, gs->floppy.position.y};
    draw_map(&gs->map, origin);

    // for (int i = 0; i < gs->powerups.nPowerups; i++)
    //     draw_powerup(gs->powerups.powerup[i], gs->map.scale, gs->floppy.position);

    { // draw floppy
        DrawCircle(gs->floppy.position.x+4, gs->floppy.position.y+4, gs->floppy.radius*1.3, get_color(COLOR_TUBE_SHADOW));
        if (gs->floppy.is_bulldozin)
            DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius*1.5, ColorAlpha(RED, 0.4));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius*1.3, get_color(COLOR_AVATAR_BORDER2));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius*1.2, get_color(COLOR_AVATAR_BORDER1));
        DrawCircle(gs->floppy.position.x, gs->floppy.position.y, gs->floppy.radius, get_color(COLOR_AVATAR));
        if (gs->settings.api_changed) {
            Powerup p = (Powerup){
                .position = Vector2Add(gs->floppy.position, (Vector2){24, 10}),
                .color = RED,
            };
            draw_powerup(p.position, p.color, (Vector2){1.0, 1.0}, gs->floppy.position);
        }
    }

    EndMode2D();

    if (gs->powerups.n_powerups > 1 || IsKeyDown('D')) {
        Vector2 barpos = { gs->screen.x / 2., gs->screen.y - 40 };
        Vector2 barwh = { 60 * 5.1, 50 };
        Color c = ColorAlpha(DARKGRAY, 0.4);
        DrawRectangleV(Vector2Subtract(barpos, Vector2Scale(barwh, 0.5)), barwh, c);
        for (int i = 0; i < MAX_POWERUPS; i++) {
            Vector2 pctr = { (i - 2) * 60.0 + barpos.x, barpos.y };
            Vector2 rxy = { pctr.x - 25, pctr.y - 20 };
            Vector2 rwh = { 50, 40 };

            Powerup p = gs->powerups.powerup[i];

            if (i == gs->powerups.active_powerup) {
                DrawRectangleV(Vector2AddValue(rxy, -2), Vector2AddValue(rwh, 4), LIGHTGRAY);
                DrawRectangleV(rxy, rwh, DARKGRAY);
            } else {
                DrawRectangleV(rxy, rwh, c);
            }

            if (i < gs->powerups.n_powerups)
                draw_powerup(pctr, p.color, (Vector2){1, 1}, (Vector2){0, 0});
        }
    }

    draw_secret_message(&gs->morpheus, gs->elapsed);

    if (gs->settings.gameOver) {
        float textlen = (float)MeasureText("PRESS [ENTER] TO PLAY AGAIN", 40);
        float x = gs->screen.x / 2.0 - textlen / 2.0;
        float y = gs->screen.y / 2.0 - 40;
        DrawRectangle(x - 11, y - 11, textlen + 22, 40 + 22, BLACK);
        DrawRectangle(x - 5, y - 5, textlen + 20, 40 + 20, get_color(COLOR_TUBE_SHADOW));
        DrawRectangle(x - 10, y - 10, textlen + 20, 40 + 20, get_color(COLOR_AVATAR));
        DrawText("PRESS [ENTER] TO PLAY AGAIN", x + 4, y + 4, 40, get_color(COLOR_TUBE_SHADOW));
        DrawText("PRESS [ENTER] TO PLAY AGAIN", x, y, 40, WHITE);
    } else if (gs->settings.pause) {
        Vector2 xy1 = {gs->screen.x / 2. - 80., gs->screen.y / 2. - 75.};
        Vector2 xy2 = {gs->screen.x / 2. + 20., gs->screen.y / 2. - 75.};
        Vector2 wh = { 50., 150.};
        Vector2 offset = { 5, 5 };
        DrawRectangleV(Vector2Scale(gs->screen, 0.25), Vector2Scale(gs->screen, 0.5), ColorAlpha(GRAY, 0.6));
        DrawRectangleV(Vector2Add(xy1, offset), Vector2Add(wh, offset), BLACK);
        DrawRectangleV(xy1, wh, WHITE);
        DrawRectangleV(Vector2Add(xy2, offset), Vector2Add(wh, offset), BLACK);
        DrawRectangleV(xy2, wh, WHITE);
    }
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

static void draw_win(game_state *gs)
{
    Color bgcolor = (Color){0xff, 0x69, 0xb4, 0xff};
    const char *msg = "You Win!!1!";
    double center_x, center_y;
    double sz;

    sz = 60.0;

    center_y = (gs->screen.y * 0.5) - (sz * 0.5);
    center_x = (gs->screen.x * 0.5);
    ClearBackground(bgcolor);
    Color fgcolor = (Color){0x4e, 0xDc, 0x4e, 0xff};
    center_x -= MeasureText(msg, sz)*0.5;
    DrawText(msg, center_x, center_y, sz, fgcolor);
}
