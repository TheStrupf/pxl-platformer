/*
Game loop is implemented via fixed-timestep. Reference:
gafferongames.com/post/fix_your_timestep/

No interpolation, as this is going to be a pixel art game
Drawn with discrete upscaled pixels anyway.
*/

#include "gfx.h"
#include "raylib.h"
#include "shared.h"

#define FPS 60
#define ACC_MIN 0.1
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

// clang-format off
// rgba values for the extended pico-8 palette
uchar pico_pal[128] = {
        0x00, 0x00, 0x00, 0xFF, 0x1D, 0x2B, 0x53, 0xFF,
        0x7E, 0x25, 0x53, 0xFF, 0x00, 0x87, 0x51, 0xFF,
        0xAB, 0x52, 0x36, 0xFF, 0x5F, 0x57, 0x4F, 0xFF,
        0xC2, 0xC3, 0xC7, 0xFF, 0xFF, 0xF1, 0xE8, 0xFF,
        0xFF, 0x00, 0x4D, 0xFF, 0xFF, 0xA3, 0x00, 0xFF,
        0xFF, 0xEC, 0x27, 0xFF, 0x00, 0xE4, 0x36, 0xFF,
        0x29, 0xAD, 0xFF, 0xFF, 0x83, 0x76, 0x9C, 0xFF,
        0xFF, 0x77, 0xA8, 0xFF, 0xFF, 0xCC, 0xAA, 0xFF,
        0x29, 0x18, 0x14, 0xFF, 0x11, 0x1D, 0x35, 0xFF,
        0x42, 0x21, 0x36, 0xFF, 0x12, 0x53, 0x59, 0xFF,
        0x74, 0x2F, 0x29, 0xFF, 0x49, 0x33, 0x3B, 0xFF,
        0xA2, 0x88, 0x79, 0xFF, 0xF3, 0xEF, 0x7D, 0xFF,
        0xBE, 0x12, 0x50, 0xFF, 0xFF, 0x6C, 0x24, 0xFF,
        0xA8, 0xE7, 0x2E, 0xFF, 0x00, 0xB5, 0x43, 0xFF,
        0x06, 0x5A, 0xB5, 0xFF, 0x75, 0x46, 0x65, 0xFF,
        0xFF, 0x6E, 0x59, 0xFF, 0xFF, 0x9D, 0x81, 0xFF,
};
// clang-format on

const float DT = 1.0 / FPS;

Texture2D screentex;
uchar *screenbuffer;
tex pixeltex;

int main(void)
{
        const int STARTWIDTH = 800;
        const int STARTHEIGHT = 600;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
        InitWindow(STARTWIDTH, STARTHEIGHT, "Hello Git");
        SetTargetFPS(FPS);
        Image img = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
        screentex = LoadTextureFromImage(img);
        UnloadImage(img);
        screenbuffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4); // 4 bytes per pixel
        if (!screenbuffer)
                return 1;
        pixeltex = gfx_tex_create(SCREEN_WIDTH, SCREEN_HEIGHT);
        tex srctex = gfx_tex_create(32, 32);
        int i = 0;
        for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 32; x++) {
                        srctex.pixels[i++] = (i % 32);
                }
        }
        srctex.pixels[31 + 31 * 32] = 8;

        uchar flags = 0;

        double acc = 0;
        double lasttime = GetTime();
        while (!WindowShouldClose()) {
                if (IsKeyPressed(KEY_A))
                        flags ^= 1;
                if (IsKeyPressed(KEY_S))
                        flags ^= 2;
                if (IsKeyPressed(KEY_D))
                        flags ^= 4;
                double time = GetTime();
                acc += time - lasttime;
                if (acc > ACC_MIN)
                        acc = ACC_MIN;
                lasttime = time;

                bool draw = false;
                while (acc >= DT) {
                        // update
                        acc -= DT;
                        draw = true;
                }

                if (draw) {
                        gfx_clear(pixeltex, 2);
                        gfx_sprite(pixeltex, srctex, 10, 10, (rec){0, 0, 32, 32}, flags);
                        for (int y = 0, i = 0; y < SCREEN_HEIGHT; y++) {
                                for (int x = 0; x < SCREEN_WIDTH; x++, i += 4) {
                                        memcpy(
                                            screenbuffer + i,
                                            &pico_pal[pixeltex.pixels[x + y * SCREEN_WIDTH] * 4],
                                            4);
                                }
                        }
                        UpdateTexture(screentex, screenbuffer);
                        BeginDrawing();
                        ClearBackground(WHITE);
                        DrawTexturePro(screentex, (Rectangle){0, 0, 128, 128},
                                       (Rectangle){0, 0, 1024, 1024},
                                       (Vector2){0, 0}, 0, WHITE);
                        EndDrawing();
                }
        }
        gfx_tex_destroy(pixeltex);
        gfx_tex_destroy(srctex);
        free(screenbuffer);
        UnloadTexture(screentex);
        CloseWindow();
        return 0;
}