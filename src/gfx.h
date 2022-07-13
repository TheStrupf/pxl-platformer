#ifndef GFX_H
#define GFX_H

#include "shared.h"

typedef struct tex {
        ushort w;
        ushort h;
        uchar *pixels;
} tex;

tex *gfx_tex_create(uint w, uint h);
void gfx_tex_destroy(tex *t);
void gfx_clear(tex *t, uchar col);

void gfx_sprite(tex *t, tex *s, int x, int y);

#endif