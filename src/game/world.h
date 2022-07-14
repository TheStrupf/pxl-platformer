#ifndef WORLD_H
#define WORLD_H

#include "gfx.h"
#include "list.h"
#include "shared.h"

#define NUM_LAYERS 4
#define NUM_TILES (512 * 512)

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
        list *actors;   // entity*
        list *solids;   // entity*

        uchar tiles[NUM_TILES];
        tile rtiles[NUM_LAYERS][NUM_TILES];
        uchar pixels[NUM_TILES * 64]; // 8x8 pixels per tile
} game_world;

extern uint tick;
extern game_world *world;

// I'm doing the movement logic as outlined in:
// maddythorson.medium.com/celeste-and-towerfall-physics-d24bd2ae0fc5
// Had a good experience with that in that past as well

int world_init();
void world_update();
void world_draw();

void set_tile_pixels(int tx, int ty, int collID);
uchar get_pixels(int x, int y, int w, int h);
void set_pixels(int x, int y, int w, int h, uchar px);
void add_pixel_flags(int x, int y, int w, int h, uchar px);

#endif