#include "world.h"
#include "geo.h"
#include "input.h"
#include "mem.h"
#include "shared.h"
#include <stdio.h>

#define MAX_ENTITIES 64

// entity with additional padding
// for subclassing
typedef struct entity_generic {
        entity base;
        char padding[128];
} entity_generic;

uint tick;
game_world *world;
entity entities_raw[MAX_ENTITIES];

// individual pixels of a tile
// will be used to set sloped tiles etc.
// clang-format off
uchar tile_collision_masks[64 * 2] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
};
// clang-format on

static uchar get_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, pw);
        const int y2 = MIN(y + h, ph);
        uchar px = 0;
        for (int yi = y1; yi < y2; yi++) {
                const int c = yi * pw;
                for (int xi = x1; xi < x2; xi++)
                        px |= p[xi + c];
        }
        return px;
}

static void set_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, pw);
        const int y2 = MIN(y + h, ph);
        for (int y = y1; y < y2; y++) {
                const int c = y * pw;
                for (int x = x1; x < x2; x++)
                        p[x + c] = pixel;
        }
}

static void add_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, pw);
        const int y2 = MIN(y + h, ph);
        for (int y = y1; y < y2; y++) {
                const int c = y * pw;
                for (int x = x1; x < x2; x++)
                        p[x + c] |= pixel;
        }
}

int world_init()
{
        tick = 0;
        world = mem_alloc(sizeof(game_world));
        memset(world, 0, sizeof(game_world));
        world->entities = list_create_sized(MAX_ENTITIES, sizeof(entity *));
        world->actors = list_create_sized(MAX_ENTITIES, sizeof(entity *));
        world->solids = list_create_sized(MAX_ENTITIES, sizeof(entity *));
        for (int n = 0; n < MAX_ENTITIES; n++) {
                entities_raw[n].active = false;
        }
        world->w = 64;
        world->h = 64;
        world->pw = world->w * 8;
        world->ph = world->h * 8;
        entity *player = en_create_actor();
        player->x = 5;
        player->y = 5;
        player->w = 8;
        player->h = 15;
        set_tile_pixels(10, 10, 1);
}

void world_update()
{
        tick++;
        entity *player = ((entity **)world->actors->data)[0];
        if (btn_pressed(BTN_LEFT))
                en_move_actor(player, -1, 0);
        if (btn_pressed(BTN_RIGHT))
                en_move_actor(player, 1, 0);
        if (btn_pressed(BTN_UP))
                en_move_actor(player, 0, -1);
        if (btn_pressed(BTN_DOWN))
                en_move_actor(player, 0, 1);

        entity **entities = world->entities->data;
        for (int n = 0; n < world->entities->n; n++) {
                entity *e = entities[n];
                if (e->jump_table)
                        e->jump_table[e->action](e);
        }

        for (int n = world->entities->n - 1; n >= 0; n--) {
                entity *e = entities[n];
                if (!e->alive) {
                        e->active = false;
                        list_del_at(world->entities, n);
                }
        }
}

static void world_draw_layer(int layer, int x1, int y1, int x2, int y2)
{
        const int w = world->w;
        tile *tiles = world->rtiles[layer];
        for (int y = y1; y < y2; y++) {
                const int c = y * w;
                for (int x = x1; x < x2; x++) {
                        tile *t = &tiles[x + c];
                }
        }
}

void world_draw()
{
        int pw = world->pw;
        int ph = world->ph;
        uchar *px = world->pixels;
        const int x2 = MIN(pw, gfx_width());
        const int y2 = MIN(ph, gfx_height());

        entity *player = ((entity **)world->actors->data)[0];
        gfx_rec_filled(player->x, player->y, player->w, player->h, 5);

        for (int y = 0; y < y2; y++) {
                for (int x = 0; x < x2; x++) {
                        if (px[x + y * pw] & PX_SOLID)
                                gfx_px(x, y, 5);
                }
        }

        entity **entities = world->entities->data;
        for (int n = 0; n < world->entities->n; n++) {
                entity *e = entities[n];
                // draw entity
        }
}

void set_tile_pixels(int tx, int ty, int collID)
{
        // out of bounds check using uint conversion
        // negative number to unsigned int -> really large
        if ((uint)tx >= world->w || (uint)ty >= world->h)
                return;
        world->tiles[tx + ty * world->w] = collID;
        // pointers to the left most pixel coloumn of the tile/mask
        char *wptr = (char *)(&world->pixels[tx * 8 + ty * 8 * world->pw]);
        char *tptr = (char *)(&tile_collision_masks[collID * 64]);
        for (int i = 0; i < 8; i++) {
                memcpy(wptr, tptr, 8);
                // incr pointer by one row ("y++")
                tptr += 8;
                wptr += world->pw;
        }
}

uchar get_pixels(int x, int y, int w, int h)
{
        uchar px = get_pixels_(world->pixels, world->pw, world->ph,
                               x, y, w, h);
        rec rmap = (rec){x, y, w, h};
        rec r;
        entity **solids = world->solids->data;
        for (int n = 0; n < world->solids->n; n++) {
                entity *e = solids[n];
                if (e->collidable &&
                    try_rec_intersection(rmap, (rec){e->x, e->y, e->w, e->h}, &r))
                        px |= en_get_pixels(e, r.x, r.y, r.w, r.h);
        }
        return px;
}

void set_pixels(int x, int y, int w, int h, uchar px)
{
        set_pixels_(world->pixels, world->pw, world->ph, x, y, w, h, px);
}

void add_pixel_flags(int x, int y, int w, int h, uchar px)
{
        add_pixels_(world->pixels, world->pw, world->ph, x, y, w, h, px);
}

static entity *en_create()
{
        for (int n = 0; n < MAX_ENTITIES; n++) {
                if (!entities_raw[n].active) {
                        entity *e = &entities_raw[n];
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
        if (!e) {
                return NULL;
        }
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
        return get_pixels_(e->px, e->w, e->h, x, y, w, h);
}