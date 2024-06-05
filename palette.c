#include "raylib.h"
#include "palette.h"

/* cliteral? sounds frisky */

const Color palette[] = {
    /* COLOR_BACKGROUND */
    CLITERAL(Color){0xFF, 0xFF, 0xBA, 0xFF},
    /* COLOR_AVATAR */
    CLITERAL(Color){0xBA, 0xE1, 0xFF, 0xFF},
    /* COLOR_AVATAR_BORDER1 */
    CLITERAL(Color){0xFF, 0xFF, 0xFF, 0xFF},
    /* COLOR_AVATAR_BORDER2 */
    CLITERAL(Color){0x92, 0x92, 0x92, 0xFF},
    /* COLOR_TUBE_DEFAULT */
    CLITERAL(Color){0xff, 0xb3, 0xba, 0xFF},
    /* COLOR_TUBE_SHADOW */
    CLITERAL(Color){0x00, 0x00, 0x00, 0x20},
};

Color get_color(int clr) {
    return palette[clr];
}