#include "raylib.h"
#include "raylib.h"
#include "raymath.h"
#include "api.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

/*
 * -------------------------------------------------------------------
 * You take the blue pill, the story ends,
 * you wake up in your bed and believe whatever you want to believe.
 *
 * You take the red pill, you stay in wonderland,
 * and I show you how deep the rabbit hole goes.
 *
 * -------------------------------------------------------------------
 *
 * Change and recompile this code with 'make' to create a pill.
 * When you do you'll see it's ready for you to take.
 *
 * In the game hit 'F' to take the pill
 *
 * -------------------------------------------------------------------
 */

void update_camera(game_state *gs)
{
    gs->camera.target.x = gs->floppy.position.x;
    // gs->camera.target.y = gs->floppy.position.y - gs->screen.y*0.5;
}

 Vector2 update_floppy_velocity(Vector2 prev_velocity, float delta_time, bool did_jump)
 {
    float gravity = -14.;
    float jump_boost = 800.0;
    float max_upward_velocity = 400.0;
    Vector2 new_velocity;

    // gravity = 14.0;
    // jump_boost = 8000.0;
    // max_upward_velocity = 800.0;
    // prev_velocity.x = 0;

    new_velocity.x = prev_velocity.x;
    new_velocity.y = prev_velocity.y + gravity;

    if (IsKeyPressed(KEY_SPACE))
        new_velocity.y = prev_velocity.y + jump_boost;

    // Clamp the velocity to the maximum upward velocity
    new_velocity.y = min(new_velocity.y, max_upward_velocity);

    return new_velocity;
}



// Defined externally
extern const Vector2 floppy_initial_velocity;

Vector2 adjust_scale(Vector2 prev_scale, Vector2 *player_velocity)
{
    const float X_SCALE_TRANSFORM = 3.0;
    player_velocity->x += ((floppy_initial_velocity.x / X_SCALE_TRANSFORM) - player_velocity->x) * 0.08;
    return (Vector2){
        .x = prev_scale.x + (X_SCALE_TRANSFORM - prev_scale.x) * 0.08,
        .y = prev_scale.y,
    };
}

Vector2 apply_transform(Vector2 point, Vector2 scale, Vector2 playerpos)
{
    Vector2 movedpt = Vector2Subtract(point, playerpos);
    return Vector2Add(Vector2Multiply(movedpt, scale), playerpos);
}
