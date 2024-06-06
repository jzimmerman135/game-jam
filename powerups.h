#ifndef POWERUPS_H
#define POWERUPS_H

#include "raylib.h"
#include "types.h"
#include <assert.h>

typedef enum {
   FIRST_FILE,
} src_file_id;

extern const float powerup_radius;

void place_powerup(Powerups *powerups, Vector2 pos, int id);
void destroy_powerup(Powerups *powerups, int id);

char *decode_fileid(src_file_id fileid);
bool try_open_text_editor(Settings *settings, char *filename);

void draw_powerup(Powerup p);

void draw_background(Assets assets, Camera2D camera, int api_version, Vector2 mapscale);

#endif
