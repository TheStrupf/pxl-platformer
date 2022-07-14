#ifndef GEO_H
#define GEO_H

#include <stdbool.h>

typedef struct rec {
        int x;
        int y;
        int w;
        int h;
} rec;

bool try_rec_intersection(rec a, rec b, rec *r);
rec rec_intersection(rec, rec);
bool rec_overlap(rec, rec);

#endif