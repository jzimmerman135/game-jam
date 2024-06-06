#include "powerups.h"
#include "api.h"
#include "raylib.h"
#include "types.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "palette.h"
#include <termios.h>
#include "raymath.h"

const float powerup_radius = 10.0;

const Color powerup_palette[] = {
    SKYBLUE,
    PURPLE,
    BLUE,
    GREEN,
    MAGENTA,
};

const int N_POWERUP_PALETTES = sizeof(powerup_palette) / sizeof(Color);

void place_powerup(Powerups *powerups, Vector2 pos, int id) {
    if (powerups->n_powerups == MAX_POWERUPS && powerups->active_powerup == 0) {
        powerups->powerup[MAX_POWERUPS - 1].api_version_id = id;
        powerups->powerup[MAX_POWERUPS - 1].color = powerup_palette[id % N_POWERUP_PALETTES];
    } else if (powerups->n_powerups == MAX_POWERUPS) {
        powerups->powerup[powerups->active_powerup].api_version_id = id;
        powerups->powerup[powerups->active_powerup].color = powerup_palette[id % N_POWERUP_PALETTES];
    } else {
        powerups->powerup[powerups->n_powerups].api_version_id = id;
        powerups->powerup[powerups->n_powerups].color = powerup_palette[id % N_POWERUP_PALETTES];
        powerups->n_powerups++;
    }
}

char *decode_fileid(src_file_id fileid) {
    switch (fileid) {
        case FIRST_FILE:
            return "jump.c";
        default:
            return NULL;
    }
}

bool try_open_text_editor(Settings *settings, char *filename) {
    if (filename == NULL)
        return true;

    int pid = fork();
    if (pid == 0) {
        char buf[256];
        snprintf(buf, 256, "$EDITOR %s\n", filename);
        execlp(getenv("EDITOR"), getenv("EDITOR"), filename, NULL);
        perror("execlp");
        exit(1);
    }

    return true;
}

void draw_powerup(Vector2 pos, Color c, Vector2 mapscale, Vector2 origin)
{
    pos = apply_transform(pos, mapscale, origin);
    DrawEllipse(pos.x, pos.y, 20.0, powerup_radius, c);
    DrawEllipse(pos.x - 6, pos.y - 5, 20.0 / 4., powerup_radius / 4., ColorAlpha(WHITE, 0.5));
}

void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Vector2 scale, Color tint);

void draw_background(Assets assets, Camera2D camera, int api_version, Vector2 mapscale) {
    Color backgroundcol = background_palette[api_version % N_BACKGROUND_PALETTES];
    ClearBackground(backgroundcol);
    DrawTextureTiled(
        assets.textures[0],
        (Rectangle){ 0, 0, 32, 30 },
        (Rectangle){ 0, 0, 820, 600 },
        (Vector2){ fmodf(camera.target.x / 4.0, 32.0f), 0 },
        0.0,
        Vector2Scale(mapscale, 5.0),
        ColorAlpha(background_palette[api_version % N_BACKGROUND_PALETTES], 0.04)
    );
}


// Draw part of a texture (defined by a rectangle) with rotation and scale tiled into dest.
// Code taken from raylib/examples/textures/texture_tiling.c
void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Vector2 scale, Color tint)
{
    if ((texture.id <= 0) || (scale.x <= 0.0f)) return;  // Wanna see a infinite loop?!...just delete this line!
    if ((source.width == 0) || (source.height == 0)) return;

    int tileWidth = (int)(source.width * scale.x), tileHeight = (int)(source.height * scale.y);
    if ((dest.width < tileWidth) && (dest.height < tileHeight))
    {
        // Can fit only one tile
        DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, ((float)dest.height/tileHeight)*source.height},
                    (Rectangle){dest.x, dest.y, dest.width, dest.height}, origin, rotation, tint);
    }
    else if (dest.width <= tileWidth)
    {
        // Tiled vertically (one column)
        int dy = 0;
        for (;dy+tileHeight < dest.height; dy += tileHeight)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, source.height}, (Rectangle){dest.x, dest.y + dy, dest.width, (float)tileHeight}, origin, rotation, tint);
        }

        // Fit last tile
        if (dy < dest.height)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                        (Rectangle){dest.x, dest.y + dy, dest.width, dest.height - dy}, origin, rotation, tint);
        }
    }
    else if (dest.height <= tileHeight)
    {
        // Tiled horizontally (one row)
        int dx = 0;
        for (;dx+tileWidth < dest.width; dx += tileWidth)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, source.width, ((float)dest.height/tileHeight)*source.height}, (Rectangle){dest.x + dx, dest.y, (float)tileWidth, dest.height}, origin, rotation, tint);
        }

        // Fit last tile
        if (dx < dest.width)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, ((float)dest.height/tileHeight)*source.height},
                        (Rectangle){dest.x + dx, dest.y, dest.width - dx, dest.height}, origin, rotation, tint);
        }
    }
    else
    {
        // Tiled both horizontally and vertically (rows and columns)
        int dx = 0;
        for (;dx+tileWidth < dest.width; dx += tileWidth)
        {
            int dy = 0;
            for (;dy+tileHeight < dest.height; dy += tileHeight)
            {
                DrawTexturePro(texture, source, (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth, (float)tileHeight}, origin, rotation, tint);
            }

            if (dy < dest.height)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                    (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth, dest.height - dy}, origin, rotation, tint);
            }
        }

        // Fit last column of tiles
        if (dx < dest.width)
        {
            int dy = 0;
            for (;dy+tileHeight < dest.height; dy += tileHeight)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, source.height},
                        (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx, (float)tileHeight}, origin, rotation, tint);
            }

            // Draw final tile in the bottom right corner
            if (dy < dest.height)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                    (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx, dest.height - dy}, origin, rotation, tint);
            }
        }
    }
}
