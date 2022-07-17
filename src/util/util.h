#ifndef UTIL_H
#define UTIL_H

#include "shared.h"
#include <float.h>
#include <stdbool.h>

u32 isqrt32(u32);
u64 isqrt64(u64);

typedef struct rec {
        int x, y;
        int w, h;
} rec;

typedef struct m2 {
        float m11, m12;
        float m21, m22;
} m2;

typedef struct v2 {
        float x;
        float y;
} v2;

typedef struct v2i {
        int x;
        int y;
} v2i;

char *read_txt(const char *);

uchar get_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h);
void set_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel);
void add_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel);

bool try_rec_intersection(rec a, rec b, rec *r);
rec rec_intersection(rec, rec);
bool rec_overlap(rec, rec);

m2 m2_I();
m2 m2_add(m2, m2);
m2 m2_sub(m2, m2);
m2 m2_mul(m2, m2);
m2 m2_scl(m2, float);
m2 m2_inv(m2);
m2 m2_aff_rot(float);
m2 m2_aff_scl(float x, float y);
m2 m2_aff_shr(float x, float y);
float m2_det(m2 a);

#define M2V2_MUL(m, v) ((v2){m.m11 * v.x + m.m12 * v.y, m.m21 * v.x + m.m22 * v.y})
#define V2_ADD(a, b) ((v2){a.x + b.x, a.y + b.y})
#define V2_SUB(a, b) ((v2){a.x - b.x, a.y - b.y})
#define V2_SCL(a, s) ((v2){a.x * s, a.y * s})
#define V2_MIN(a, b) ((v2){MIN(a.x, b.x), MIN(a.y, b.y)})
#define V2_MAX(a, b) ((v2){MAX(a.x, b.x), MAX(a.y, b.y)})
#define V2_DOT(a, b) (a.x * b.x + a.y + b.y)
#endif