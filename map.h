#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "raymath.h"
#include "types.h"
#include "powerups.h"

void init_map(Map *map);
void draw_map(Map *map, Vector2 origin);

Rectangle transform_rec(Rectangle r, Vector2 scale, Vector2 origin);
src_file_id relevant_src_file_id_from_world_pos(const Map *map, Vector2 playerpos);

#endif
