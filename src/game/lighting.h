#ifndef LIGHTING_H
#define LIGHTING_H

#include "engine.h"
#include <stdbool.h>

typedef struct light {
        ushort r;
        uchar strength;
        int x;
        int y;
        bool flickering;
        int angle; // TODO: implement light cones
        int opening;
} light;

typedef struct tex tex;

void light_init();
void light_clear(uchar);
void light_update(light l);
void light_apply_to(tex t, int x, int y);

#endif