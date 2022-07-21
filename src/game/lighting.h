#ifndef LIGHTING_H
#define LIGHTING_H

#include "shared.h"

typedef struct light {
        uint r;
        uchar strength;
        int x;
        int y;
} light;

typedef struct tex tex;

void light_init();
void light_clear(uchar);
void light_update(light l);
void light_apply_to(tex t, int x, int y);

#endif