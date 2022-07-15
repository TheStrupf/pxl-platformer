#ifndef GFX_H
#define GFX_H

#include "shared.h"
#include "util.h"

// Pico-8 color palette
// Color names adopted from
// pico-8.fandom.com/wiki/Palette

#define FPS 60
#define COLORS 32
#define COL_TRANSPARENT COLORS

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
        ushort iw;   // internal width pow2
        ushort ih;   // internal height pow2
        uchar shift; // "<< shift" = "* w"
        uchar *px;
} tex;

void gfx_init();
void gfx_destroy();
void gfx_begin();
void gfx_end();
void gfx_show();
bool gfx_close_requested();
void gfx_set_translation(int x, int y);

int gfx_width();
int gfx_height();

// return tex as value
// could return as a pointer IF I wanted to change texture dimensions
tex gfx_tex_create(uint w, uint h);
void gfx_tex_destroy(tex t);
void gfx_clear_tex(tex t, uchar col);
void gfx_clear(uchar col);
void gfx_px(int x, int y, uchar col);

// flags: bitmask for transformation
// 001: Flip X
// 010: Flip Y
// 100: Flip Diagonal
// Idea: doc.mapeditor.org/en/stable/reference/global-tile-ids/#tile-flipping
void gfx_sprite(tex s, int x, int y, rec r, char flags);
void gfx_sprite_affine(tex s, v2 p, v2 o, rec r, m2 m);
void gfx_line(int x1, int y1, int x2, int y2, uchar col);
void gfx_rec_filled(int x, int y, uint w, uint h, uchar col);
void gfx_rec_outline(int x, int y, uint w, uint h, uchar col);

#endif