// Game loop is implemented via fixed-timestep. Reference:
// gafferongames.com/post/fix_your_timestep/

// No interpolation, as this is going to be a pixel art game
// Drawn with discrete upscaled pixels anyway.

#include "gfx.h"
#include "input.h"
#include "mem.h"
#include "raylib.h"
#include "shared.h"
#include "world.h"
#include <limits.h>

#define MEMORY MB(512)
#define DT (1.0 / FPS)

int main(void)
{
#if CHAR_BIT != 8
        return 1;
#endif
        // custom allocator
        void *mem = malloc(MEMORY);
        if (!mem)
                return 1;
        memset(mem, 0, MEMORY);
        mem_init(mem, MEMORY);
        gfx_init();
        world_init();

        double acc = 0;
        double lasttime = GetTime();
        while (!gfx_close_requested()) {
                double time = GetTime();
                acc += time - lasttime;
                lasttime = time;

                // cap delta accumulator ("spiral of death")
                acc = MAX(acc, 0.1);
                if (acc >= DT) {
                        do {
                                btn_update();
                                world_update();
                                acc -= DT;
                        } while (acc >= DT);

                        gfx_begin();
                        world_draw();
                        gfx_line(0, 0, 50, 20, 5);
                        gfx_rec_outline(10, 10, 30, 40, 3);
                        gfx_end();
                }
                gfx_show();
        }
        free(mem);

        return 0;
}