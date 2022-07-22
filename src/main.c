// Game loop is implemented via fixed-timestep. Reference:
// gafferongames.com/post/fix_your_timestep/

// No interpolation, as this is going to be a pixel art game
// Drawn with discrete upscaled pixels anyway.

#include "gfx.h"
#include "input.h"
#include "jsonp.h"
#include "list.h"
#include "mem.h"
#include "raylib.h"
#include "shared.h"
#include "snd.h"
#include "util.h"
#include "world.h"
#include <time.h>

#define MEMORY_DYN MB(512)
#define MEMORY_STACK MB(32)
#define DT (1.0 / FPS)

int main(void)
{

        void *mem = malloc(MEMORY_DYN + MEMORY_STACK);
        if (!mem)
                return 1;
        memset(mem, 0, MEMORY_STACK + MEMORY_DYN);
        mem_init(mem, MEMORY_STACK, MEMORY_DYN);
        gfx_init();
        snd_init();

        int jumpsnd = snd_load("assets/snd/jump.wav");
        world_init();

        double acc = 0;
        double lasttime = GetTime();
        while (!gfx_close_requested()) {
                double time = GetTime();
                acc += time - lasttime;
                lasttime = time;

                // cap delta accumulator ("spiral of death")
                acc = MIN(acc, 0.1);
                if (acc >= DT) {
                        int loopruns = 0;
                        do {
                                btn_update();
                                world_update();
                                acc -= DT;
                                loopruns++;
                        } while (acc >= DT);

                        if (loopruns > 1)
                                printf("Skipped %i frames\n", loopruns - 1);

                        gfx_begin();
                        world_draw();
                        gfx_circle_outline(10, 10, 100, 18);
                        gfx_end();
                }
                gfx_show();
        }

        snd_destroy();
        gfx_destroy();
        free(mem);
        return 0;
}