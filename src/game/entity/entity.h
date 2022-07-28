#ifndef ENTITY_H
#define ENTITY_H

#include "engine.h"
#include "lighting.h"

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
        int x;
        int y;
        int w;
        int h;
        v2i move_q8;
        v2i vel_q12;
        v2i drag_q8;
        ushort timer;
        ushort subtimer;
        uchar action;
        func *jump_table;

        bool climbslopes;
        bool landonplatforms;
        bool gluetoground;
        bool collidable;
        struct entity *linked;
        struct entity *ghostactor; // used when solids should move like actors...
        uchar *px;
        light lig;
} entity;

#define EN_REC(e) ((rec){e->x, e->y, e->w, e->h})
#define EN_OVERLAP(a, b) rec_overlap(EN_REC(a), EN_REC(b))

int en_y_mask(entity *e);

entity *en_create_actor();
entity *en_create_solid(int w, int h);
void en_move(entity *, int dx, int dy);

// input x, y, w, h relative to map origin or entity?
bool en_grounded(entity *);
uchar solid_get_pixels(entity *, int x, int y, int w, int h);

entity *en_hero();
entity *en_dummysolid();
entity *en_template();

void en_reset_raw();
entity *en_tagged(uchar);

#endif