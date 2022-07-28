#include "world.h"
#include "engine.h"
#include "entity.h"
#include "lighting.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint tick;
game_world *world;

tex playertex;

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

typedef struct tileanimator {
        char num_frames;
        ushort frames[8];
} tileanimator;

tileanimator tileanimators[16];

static int tile_frame(int ID)
{
        for (int n = 0; n < 16; n++) {
                tileanimator *t = &tileanimators[n];
        }
        return ID;
}

void world_init()
{
        tick = 0;
        light_init();
        world = vmalloc(sizeof(game_world));
        memset(world, 0, sizeof(game_world));
        world->entities = list_create_sized(MAX_ENTITIES, sizeof(entity *));
        world->actors = list_create_sized(MAX_ENTITIES, sizeof(entity *));
        world->solids = list_create_sized(MAX_ENTITIES, sizeof(entity *));
        playertex = vmgfx_tex_load("assets/gfx/player.json");
        world->w = 64;
        world->h = 64;
        world->pw = world->w * 8;
        world->ph = world->h * 8;
        world->cam.r.w = vmgfx_width();
        world->cam.r.h = vmgfx_height();
        en_hero();
        entity *dsolid = en_dummysolid();
        dsolid->x = 10;
        dsolid->y = 100;
        for (int n = 0; n < 64; n++)
                set_tile_pixels(n, 20, 0);
}

void world_update()
{
        tick++;
        entity **entities = world->entities->data;
        for (int n = 0; n < world->entities->n; n++) {
                entity *e = entities[n];
                if (e->jump_table) e->jump_table[e->action](e);
                // fixed point
                e->move_q8.x += (e->vel_q12.x >> 4);
                e->move_q8.y += (e->vel_q12.y >> 4);
                e->vel_q12.x = (e->vel_q12.x * e->drag_q8.x) >> 8;
                e->vel_q12.y = (e->vel_q12.y * e->drag_q8.y) >> 8;
                en_move(e, e->move_q8.x >> 8, e->move_q8.y >> 8);
                e->move_q8.x &= 0xFF;
                e->move_q8.y &= 0xFF;
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
        vmgfx_clear(COL_WHITE);
        const int pw = world->pw;
        const int ph = world->ph;
        const int x2 = MIN(pw, vmgfx_width());
        const int y2 = MIN(ph, vmgfx_height());
        uchar *px = world->pixels;

        entity *player;
        list_get(world->actors, 0, &player);
        cam_center(player->x, player->y);

        const int tx1 = MAX(world->cam.r.x / 8 - 1, 0);
        const int ty1 = MAX(world->cam.r.y / 8 - 1, 0);
        const int tx2 = MIN((world->cam.r.x + world->cam.r.w) / 8 + 1, world->w);
        const int ty2 = MIN((world->cam.r.y + world->cam.r.h) / 8 + 1, world->h);
        world_draw_layer(0, tx1, ty1, tx2, ty2);

        vmgfx_sprite(playertex, player->x - 10, player->y - 10, (rec){0, 0, 32, 32}, 0);

        for (int y = 0; y < y2; y++) {
                for (int x = 0; x < x2; x++) {
                        if (px[x + y * pw] & PX_SOLID)
                                vmgfx_px(x, y, 5);
                }
        }

        entity **entities = world->entities->data;
        for (int n = 0; n < world->entities->n; n++) {
                entity *e = entities[n];
                // draw entity
                vmgfx_rec_filled(e->x, e->y, e->w, e->h, COL_GREEN);
        }
        world->baselight = 0xFF;
        light_clear(world->baselight);
        if (world->baselight < 0xFF) {
                light l;
                l.x = 10;
                l.y = 100;
                l.r = 150;
                l.flickering = true;
                l.strength = 255;
                light_update(l);

                light l2;
                l2.x = 200;
                l2.y = 50;
                l2.r = 100;
                l2.flickering = true;
                l2.strength = 255;
                light_update(l2);
                light_apply_to(vmgfx_screen_tex(), 0, 0);
        }
}

void cam_center(int x, int y)
{
        cam *c = &world->cam;
        if (!c->lock_x) {
                c->r.x = x - c->r.w / 2;
                if (c->r.x < 0) c->r.x = 0;
                if (c->r.x + c->r.w >= world->pw) c->r.x = world->pw - c->r.w;
        }

        if (!c->lock_y) {
                c->r.y = y - c->r.h / 2;
                if (c->r.y < 0) c->r.y = 0;
                if (c->r.y + c->r.h >= world->ph) c->r.y = world->ph - c->r.h;
        }
}

void set_tile_pixels(int tx, int ty, int collID)
{
        if ((uint)tx >= world->w || (uint)ty >= world->h)
                return;
        world->tiles[tx + ty * world->w] = collID;

        // pointers to the left most pixel coloumn of the tile/mask
        char *wptr = (char *)(&world->pixels[tx * 8 + ty * 8 * world->pw]);
        char *tptr = (char *)(&tile_collision_masks[collID * 64]);
        for (int i = 0; i < 8; i++) {
                memcpy(wptr, tptr, 8);
                tptr += 8;
                wptr += world->pw;
        }
}

uchar get_pixels(int x, int y, int w, int h)
{
        uchar p = get_pixels_(world->pixels, world->pw, world->ph,
                              x, y, w, h);
        entity **solids = world->solids->data;
        for (int n = 0; n < world->solids->n; n++) {
                entity *e = solids[n];
                p |= solid_get_pixels(e, x - e->x, y - e->y, w, h);
        }
        return p;
}

void set_pixels(int x, int y, int w, int h, uchar px)
{
        set_pixels_(world->pixels, world->pw, world->ph,
                    x, y, w, h, px);
}

void add_pixel_flags(int x, int y, int w, int h, uchar px)
{
        add_pixels_(world->pixels, world->pw, world->ph,
                    x, y, w, h, px);
}

uchar get_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h)
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

void set_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, pw);
        const int y2 = MIN(y + h, ph);
        for (int y = y1; y < y2; y++)
                memset(&p[x1 + y * pw], pixel, x2 - x1);
}

void add_pixels_(uchar *p, int pw, int ph, int x, int y, int w, int h, uchar pixel)
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