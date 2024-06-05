#ifndef JUMP_H
#define JUMP_H

#include "raylib.h"

Vector2 floppy_velocity(bool did_jump, Vector2 prev_velocity, float delta_time);

#endif
