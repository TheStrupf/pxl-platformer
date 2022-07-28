#include "lighting.h"
#include "engine.h"
#include "entity.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSIZE 1024

static const int MAXR = LSIZE / 2 - 4;

// replacement color of a pixel at a specified light level (0... 255)
// X: Light level, 0 (dark) to 255 (bright)
// Y: Color index
// clang-format off
static const uchar colormap[8192] = {
        #include "lighting_px.csv"
};
// clang-format on

static uchar *lmap; // temporary buffer max size
static uchar *lightmap;
static uint lightrng = 213;

void light_init()
{
        lightmap = vmalloc(NUM_TILES * 64);
        lmap = vmalloc(LSIZE * LSIZE * 2);
}

void light_clear(uchar baselight)
{
        memset(lightmap, baselight, NUM_TILES * 64);
}

void light_update(light l)
{
        if (l.r > MAXR) l.r = MAXR;
        uint PW = world->pw;
        uint PH = world->ph;
        uint R = l.r + 1;
        uint SIZE = R * 2 + 1;

        // intersection light area and world
        rec lightrec = (rec){l.x - R, l.y - R, SIZE, SIZE};
        rec aabb;
        if (!try_rec_intersection(lightrec, (rec){0, 0, PW, PH}, &aabb))
                return;

        // copy the occupied pixel area of the light into a local buffer
        uchar *pxmap = world->pixels;
        uchar *pxcache = &lmap[SIZE * SIZE]; // local pixel buffer just behind the local light buffer
        memset(lmap, 0, SIZE * SIZE * 2);    // clear both lightmap and collision map
        int Y2 = aabb.y + aabb.h;
        int X2 = aabb.x + aabb.w;
        int C0 = -lightrec.x - lightrec.y * SIZE;

        // could use memcpy instead - but maybe I want to have a coarser lightmap than currently e.g. 2x2 instead of 1x1
        for (int y = aabb.y; y < Y2; y++) {
                int C1 = y * SIZE + C0;
                int C2 = y * PW;
                for (int x = aabb.x; x < X2; x++)
                        pxcache[x + C1] = pxmap[x + C2];
        }

        entity **solids = (entity **)world->solids->data;
        for (int n = 0; n < world->solids->n; n++) {
                entity *e = solids[n];
                rec temp;
                if (!try_rec_intersection(lightrec, EN_REC(e), &temp))
                        continue;
                int IX2 = temp.x + temp.w;
                int IY2 = temp.y + temp.h;
                int EW = e->w;
                int C3 = -e->x - e->y * EW;
                uchar *epx = e->px;
                for (int y = temp.y; y < IY2; y++) {
                        int C1 = y * SIZE + C0;
                        int C2 = y * EW + C3;
                        for (int x = temp.x; x < IX2; x++)
                                pxcache[x + C1] |= epx[x + C2];
                }
        }

        // some caching
        int START = R + R * SIZE;
        uint STRENGTH = l.strength;
        uint CMP = STRENGTH << 16;
        uint SCALE_Q16 = CMP / (l.r * l.r);
        uint INCR_SQ = SCALE_Q16 << 1;

        // four cases - not quadrants
        //  \ 2 /
        // 0 \ / 1
        //   / \'
        //  / 3 \'

        for (int q = 0; q < 4; q++) {
                int mx, my, x2, y2;
                // -R is cached from const int DTX = ...
                switch (q) {
                case 0: // traverse left edge, iterate y2
                        x2 = -R;
                        y2 = -R;
                        mx = 0xFFFF;
                        my = 0;
                        break;
                case 1: // traverse right edge, iterate y2
                        x2 = SIZE - 1 - R;
                        y2 = -R;
                        mx = 0xFFFF;
                        my = 0;
                        break;
                case 2: // traverse top edge, iterate x2
                        x2 = -R;
                        y2 = -R;
                        mx = 0;
                        my = 0xFFFF;
                        break;
                case 3: // traverse bottom edge, iterate x2
                        x2 = -R;
                        y2 = SIZE - 1 - R;
                        mx = 0;
                        my = 0xFFFF;
                        break;
                }

                // cast rays
                for (int n = 0; n < SIZE; n++) {
                        int DTX = x2 + (n & my); // bitmasking for iteration of only x or y
                        int DTY = y2 + (n & mx);
                        int DX, DY, XSTEP, YSTEP;
                        if (DTX > 0) {
                                DX = DTX;
                                XSTEP = 1;
                        } else {
                                DX = -DTX;
                                XSTEP = -1;
                        }
                        if (DTY > 0) {
                                DY = -DTY;
                                YSTEP = SIZE;
                        } else {
                                DY = DTY;
                                YSTEP = -SIZE;
                        }

                        // scale light proportional to 1 / (r^2)
                        // use a math trick to avoid having to calc r^2 = x*x + y*y every frame:
                        // because we start at x = 0, y= 0 (center, distance 0) we can increment
                        // the r^2 on every step:
                        //
                        // x | area | area increased by
                        // -----------------------------
                        // 0 |  0   | 0
                        // 1 |  1   | 1
                        // 2 |  4   | 3
                        // 3 |  9   | 5
                        // 4 | 16   | 7
                        // 5 | 25   | 9
                        //
                        // "area increased by" increases by 2 on every step in one direction.
                        // also using fixed point because it's faster

                        uint dsq_q16 = 0;    // distance in 16.16 fixed point from the origin
                        uint ix = SCALE_Q16; // SCALE_Q16 = "1"
                        uint iy = SCALE_Q16;
                        int index = START;
                        int err = DX + DY;

                        // while light is not 0 - bresenham
                        while (dsq_q16 < CMP) {
                                lmap[index] = (uchar)(STRENGTH - (dsq_q16 >> 16));
                                const int E2 = err * 2;
                                if (E2 >= DY) {
                                        err += DY;
                                        index += XSTEP;
                                        dsq_q16 += ix; // incr by 1, 3, 5, 7, ...
                                        ix += INCR_SQ; // INCR_SQ = "2"
                                }
                                if (E2 <= DX) {
                                        err += DX;
                                        index += YSTEP;
                                        dsq_q16 += iy;
                                        iy += INCR_SQ;
                                }

                                // reduce light level if we hit a solid pixel
                                if (pxcache[index] & PX_SOLID) dsq_q16 <<= 1;
                        }
                }
        }

        // merge local lightmap to world lightmap
        // mask for flickering
        uint mask = l.flickering ? B_00000011 : 0;
        for (int y = aabb.y; y < Y2; y++) {
                lightrng = lightrng * 32310901 + 1; // fast LCG random for light flickering
                uint rng = (lightrng >> 16) & mask;
                int C1 = y * PW;
                int C2 = y * SIZE + C0;
                for (int x = aabb.x; x < X2; x++) {
                        uint b = lmap[x + C2];
                        if (b <= rng) continue;
                        b -= rng;
                        uint ind = x + C1;
                        uint a = lightmap[ind];
                        uint r = a * a + b * b;
                        if (r >= 64516) { // sqrt(64516) = 254... put 255 instead
                                lightmap[ind] = 0xFF;
                                continue;
                        }

                        // clang-format off
                        uint q, t;
                        if (r >= 0x4000) {
                                r -= 0x4000; q = 0x2000; t = 0x5000;
                        } else {
                                q = 0; t = 0x1000; 
                        }

                        if (r >= t) { r -= t; q += 0x1000; }
                        t = q + 0x0400; q >>= 1;
                        if (r >= t) { r -= t; q += 0x0400; }
                        t = q + 0x0100; q >>= 1;
                        if (r >= t) { r -= t; q += 0x0100; }
                        t = q + 0x0040; q >>= 1;
                        if (r >= t) { r -= t; q += 0x0040; }
                        t = q + 0x0010; q >>= 1;
                        if (r >= t) { r -= t; q += 0x0010; }
                        t = q + 0x0004; q >>= 1;
                        if (r >= t) { r -= t; q += 0x0004; }
                        t = q + 0x0001; q >>= 1;
                        if (r >= t) q += 0x0001;
                        lightmap[ind] = q; // clang-format on
                }
        }
}

// replace texture pixels by corresponding pixels for the present light level
void light_apply_to(tex t, int x, int y)
{
        rec tr = (rec){x, y, t.w, t.h};
        rec rr = (rec){0, 0, world->pw, world->ph};
        rec r;
        if (!try_rec_intersection(tr, rr, &r)) return;
        int PW = world->pw;
        int x1 = r.x - x;
        int y1 = r.y - y;
        int x2 = x1 + r.w;
        int y2 = y1 + r.h;
        int C = x + y * PW;
        for (int yi = y1; yi < y2; yi++) {
                int C2 = yi * PW + C;
                int C3 = yi << t.shift;
                for (int xi = x1; xi < x2; xi++) {
                        int i = xi + C3;
                        uint col = t.px[i];
                        if (col < COLORS)
                                t.px[i] = colormap[(col << 8) + lightmap[xi + C2]];
                }
        }
}