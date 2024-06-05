#include "raylib.h"
#include "api.h"

#include "intro.txt.h"
#include "prophecy.txt.h"
#include "flapflap.txt.h"

#define NWINDOWS 3

static const unsigned char *textwindows[] = {
    intro_txt,
    prophecy_txt,
    flapflap_txt,
};

void intro_setup(Intro *intro)
{
    Font fnt = LoadFont("Ac437_IBM_BIOS.ttf");
    intro->ibmfont = fnt;
    intro->running = 0;
    intro->pos = 0;
}

void intro_update(Intro *intro)
{
    if (IsKeyPressed(KEY_SPACE)) {
        intro->pos++;

        if (intro->pos >= NWINDOWS) {
            intro->running = 0;
        }
    }
}

void draw_intro_screen(Intro *intro)
{
    Font fnt = intro->ibmfont;
    Color bgcolor = BLACK;
    Color fgcolor = (Color){0x4e, 0xDc, 0x4e, 0xff};
    ClearBackground(bgcolor);
    DrawTextEx(fnt,
        (const char *)textwindows[intro->pos],
        (Vector2){20, 20},
        fnt.baseSize*0.5,
        1.0,
        fgcolor
            );
}
