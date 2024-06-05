#include "raylib.h"
#include "jump.h"

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
 * In the game hit 'n' to take the pill
 *
 * -------------------------------------------------------------------
 */
 Vector2 update_floppy_velocity(Vector2 prev_velocity, float delta_time, bool did_jump)
 {
    float gravity = -14.0;
    float jump_boost = 1000.0;
    float max_upward_velocity = 400.0;
    Vector2 new_velocity;

    new_velocity.x = prev_velocity.x;
    new_velocity.y = prev_velocity.y + gravity;

    if (IsKeyPressed(KEY_SPACE))
        new_velocity.y = prev_velocity.y + jump_boost;

    // Clamp the velocity to the maximum upward velocity
    new_velocity.y = max(new_velocity.y, max_upward_velocity);

    return new_velocity;
}
