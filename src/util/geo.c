#include "geo.h"
#include "shared.h"

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