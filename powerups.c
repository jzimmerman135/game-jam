#include "powerups.h"
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

void place_powerup(Powerups *powerups, Vector2 pos, int id) {
    if (powerups->nPowerups > MAX_POWERUPS) {
        powerups->powerup[0] = powerups->powerup[--powerups->nPowerups];
    }
    powerups->powerup[powerups->nPowerups].position = pos;
    powerups->powerup[powerups->nPowerups].api_version_id = id;
    powerups->powerup[powerups->nPowerups].color = RED;
    powerups->nPowerups++;
}

void destroy_powerup(Powerups *powerups, int id) {
    for (int i = 0; i < powerups->nPowerups % MAX_POWERUPS; i++) {
        if (powerups->powerup[i].api_version_id == id) {
            powerups->powerup[i] = powerups->powerup[--powerups->nPowerups];
        }
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
        // struct termios orig_term;
        // tcgetattr(0, &orig_term);
        char buf[256];
        snprintf(buf, 256, "$EDITOR %s\n", filename);
        execlp(getenv("EDITOR"), getenv("EDITOR"), filename, NULL);
        perror("execlp");
        exit(1);
        // tcsetattr(0, TCSAFLUSH, &orig_term);
    }

    return true;
}

void draw_powerup(Powerup p)
{
    DrawEllipse(p.position.x, p.position.y, 20.0, powerup_radius, p.color);
    DrawEllipse(p.position.x - 6, p.position.y - 5, 20.0 / 4., powerup_radius / 4., ColorAlpha(WHITE, 0.5));
}

void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint);

void draw_background(Assets assets, Camera2D camera, int api_version) {
    Color backgroundcol = background_palette[api_version % N_BACKGROUND_PALETTES];
    ClearBackground(backgroundcol);
    DrawTextureTiled(
        assets.textures[0],
        (Rectangle){ 0, 0, 32, 30 },
        (Rectangle){ 0, 0, 820, 600 },
        (Vector2){ fmodf(camera.target.x / 4.0, 32.0f), 0 },
        0.0,
        5.0,
        ColorAlpha(background_palette[api_version], 0.04)
    );
}


// Draw part of a texture (defined by a rectangle) with rotation and scale tiled into dest.
// Code taken from raylib/examples/textures/texture_tiling.c
void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint)
{
    if ((texture.id <= 0) || (scale <= 0.0f)) return;  // Wanna see a infinite loop?!...just delete this line!
    if ((source.width == 0) || (source.height == 0)) return;

    int tileWidth = (int)(source.width*scale), tileHeight = (int)(source.height*scale);
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
