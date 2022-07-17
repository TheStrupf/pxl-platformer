#include "entity.h"
#include "list.h"
#include "mem.h"
#include "shared.h"
#include "util.h"
#include "world.h"

// entity with additional padding
// for subclassing
typedef struct entity_generic {
        entity base;
        char padding[128];
} entity_generic;

entity_generic entities_raw[MAX_ENTITIES] = {0};

static entity *en_create()
{
        for (int n = 0; n < MAX_ENTITIES; n++) {
                if (!entities_raw[n].base.active) {
                        entity *e = &entities_raw[n].base;
                        e->active = true;
                        e->alive = true;
                        e->jump_table = NULL;
                        e->px = NULL;
                        e->linked = NULL;
                        e->action = 0;
                        list_push(world->entities, &e);
                        return e;
                }
        }
        return NULL;
}

entity *en_create_actor()
{
        entity *e = en_create();
        if (!e)
                return NULL;
        list_push(world->actors, &e);
        e->flags |= EN_ACTOR;
        return e;
}

entity *en_create_solid(int w, int h)
{
        entity *e = en_create();
        if (!e)
                return NULL;
        e->r.w = w;
        e->r.h = h;
        e->px = mem_alloc(w * h);
        list_push(world->solids, &e);
        e->flags |= EN_SOLID;
        return e;
}

static void en_move_actor(entity *e, int dx, int dy)
{
        int move, sn, prev, tmp;
        move = ABS(dx);
        sn = SIGN(dx);
        while (move-- > 0) {
                prev = e->r.x;
                tmp = sn == 1 ? e->r.w : -1;
                uchar px = get_pixels(e->r.x + tmp, e->r.y, 1, e->r.h);

                if (!(px & PX_SOLID)) {
                        e->r.x += sn;
                }

                if (e->r.x == prev) {
                        // didnt move, bumped
                        break;
                }
        }

        move = ABS(dy);
        sn = SIGN(dy);
        while (move-- > 0) {
                prev = e->r.y;
                tmp = sn == 1 ? e->r.h : -1;
                uchar px = get_pixels(e->r.x, e->r.y + tmp, e->r.w, 1);

                if (!(px & PX_SOLID)) {
                        e->r.y += sn;
                }

                if (e->r.y == prev) {
                        // didnt move, bumped
                        break;
                }
        }
}

static void en_move_solid(entity *e, int dx, int dy)
{
        list *riders = list_create_sized(8, sizeof(entity *));
        list *actors = world->actors;
        int move, sn;
        move = ABS(dx);
        sn = SIGN(dx);
        while (move-- > 0) {
                riders->n = 0;
                entity **act = (entity **)actors->data;
                for (int n = 0; n < actors->n; n++) {
                        entity *a = act[n];
                        // if a on solid then add a to riders
                }
        }

        move = ABS(dy);
        sn = SIGN(dy);
        while (move-- > 0) {
                riders->n = 0;
                entity **act = (entity **)actors->data;
                for (int n = 0; n < actors->n; n++) {
                        entity *a = act[n];
                        // if a on solid then add a to riders
                }
        }
}

void en_move(entity *e, int dx, int dy)
{
        if (e->flags & EN_ACTOR)
                en_move_actor(e, dx, dy);
        else if (e->flags & EN_SOLID)
                en_move_solid(e, dx, dy);
}

// input x, y, w, h relative to map origin or entity?
uchar solid_get_pixels(entity *e, int x, int y, int w, int h)
{
        if (!e->collidable)
                return 0;
        return get_pixels_(e->px, e->r.w, e->r.h, x, y, w, h);
}

entity *en_tagged(uchar tag)
{
        entity **entities = world->entities->data;
        for (int n = 0; n < world->entities->n; n++) {
                entity *e = entities[n];
                if (e->tag == tag)
                        return e;
        }
        return NULL;
}