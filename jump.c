#include "raylib.h"
#include "api.h"
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

void update_camera(game_state *gs)
{
    gs->camera.target.x = gs->floppy.position.x;
    //gs->camera.target.y = gs->floppy.position.y - gs->screen.y*0.5;
}

 Vector2 update_floppy_velocity(Vector2 prev_velocity, float delta_time, bool did_jump)
 {
    float gravity = -14.0;
    float jump_boost = 1000.0;
    float max_upward_velocity = 400.0;
    float next_velocity_x = prev_velocity.x;
    Vector2 new_velocity;

    //gravity = 14.0;
    //jump_boost = 8000.0;
    //max_upward_velocity = 800.0;
    //next_velocity_x = 0;

    new_velocity.x = next_velocity_x;
    new_velocity.y = prev_velocity.y + gravity;

    if (IsKeyPressed(KEY_SPACE))
        new_velocity.y = prev_velocity.y + jump_boost;

    // Clamp the velocity to the maximum upward velocity
    new_velocity.y = min(new_velocity.y, max_upward_velocity);

    return new_velocity;
}
