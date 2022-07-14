#include "gfx.h"
#include "raylib.h"
#include "shared.h"

// rgba values for the extended pico-8 palette
// clang-format off
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

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

Texture2D ray_screentex;
uchar *screenbuffer;
tex screentex;
tex target;
int offsetx = 0;
int offsety = 0;

int gfx_init()
{
        const int STARTWIDTH = 800;
        const int STARTHEIGHT = 600;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
        InitWindow(STARTWIDTH, STARTHEIGHT, "Hello Git");
        SetTargetFPS(FPS);
        Image img = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
        ray_screentex = LoadTextureFromImage(img);
        UnloadImage(img);
        screenbuffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4); // 4 bytes per pixel
        screentex = gfx_tex_create(SCREEN_WIDTH, SCREEN_HEIGHT);
        if (!screenbuffer || !screentex.px) {
                gfx_destroy();
                printf("Err gfx\n");
                return 1;
        }
        return 0;
}

void gfx_destroy()
{
        if (screenbuffer)
                free(screenbuffer);
        gfx_tex_destroy(screentex);
        UnloadTexture(ray_screentex);
        CloseWindow();
}

void gfx_begin()
{
        target = screentex;
        gfx_set_translation(0, 0);
        gfx_clear(0);
}

void gfx_end()
{
        for (int y = 0, i = 0; y < SCREEN_HEIGHT; y++) {
                const int c = y * SCREEN_WIDTH;
                for (int x = 0; x < SCREEN_WIDTH; x++, i += 4) {
                        memcpy(
                            &screenbuffer[i],
                            &pico_pal[screentex.px[x + c] << 2],
                            4);
                }
        }
        UpdateTexture(ray_screentex, screenbuffer);
}

void gfx_show()
{
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(ray_screentex, (Rectangle){0, 0, 128, 128},
                       (Rectangle){0, 0, 1024, 1024},
                       (Vector2){0, 0}, 0, WHITE);
        EndDrawing();
}

bool gfx_close_requested()
{
        return WindowShouldClose();
}

void gfx_set_translation(int x, int y)
{
        offsetx = x;
        offsety = y;
}

tex gfx_tex_create(uint w, uint h)
{
        tex t;
        t.px = malloc(w * h);
        if (!t.px)
                t.w = t.h = 0;
        else {
                t.w = w;
                t.h = h;
        }
        return t;
}

void gfx_tex_destroy(tex t)
{
        if (t.px)
                free(t.px);
}

void gfx_clear_tex(tex t, uchar col)
{
        if (t.px)
                memset(t.px, col, t.w * t.h);
}

void gfx_clear(uchar col)
{
        gfx_clear_tex(target, col);
}

void gfx_sprite(tex s, int x, int y, rec r, char flags)
{
        int srcx, srcy;
        int xa = 0;
        int ya = 0;
        switch (flags) {
        case B8(00000000):
                srcx = 1;
                srcy = s.w;
                break;
        case B8(00000001):
                srcx = -1;
                srcy = s.w;
                xa = r.w - 1;
                break;
        case B8(00000010):
                srcx = 1;
                srcy = -s.w;
                ya = r.h - 1;
                break;
        case B8(00000011):
                srcx = -1;
                srcy = -s.w;
                xa = r.w - 1;
                ya = r.h - 1;
                break;
        case B8(00000100):
                srcx = s.w;
                srcy = 1;
                break;
        case B8(00000101):
                srcx = -s.w;
                srcy = 1;
                ya = r.h - 1;
                break;
        case B8(00000110):
                srcx = s.w;
                srcy = -1;
                xa = r.w - 1;
                break;
        case B8(00000111):
                srcx = -s.w;
                srcy = -1;
                xa = r.w - 1;
                ya = r.h - 1;
                break;
        default:
                return;
        }
        x += offsetx;
        y += offsety;
        const int x1 = MAX(0, -x);
        const int y1 = MAX(0, -y);
        const int x2 = MIN((flags & SPR_FLAG_D) > 0 ? r.h : r.w, target.w - x);
        const int y2 = MIN((flags & SPR_FLAG_D) > 0 ? r.w : r.h, target.h - y);

        for (int yi = y1; yi < y2; yi++) {
                const int cc = (yi * target.w) + x + (y * target.w);
                const int cz = yi * srcy + r.x + xa + ((r.y + ya) * s.w);
                for (int xi = x1; xi < x2; xi++) {
                        const uchar col = s.px[srcx * xi + cz];
                        if (col < COL_TRANSPARENT)
                                target.px[xi + cc] = col;
                }
        }
}

// bresenham
void gfx_line(int x1, int y1, int x2, int y2, uchar col)
{
        if (col >= COL_TRANSPARENT)
                return;

        const int dx = ABS(x2 - x1);
        const int dy = -ABS(y2 - y1);
        const int sx = x2 > x1 ? 1 : -1;
        const int sy = y2 > y1 ? 1 : -1;
        const int steps = MAX(dx, -dy);
        int err = dx + dy;
        int xi = x1 + offsetx;
        int yi = y1 + offsety;
        for (int n = 0; n <= steps; n++) {
                if ((uint)xi < target.w && (uint)yi < target.h)
                        target.px[xi + (yi * target.w)] = col;
                int e2 = err << 1;
                if (e2 >= dy) {
                        err += dy;
                        xi += sx;
                }
                if (e2 <= dx) {
                        err += dx;
                        yi += sy;
                }
        }
}

void gfx_rec_filled(int x, int y, uint w, uint h, uchar col)
{
        if (col >= COL_TRANSPARENT)
                return;
        x += offsetx;
        y += offsety;
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + (int)w, target.w);
        const int y2 = MIN(y + (int)h, target.h);
        for (int ty = y1; ty < y2; ty++) {
                const int c = ty * target.w;
                for (int tx = x1; tx < x2; tx++)
                        target.px[tx + c] = col;
        }
}

void gfx_rec_outline(int x, int y, uint w, uint h, uchar col)
{
        const int x1 = x;
        const int y1 = y;
        const int x2 = x + (int)w;
        const int y2 = y + (int)h;
        gfx_line(x1, y1, x1, y2, col);
        gfx_line(x1, y1, x2, y1, col);
        gfx_line(x2, y1, x2, y2, col);
        gfx_line(x1, y2, x2, y2, col);
}