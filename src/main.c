// Game loop is implemented via fixed-timestep. Reference:
// gafferongames.com/post/fix_your_timestep/

// No interpolation, as this is going to be a pixel art game
// Drawn with discrete upscaled pixels anyway.

#include "gfx.h"
#include "raylib.h"
#include "shared.h"
#include "world.h"

#define ACC_MIN 0.1
const float DT = 1.0 / FPS;

int main(void)
{
        if (gfx_init() > 0)
                goto CLEAN_1;
        if (world_init() > 0)
                goto CLEAN_2;

        double acc = 0;
        double lasttime = GetTime();
        while (!gfx_close_requested()) {
                double time = GetTime();
                acc += time - lasttime;
                if (acc > ACC_MIN)
                        acc = ACC_MIN;
                lasttime = time;

                bool draw = false;
                while (acc >= DT) {
                        world_udpate();
                        acc -= DT;
                        draw = true;
                }

                if (draw) {
                        gfx_begin();
                        world_draw();
                        gfx_line(0, 0, 50, 20, 5);
                        gfx_rec_outline(10, 10, 30, 40, 3);
                        gfx_end();
                }
                // draw rendertexture to not mess with
                // raylib's frame counting
                gfx_show();
        }
CLEAN_2:
        world_destroy();
CLEAN_1:
        gfx_destroy();
        return 0;
}