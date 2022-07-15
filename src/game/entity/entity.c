#include "entity.h"
#include "list.h"
#include "mem.h"
#include "shared.h"
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
        return e;
}

entity *en_create_solid(int w, int h)
{
        entity *e = en_create();
        if (!e)
                return NULL;
        e->w = w;
        e->h = h;
        e->px = mem_alloc(w * h);
        list_push(world->solids, &e);
        return e;
}

void en_move_actor(entity *e, int dx, int dy)
{
        // potential optimization:
        // only query for leading entity edge (moving direction)
        // instead of a whole block

        int move = ABS(dx);
        int sx = SIGN(dx);
        while (move-- > 0) {
                uchar px = get_pixels(e->x + sx, e->y, e->w, e->h);
                if (!(px & PX_SOLID)) {
                        e->x += sx;
                } else
                        break;
        }

        move = ABS(dy);
        int sy = SIGN(dy);
        while (move-- > 0) {
                uchar px = get_pixels(e->x, e->y + dy, e->w, e->h);
                if (!(px & PX_SOLID)) {
                        e->y += sy;
                } else
                        break;
        }
}

// input x, y, w, h relative to map origin or entity?
uchar en_get_pixels(entity *e, int x, int y, int w, int h)
{
        return 0;
        // return get_pixels_(e->px, e->w, e->h, x, y, w, h);
}