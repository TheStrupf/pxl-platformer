#ifndef WORLD_H
#define WORLD_H

#include "gfx.h"
#include "list.h"
#include "shared.h"

#define NUM_LAYERS 4
#define NUM_TILES 262144 // 512 x 512 tiles
#define PX 8             // pixels per tile width

// a pixel (uchar) is bitmask of properties
enum pixel_flags {
        PX_NONE = 0,
        PX_SOLID = 1,
        PX_PLATFORM = 2,
        PX_3 = 4,
        PX_4 = 8,
        PX_5 = 16,
        PX_6 = 32,
        PX_7 = 64,
        PX_8 = 128,
};

typedef struct entity {
        bool active;
        bool alive;
        int x;
        int y;
        int w;
        int h;
} entity;

typedef struct tile {
        ushort ID;
        uchar flags;
} tile;

typedef struct game_world {
        uint w;
        uint h;
        uint pw;
        uint ph;

        list *entities; // entity*

        tile rtiles[NUM_LAYERS][NUM_TILES];
        uchar pixels[NUM_TILES * PX * PX];
} game_world;

extern uint tick;
extern game_world *world;

int world_init();
void world_udpate();
void world_draw();
void world_destroy();
uchar get_pixels(int x, int y, int w, int h);
void set_pixels(int x, int y, int w, int h, uchar px);
void add_pixel_flags(int x, int y, int w, int h, uchar px);

entity *entity_create();

#endif