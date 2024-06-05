#ifndef JUMP_H
#define JUMP_H

#include "raylib.h"

Vector2 update_floppy_velocity(Vector2 prev_velocity, float delta_time, bool did_jump);

#endif
