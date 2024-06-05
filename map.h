#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "types.h"
#include "powerups.h"

void init_map(Map *map);
void draw_map(Map *map);

src_file_id relevant_src_file_id_from_world_pos(const Map *map, Vector2 playerpos);

#endif
