/*

Game loop is implemented via fixed-timestep. Reference:
https://gafferongames.com/post/fix_your_timestep/

No interpolation, as this is going to be a pixel art game
Drawn with discrete upscaled pixels anyway.

*/

#include "raylib.h"
#include "shared.h"

#define FPS 60
#define ACC_MIN 0.1
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

const float DT = 1.0 / FPS;

Texture2D screentex;
uchar *screenbuffer;

int main(void)
{
        const int STARTWIDTH = 800;
        const int STARTHEIGHT = 600;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
        InitWindow(STARTWIDTH, STARTHEIGHT, "Hello Git");
        SetTargetFPS(FPS);
        Image i = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
        screentex = LoadTextureFromImage(i);
        UnloadImage(i);
        screenbuffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4); // 4 bytes per pixel
        if (!screenbuffer)
                return 1;

        double acc = 0;
        double lasttime = GetTime();
        while (!WindowShouldClose()) {
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
                        BeginDrawing();
                        ClearBackground(WHITE);
                        UpdateTexture(screentex, screenbuffer);
                        DrawTexture(screentex, 0, 0, WHITE);
                        EndDrawing();
                }
        }
        free(screenbuffer);
        UnloadTexture(screentex);
        CloseWindow();
        return 0;
}