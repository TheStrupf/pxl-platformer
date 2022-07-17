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

#define MEMORY MB(512)
#define DT (1.0 / FPS)

int main(void)
{
        void *mem = malloc(MEMORY);
        if (!mem)
                return 1;
        memset(mem, 0, MEMORY);
        mem_init(mem, MEMORY);
        gfx_init();
        snd_init();

        int jumpsnd = snd_load("assets/snd/jump.wav");

        world_init();

        /* JSON TEST
        {
                "description": "The test case description",
                "schema": { "type": "string" },
                "tests": [
                        {
                        "description": "a test with a valid instance",
                        "data": "a string",
                        "valid": true
                        },
                        {
                        "description": "a test with an invalid instance",
                        "data": 15,
                        "valid": false
                        }
                ]
        }
        */
        char *jsontest = read_txt("test.json");
        // char *jsontest = "{\"description\" : \" The test case description \", \"schema\" : {\"type\" : \"string\"}, \"tests\" : [ { \"description\" : \"a test with a valid instance\",\"data\" : \"a string\",\"valid\" : true},{\"description\" : \"a test with an invalid instance\",\"data\" : 15,\"valid\" : false}]}";
        jsn toks[256];
        if (json_parse(jsontest, toks, 256) == JSON_RET_SUCCESS) {
                json_print_debug(jsontest, toks);
        } else {
                PRINT("Json error\n");
        }

        mem_free(jsontest);

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
                acc = MIN(acc, 0.1);
                if (acc >= DT) {
                        int loopruns = 0;
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
                                loopruns++;
                        } while (acc >= DT);

                        if (loopruns > 1)
                                printf("Skipped %i frames\n", loopruns - 1);

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
        snd_destroy();
        gfx_destroy();
        free(mem);
        return 0;
}