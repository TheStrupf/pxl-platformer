#ifndef GFX_H
#define GFX_H

#include "shared.h"

// Pico-8 color palette
// Color names adopted from
// pico-8.fandom.com/wiki/Palette

#define COLORS 32
#define COL_TRANSPARENT 255

enum color_IDs {
        COL_BLACK,
        COL_DARK_BLUE,
        COL_DARK_PURPLE,
        COL_DARK_GREEN,
        COL_BROWN,
        COL_DARK_GREY,
        COL_LIGHT_GREY,
        COL_WHITE,
        COL_RED,
        COL_ORANGE,
        COL_YELLOW,
        COL_GREEN,
        COL_BLUE,
        COL_LAVENDER,
        COL_PINK,
        COL_LIGHT_PEACH,
        COL_BROWNISH_BLACK,
        COL_DARKER_BLUE,
        COL_DARKER_PURPLE,
        COL_BLUE_GREEN,
        COL_DARK_BROWN,
        COL_DARKER_GREY,
        COL_MEDIUM_GREY,
        COL_LIGHT_YELLOW,
        COL_DARK_RED,
        COL_DARK_ORANGE,
        COL_LIME_GREEN,
        COL_MEDIUM_GREEN,
        COL_TRUE_BLUE,
        COL_MAUVE,
        COL_DARK_PEACH,
        COL_PEACH
};

enum sprite_flags {
        SPR_FLAG_X = 1,
        SPR_FLAG_Y = 2,
        SPR_FLAG_D = 4
};

typedef struct tex {
        ushort w;
        ushort h;
        uchar *pixels;
} tex;

typedef struct rec {
        int x;
        int y;
        int w;
        int h;
} rec;

void gfx_set_translation(int x, int y);
tex gfx_tex_create(uint w, uint h);
void gfx_tex_destroy(tex t);
void gfx_clear(tex t, uchar col);

// flags: bitmask for transformation
// 001: Flip X
// 010: Flip Y
// 100: Flip Diagonal
// Idea: doc.mapeditor.org/en/stable/reference/global-tile-ids/#tile-flipping
void gfx_sprite(tex t, tex s, int x, int y, rec r, char flags);

#endif