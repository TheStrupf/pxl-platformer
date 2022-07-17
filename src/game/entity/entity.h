#ifndef ENTITY_H
#define ENTITY_H

#include "shared.h"
#include "util.h"

#define MAX_ENTITIES 64

// used to flag entities for properties
enum en_flags {
        EN_ACTOR = 1,
        EN_SOLID = 2,
};

// used to identify concrete entities
// skeleton, chest, coin, ...
enum en_type {
        DUMMY,
};

// used to identify UNIQUE entities
enum en_tag {
        ENTAG_HERO,
};

// macro for subclassing
#define super ((entity *)this)

typedef struct entity {
        uint flags;
        uchar type;
        uchar tag;
        bool active;
        bool alive;
        rec r;
        v2i move_q8;
        v2i vel_q12;
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
void en_move(entity *e, int dx, int dy);

// input x, y, w, h relative to map origin or entity?
uchar solid_get_pixels(entity *e, int x, int y, int w, int h);

entity *en_hero();
entity *en_template();

entity *en_tagged(uchar);

#endif