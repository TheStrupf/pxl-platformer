#ifndef ENTITY_H
#define ENTITY_H

#include "shared.h"

#define MAX_ENTITIES 64

enum en_type {
        EN_ACTOR,
        EN_SOLID,
};

// macro for subclassing
#define super ((entity *)this)

typedef struct entity {
        uint type;
        bool active;
        bool alive;
        int x;
        int y;
        int w;
        int h;
        ushort timer;
        ushort subtimer;
        uchar action;
        func *jump_table;

        bool collidable;
        struct entity *linked;
        uchar *px;
} entity;

entity *en_create_actor();
entity *en_create_solid(int w, int h);
void en_move_actor(entity *e, int dx, int dy);

// input x, y, w, h relative to map origin or entity?
uchar en_get_pixels(entity *e, int x, int y, int w, int h);

entity *en_hero();
entity *en_template();

#endif