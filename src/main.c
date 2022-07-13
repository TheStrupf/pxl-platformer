/*

Game loop is implemented via fixed-timestep. Reference:
https://gafferongames.com/post/fix_your_timestep/

No interpolation, as this is going to be a pixel art game
Drawn with discrete upscaled pixels anyway.

*/

#include "raylib.h"
#include <float.h>
#include <stdbool.h>

#define FPS 60
#define ACC_MIN 0.1

const float DT = 1.0 / FPS;

int main(void)
{
        const int STARTWIDTH = 800;
        const int STARTHEIGHT = 600;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
        InitWindow(STARTWIDTH, STARTHEIGHT, "Hello Git");
        SetTargetFPS(FPS);
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
                        acc -= DT;
                        draw = true;
                }

                if (draw) {
                        BeginDrawing();
                        ClearBackground(WHITE);
                        DrawRectangle(10, 10, 10, 10, RED);
                        EndDrawing();
                }
        }
        CloseWindow();
        return 0;
}