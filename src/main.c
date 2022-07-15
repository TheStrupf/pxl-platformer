// Game loop is implemented via fixed-timestep. Reference:
// gafferongames.com/post/fix_your_timestep/

// No interpolation, as this is going to be a pixel art game
// Drawn with discrete upscaled pixels anyway.

#include "gfx.h"
#include "input.h"
#include "list.h"
#include "mem.h"
#include "raylib.h"
#include "shared.h"
#include "world.h"

#define MEMORY MB(512)
#define DT (1.0 / FPS)

int main(void)
{
        // custom allocator
        void *mem = malloc(MEMORY);
        if (!mem)
                return 1;
        memset(mem, 0, MEMORY);
        mem_init(mem, MEMORY);
        gfx_init();
        world_init();

        tex src = gfx_tex_create(64, 64);
        for (int n = 0; n < 64 * 64; n++)
                src.px[n] = n & 31;
        float sclx = 1;
        float scly = 1;
        float rot = 0;
        float shx = 0;
        float shy = 0;
        const float S = 0.01f;
        const float SH = 0.01f;
        const float R = 0.01f;

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
                                if (btn_pressed(BTN_UP))
                                        sclx += S;
                                if (btn_pressed(BTN_DOWN))
                                        sclx -= S;
                                if (btn_pressed(BTN_LEFT))
                                        scly += S;
                                if (btn_pressed(BTN_RIGHT))
                                        scly -= S;
                                if (btn_pressed(BTN_L_SHOULDER))
                                        rot -= R;
                                if (btn_pressed(BTN_R_SHOULDER))
                                        rot += R;
                                if (btn_pressed(BTN_FACE_LEFT))
                                        shx -= SH;
                                if (btn_pressed(BTN_FACE_RIGHT))
                                        shx += SH;
                                if (btn_pressed(BTN_FACE_UP))
                                        shy -= SH;
                                if (btn_pressed(BTN_FACE_DOWN))
                                        shy += SH;

                                btn_update();
                                world_update();
                                acc -= DT;
                        } while (acc >= DT);

                        gfx_begin();
                        world_draw();
                        gfx_line(0, 0, 50, 20, 5);
                        gfx_rec_outline(10, 10, 30, 40, 3);

                        m2 mt = m2_I();
                        rec r = (rec){0, 0, 64, 64};
                        v2 p = (v2){0, 0};
                        v2 o = (v2){64, 32};
                        mt = m2_mul(mt, m2_aff_rot(rot));
                        mt = m2_mul(mt, m2_aff_scl(sclx, scly));
                        mt = m2_mul(mt, m2_aff_shr(shx, shy));
                        gfx_sprite_affine(src, p, o, r, mt);
                        gfx_end();
                }
                gfx_show();
        }
        free(mem);
        return 0;
}