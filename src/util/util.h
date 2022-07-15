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

bool try_rec_intersection(rec a, rec b, rec *r);
rec rec_intersection(rec, rec);
bool rec_overlap(rec, rec);

typedef struct m2 {
        float m11, m12;
        float m21, m22;
} m2;

m2 m2_I();
m2 m2_add(m2, m2);
m2 m2_sub(m2, m2);
m2 m2_mul(m2, m2);
m2 m2_scl(m2, float);
m2 m2_inv(m2);

m2 m2_aff_rot(float);
m2 m2_aff_scl(float x, float y);
m2 m2_aff_shr(float x, float y);
float m2_det(m2);

typedef struct v2 {
        float x;
        float y;
} v2;

typedef struct v2i {
        int x;
        int y;
} v2i;

v2 v2_add(v2, v2);
v2 v2_sub(v2, v2);
v2 v2_scl(v2, float);
float v2_dot(v2, v2);
v2 m2_v2_mul(m2, v2);
v2 v2_min(v2, v2);
v2 v2_max(v2, v2);

#endif