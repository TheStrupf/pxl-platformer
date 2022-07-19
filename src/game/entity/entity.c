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

void en_reset_raw()
{
        for (int n = 0; n < MAX_ENTITIES; n++) {
                entity *e = &entities_raw[n].base;
                e->active = false;
                e->alive = false;
        }
}

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
                        e->drag_q8.x = 256;
                        e->drag_q8.y = 256;
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
        e->w = w;
        e->h = h;
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
                prev = e->x;
                tmp = sn == 1 ? e->w : -1;
                uchar px = get_pixels(e->x + tmp, e->y, 1, e->h);

                if (!(px & PX_SOLID)) {
                        e->x += sn;

                        // avoid downhill hopping by glueing the player to the ground
                        if (e->gluetoground) {
                        }
                } else {
                        if (e->climbslopes) {
                                px = get_pixels(e->x + tmp, e->y - 1, 1, e->h) | get_pixels(e->x + tmp, e->y - 1, e->w, 1);
                                if (!(px & PX_SOLID)) {
                                        e->x += sn;
                                        e->y--;
                                }
                        }
                }

                if (e->x == prev) {
                        // didnt move, bumped
                        e->vel_q12.x = 0;
                        break;
                }
        }

        move = ABS(dy);
        sn = SIGN(dy);
        int mask = PX_SOLID;
        if (e->landonplatforms && sn == 1)
                mask |= PX_PLATFORM;
        while (move-- > 0) {
                prev = e->y;
                tmp = sn == 1 ? e->h : -1;
                uchar px = get_pixels(e->x, e->y + tmp, e->w, 1);

                if (!(px & mask)) {
                        e->y += sn;
                }

                if (e->y == prev) {
                        // didnt move, bumped
                        e->vel_q12.y = 0;
                        break;
                }
        }
}

static void en_move_solid_axis(entity *solid, int *coord, int dx, int dy)
{
        list *riders = list_create_sized(8, sizeof(entity *));
        entity **actors = (entity **)world->actors->data;
        int move = ABS(dx + dy);
        int sx = SIGN(dx);
        int sy = SIGN(dy);
        while (move-- > 0) {
                riders->n = 0;
                for (int n = 0; n < world->actors->n; n++) {
                        entity *e = actors[n];

                        // check if entity stands on solid
                        rec standingrow = (rec){e->x, e->y + e->h, e->w, 1};
                        rec ir;
                        if (try_rec_intersection(standingrow, EN_REC(solid), &ir)) {
                                const int c = (ir.y - solid->y) * solid->w;
                                const int x1 = ir.x - solid->x;
                                const int x2 = x1 + ir.w;
                                int mask = PX_SOLID;
                                if (e->landonplatforms)
                                        mask |= PX_PLATFORM;
                                for (int xi = x1; xi < x2; xi++) {
                                        if ((solid->px[xi + c] & mask) != 0) {
                                                list_push(riders, &e);
                                                break;
                                        }
                                }
                        }
                }

                *coord += sx + sy;
                for (int n = 0; n < world->actors->n; n++) {
                        entity *e = actors[n];
                        if (EN_OVERLAP(solid, e) || list_find(riders, &e) >= 0)
                                en_move_actor(e, sx, sy);
                }
        }
}

void en_move(entity *e, int dx, int dy)
{
        if (e->flags & EN_ACTOR)
                en_move_actor(e, dx, dy);
        else if (e->flags & EN_SOLID) {
                e->collidable = false;
                en_move_solid_axis(e, &e->x, dx, 0);
                en_move_solid_axis(e, &e->y, 0, dy);
                e->collidable = true;
        }
}

bool en_grounded(entity *e)
{
        int mask = PX_SOLID;
        if (e->vel_q12.y >= 0 && e->landonplatforms)
                mask |= PX_PLATFORM;
        uchar pixels = get_pixels(e->x, e->y + e->h, e->w, 1);
        return (pixels & mask);
}

// input x, y, w, h relative to map origin or entity?
uchar solid_get_pixels(entity *e, int x, int y, int w, int h)
{
        if (!e->collidable)
                return 0;
        return get_pixels_(e->px, e->w, e->h, x, y, w, h);
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