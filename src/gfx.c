#include "gfx.h"

int offsetx = 0;
int offsety = 0;

void gfx_set_translation(int x, int y)
{
        offsetx = x;
        offsety = y;
}

tex gfx_tex_create(uint w, uint h)
{
        tex t;
        t.pixels = malloc(w * h);
        if (!t.pixels)
                t.w = t.h = 0;
        else {
                t.w = w;
                t.h = h;
        }
        return t;
}

void gfx_tex_destroy(tex t)
{
        if (t.pixels)
                free(t.pixels);
}

void gfx_clear(tex t, uchar col)
{
        memset(t.pixels, col, sizeof(uchar) * t.w * t.h);
}

void gfx_sprite(tex t, tex s, int x, int y, rec r, char flags)
{
        int srcx, srcy;
        int xa = 0;
        int ya = 0;
        switch (flags) {
        case B8(00000000):
                srcx = 1;
                srcy = s.w;
                break;
        case B8(00000001):
                srcx = -1;
                srcy = s.w;
                xa = r.w - 1;
                break;
        case B8(00000010):
                srcx = 1;
                srcy = -s.w;
                ya = r.h - 1;
                break;
        case B8(00000011):
                srcx = -1;
                srcy = -s.w;
                xa = r.w - 1;
                ya = r.h - 1;
                break;
        case B8(00000100):
                srcx = s.w;
                srcy = 1;
                break;
        case B8(00000101):
                srcx = -s.w;
                srcy = 1;
                ya = r.h - 1;
                break;
        case B8(00000110):
                srcx = s.w;
                srcy = -1;
                xa = r.w - 1;
                break;
        case B8(00000111):
                srcx = -s.w;
                srcy = -1;
                xa = r.w - 1;
                ya = r.h - 1;
                break;
        default:
                return;
        }
        x += offsetx;
        y += offsety;
        int x1 = MAX(0, -x);
        int y1 = MAX(0, -y);
        int x2 = MIN((flags & SPR_FLAG_D) > 0 ? r.h : r.w, t.w - x);
        int y2 = MIN((flags & SPR_FLAG_D) > 0 ? r.w : r.h, t.h - y);

        for (int cy = y1; cy < y2; cy++) {
                int cc = (cy * t.w) + x + (y * t.w);
                int cz = cy * srcy + r.x + xa + ((r.y + ya) * s.w);
                for (int cx = x1; cx < x2; cx++) {
                        uchar col = s.pixels[srcx * cx + cz];
                        if (col != COL_TRANSPARENT)
                                t.pixels[cx + cc] = col;
                }
        }
}