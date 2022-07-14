#include "world.h"
#include "shared.h"

#define MAX_ENTITIES 256

uint tick;
game_world *world;
entity entities_raw[MAX_ENTITIES];

int world_init()
{
        tick = 0;
        world = malloc(sizeof(game_world));
        if (!world) {
                printf("Err world\n");
                return 1;
        }
        world->entities = list_create_sized(MAX_ENTITIES, sizeof(entity *));
        return 0;
}

void world_udpate()
{
        tick++;
        entity **entities = world->entities->data;
        for (int n = world->entities->n - 1; n >= 0; n--) {
                entity *e = entities[n];
                if (!e->alive) {
                        e->active = false;
                        list_del_at(world->entities, n);
                }
        }
}

void world_draw()
{
}

void world_destroy()
{
        if (world) {
                free(world->entities);
                free(world);
        }
}

uchar get_pixels(int x, int y, int w, int h)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, world->pw);
        const int y2 = MIN(y + h, world->ph);
        uchar px = 0;
        for (int y = y1; y < y2; y++) {
                const int c = y * world->pw;
                for (int x = x1; x < x2; x++)
                        px |= world->pixels[x + c];
        }
        return px;
}

void set_pixels(int x, int y, int w, int h, uchar px)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, world->pw);
        const int y2 = MIN(y + h, world->ph);
        for (int y = y1; y < y2; y++) {
                const int c = y * world->pw;
                for (int x = x1; x < x2; x++)
                        world->pixels[x + c] = px;
        }
}

void add_pixel_flags(int x, int y, int w, int h, uchar px)
{
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + w, world->pw);
        const int y2 = MIN(y + h, world->ph);
        for (int y = y1; y < y2; y++) {
                const int c = y * world->pw;
                for (int x = x1; x < x2; x++)
                        world->pixels[x + c] |= px;
        }
}

entity *entity_create()
{
        for (int n = 0; n < MAX_ENTITIES; n++) {
                if (!entities_raw[n].active) {
                        entity *e = &entities_raw[n];
                        e->active = true;
                        list_push(world->entities, &e);
                        return e;
                }
        }
        return NULL;
}