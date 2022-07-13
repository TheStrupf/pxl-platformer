#include "gfx.h"

tex *gfx_tex_create(uint w, uint h)
{
        const size_t TSIZE = sizeof(tex);
        tex *t = malloc(TSIZE + w * h);
        if (!t)
                return NULL;
        t->pixels = (char *)t + TSIZE;
        t->w = w;
        t->h = h;
        return t;
}

void gfx_tex_destroy(tex *t)
{
        if (t)
                free(t);
}

void gfx_clear(tex *t, uchar col)
{
        memset(t->pixels, col, sizeof(uchar) * t->w * t->h);
}

void gfx_sprite(tex *t, tex *s, int x, int y)
{
}