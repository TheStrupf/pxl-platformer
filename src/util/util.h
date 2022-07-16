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
v2 v2_add(v2 a, v2 b);
v2 v2_sub(v2 a, v2 b);
v2 v2_scl(v2 a, float s);
float v2_dot(v2 a, v2 b);
v2 m2_v2_mul(m2 m, v2 v);
v2 v2_min(v2 a, v2 b);
v2 v2_max(v2 a, v2 b);

#endif