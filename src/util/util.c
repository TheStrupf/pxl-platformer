#include "util.h"
#include "mem.h"
#include "shared.h"
#include <math.h>
#include <stdio.h>

char *read_txt(const char *filename)
{
        FILE *fptr = fopen(filename, "r");
        if (!fptr) {
                printf("Error opening txt\n");
                return NULL;
        }
        fseek(fptr, 0, SEEK_END);
        long length = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        char *txt = mem_alloc(length + 1);
        if (!txt) {
                printf("Error allocating txt buffer\n");
                fclose(fptr);
                return NULL;
        }
        fread(txt, 1, length, fptr);
        txt[length] = '\0';
        fclose(fptr);
        printf("read txt\n");
        printf("%s\n", txt);
        return txt;
}

uchar get_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, pw);
        const int y2 = MIN(y + h, ph);
        uchar px = 0;
        for (int yi = y1; yi < y2; yi++) {
                const int c = yi * pw;
                for (int xi = x1; xi < x2; xi++)
                        px |= p[xi + c];
        }
        return px;
}

void set_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, pw);
        const int y2 = MIN(y + h, ph);
        for (int y = y1; y < y2; y++)
                memset(&p[x1 + y * pw], pixel, x2 - x1);
}

void add_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, pw);
        const int y2 = MIN(y + h, ph);
        for (int y = y1; y < y2; y++) {
                const int c = y * pw;
                for (int x = x1; x < x2; x++)
                        p[x + c] |= pixel;
        }
}

u32 isqrt32(u32 x)
{
        u32 r = x, q = 0, b = ((u32)1) << 30;
        while (b > r)
                b >>= 2;
        while (b > 0) {
                u32 t = q + b;
                q >>= 1;
                if (r >= t) {
                        r -= t;
                        q += b;
                }
                b >>= 2;
        }
        return q;
}

u64 isqrt64(u64 x)
{
        u64 r = x, q = 0, b = ((u64)1) << 62;
        while (b > r)
                b >>= 2;
        while (b > 0) {
                u64 t = q + b;
                q >>= 1;
                if (r >= t) {
                        r -= t;
                        q += b;
                }
                b >>= 2;
        }
        return q;
}

bool try_rec_intersection(rec a, rec b, rec *r)
{
        rec c = rec_intersection(a, b);
        if (c.w > 0 && c.h > 0) {
                *r = c;
                return true;
        }
        return false;
}

rec rec_intersection(rec a, rec b)
{
        rec c;
        c.x = MAX(a.x, b.x);
        c.y = MAX(a.y, b.y);
        c.w = MIN(a.x + a.w, b.x + b.w) - c.x;
        c.h = MIN(a.y + a.h, b.y + b.h) - c.y;
        return c;
}

bool rec_overlap(rec a, rec b)
{
        return (
            a.x < b.x + b.w &&
            a.y < b.y + b.h &&
            b.x < a.x + a.w &&
            b.y < a.y + a.h);
}

m2 m2_I()
{
        m2 r;
        r.m11 = r.m22 = 1;
        r.m12 = r.m21 = 0;
        return r;
}

m2 m2_add(m2 a, m2 b)
{
        m2 r;
        r.m11 = a.m11 + b.m11;
        r.m12 = a.m12 + b.m12;
        r.m21 = a.m21 + b.m21;
        r.m22 = a.m22 + b.m22;
        return r;
}

m2 m2_sub(m2 a, m2 b)
{
        m2 r;
        r.m11 = a.m11 - b.m11;
        r.m12 = a.m12 - b.m12;
        r.m21 = a.m21 - b.m21;
        r.m22 = a.m22 - b.m22;
        return r;
}

m2 m2_mul(m2 a, m2 b)
{
        m2 r;
        r.m11 = a.m11 * b.m11 + a.m12 * b.m21;
        r.m12 = a.m11 * b.m12 + a.m12 * b.m22;
        r.m21 = a.m21 * b.m11 + a.m22 * b.m21;
        r.m22 = a.m21 * b.m12 + a.m22 * b.m22;
        return r;
}

m2 m2_scl(m2 a, float s)
{
        m2 r;
        r.m11 = a.m11 * s;
        r.m12 = a.m12 * s;
        r.m21 = a.m21 * s;
        r.m22 = a.m22 * s;
        return r;
}

m2 m2_inv(m2 a)
{
        m2 r;
        r.m11 = a.m22;
        r.m12 = -a.m12;
        r.m21 = -a.m21;
        r.m22 = a.m11;
        return m2_scl(r, 1.0 / m2_det(r));
}

m2 m2_aff_rot(float a)
{
        m2 r;
        float s = sinf(a);
        float c = cosf(a);
        r.m11 = c;
        r.m12 = -s;
        r.m21 = s;
        r.m22 = c;
        return r;
}

m2 m2_aff_scl(float x, float y)
{
        m2 r;
        r.m11 = x;
        r.m22 = y;
        r.m12 = r.m21 = 0;
        return r;
}

m2 m2_aff_shr(float x, float y)
{
        m2 r;
        r.m11 = r.m22 = 1;
        r.m21 = y;
        r.m12 = x;
        return r;
}

float m2_det(m2 a)
{
        return a.m11 * a.m22 - a.m21 * a.m12;
}