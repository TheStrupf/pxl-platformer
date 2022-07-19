#include "gfx.h"
#include "jsonp.h"
#include "mem.h"
#include "raylib.h"
#include "shared.h"
#include "util.h"
#include <string.h>

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

#define GAME_WIDTH 384
#define GAME_HEIGHT 216
#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 256

static Texture2D ray_screentex;
static uchar *screenbuffer;
static tex screentex;
static tex target;
static int offsetx = 0;
static int offsety = 0;

static uchar palette[256];

// game width/height might become variable?
static int width = GAME_WIDTH;
static int height = GAME_HEIGHT;

void gfx_init()
{
        const int STARTWIDTH = 800;
        const int STARTHEIGHT = 600;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
        InitWindow(STARTWIDTH, STARTHEIGHT, "Hello Git");
        SetTargetFPS(120);
        Image img = GenImageColor(BUFFER_WIDTH, BUFFER_HEIGHT, BLACK);
        ray_screentex = LoadTextureFromImage(img);
        UnloadImage(img);
        screenbuffer = mem_alloc(BUFFER_WIDTH * BUFFER_HEIGHT * 4); // 4 bytes per pixel
        screentex = gfx_tex_create(BUFFER_WIDTH, BUFFER_HEIGHT);
}

void gfx_destroy()
{
        UnloadTexture(ray_screentex);
        CloseWindow();
}

void gfx_begin()
{
        target = screentex;
        gfx_set_translation(0, 0);
        gfx_clear(0);
        gfx_pal_reset();
}

void gfx_end()
{
        for (int y = 0, i = 0; y < BUFFER_HEIGHT; y++) {
                const int c = y * BUFFER_WIDTH;
                for (int x = 0; x < BUFFER_WIDTH; x++, i += 4) {
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
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        float scalex = (float)sw / (float)width;
        float scaley = (float)sh / (float)height;
        int gw, gh, tx, ty;
        if (scalex < scaley) { // black bars top and bottom
                gw = sw;
                gh = height * scalex;
                tx = 0;
                ty = (int)((sh - gh) * 0.5);
        } else { // black bars left and right
                gw = width * scaley;
                gh = sh;
                tx = (int)((sw - gw) * 0.5);
                ty = 0;
        }

        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(ray_screentex,
                       (Rectangle){0, 0, width, height},
                       (Rectangle){tx, ty, gw + 0.5, gh + 0.5},
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

int gfx_width()
{
        return width;
}
int gfx_height()
{
        return height;
}

tex gfx_tex_create(uint w, uint h)
{
        tex t;
        t.w = 0;
        t.h = 0;
        t.iw = 1;
        t.ih = 1;
        t.shift = 0;
        while (t.iw < w) {
                t.iw <<= 1;
                t.shift++;
        }
        while (t.ih < h)
                t.ih <<= 1;
        t.px = mem_alloc(t.iw * t.ih);
        if (t.px) {
                t.w = w;
                t.h = h;
                gfx_clear_tex(t, 0);
        }
        return t;
}

tex gfx_tex_load(const char *filename)
{
        char *text = read_txt(filename);
        jsn toks[16];
        json_parse(text, toks, 16);
        jsn *root = &toks[0];
        tex t = gfx_tex_create(json_ik(root, "width"), json_ik(root, "height"));

        int i = json_get(root, "data")->start;
        for (int y = 0; y < t.h; y++) {
                int cache = y << t.shift;
                for (int x = 0; x < t.w; x++) {
                        char a = text[i++];
                        char b = text[i++];
                        a = (a <= '9') ? a - '0' : (a & 0x7) + 9;
                        b = (b <= '9') ? b - '0' : (b & 0x7) + 9;
                        int res = (a << 4) + b;
                        t.px[cache + x] = (uchar)res;
                }
        }

        mem_free(text);
        return t;
}

void gfx_tex_destroy(tex t)
{
        if (t.px)
                mem_free(t.px);
}

void gfx_clear_tex(tex t, uchar col)
{
        if (t.px)
                memset(t.px, col, t.iw * t.ih);
}

void gfx_clear(uchar col)
{
        gfx_clear_tex(target, col);
}

void gfx_px(int x, int y, uchar col)
{
        col = palette[col];
        if (col >= COL_TRANSPARENT)
                return;
        x += offsetx;
        y += offsety;
        if ((uint)x < target.w && (uint)y < target.h)
                target.px[x + (y << target.shift)] = col;
}

void gfx_sprite(tex s, int x, int y, rec r, char flags)
{
        int srcx, srcy;
        int xa = 0;
        int ya = 0;
        switch (flags) {
        case B_00000000:
                srcx = 1;
                srcy = s.w;
                break;
        case B_00000001:
                srcx = -1;
                srcy = s.w;
                xa = r.w - 1;
                break;
        case B_00000010:
                srcx = 1;
                srcy = -s.w;
                ya = r.h - 1;
                break;
        case B_00000011:
                srcx = -1;
                srcy = -s.w;
                xa = r.w - 1;
                ya = r.h - 1;
                break;
        case B_00000100:
                srcx = s.w;
                srcy = 1;
                break;
        case B_00000101:
                srcx = -s.w;
                srcy = 1;
                ya = r.h - 1;
                break;
        case B_00000110:
                srcx = s.w;
                srcy = -1;
                xa = r.w - 1;
                break;
        case B_00000111:
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
        const int x2 = MIN((flags & SPR_FLAG_D) ? r.h : r.w, target.w - x);
        const int y2 = MIN((flags & SPR_FLAG_D) ? r.w : r.h, target.h - y);
        const int c1 = x + (y << target.shift);
        const int c2 = r.x + xa + ((r.y + ya) << s.shift);

        // Loops over target area and maps back to source pixel
        for (int yi = y1; yi < y2; yi++) {
                const int cc = (yi << target.shift) + c1;
                const int cz = yi * srcy + c2;
                for (int xi = x1; xi < x2; xi++) {
                        const uchar col = palette[s.px[srcx * xi + cz]];
                        if (col < COL_TRANSPARENT)
                                target.px[xi + cc] = col;
                }
        }
}

void gfx_sprite_affine(tex s, v2 p, v2 o, rec r, m2 m)
{
        p.x += offsetx;
        p.y += offsety;
        p = V2_ADD(p, o);
        o.x += r.x;
        o.y += r.y;
        const int x2 = r.x + r.w;
        const int y2 = r.y + r.h;
        const m2 m_inv = m2_inv(m);

        // Calculate AABB rectangle by transforming
        // the rectangle's four corners and calculating
        // the min/max for the x & and y axis

        // source texture
        // a---b
        // |   |
        // c---d

        // transform point a
        v2 v = M2V2_MUL(m_inv, p);
        v2 t = (v2){r.x - 1, r.y - 1};
        t = V2_ADD(t, v);
        t = V2_SUB(t, o);
        v = M2V2_MUL(m, t);
        v2 p1 = v;
        v2 p2 = v;

        t.y += r.h + 2; // transform point c
        v = M2V2_MUL(m, t);
        p1 = V2_MIN(p1, v);
        p2 = V2_MAX(p2, v);
        t.x += r.w + 2; // transform point d
        v = M2V2_MUL(m, t);
        p1 = V2_MIN(p1, v);
        p2 = V2_MAX(p2, v);
        t.y -= r.h + 2; // transform point b
        v = M2V2_MUL(m, t);
        p1 = V2_MIN(p1, v);
        p2 = V2_MAX(p2, v);

        // calculate target area
        const int dx1 = MAX((int)(p1.x - 1), 0);
        const int dy1 = MAX((int)(p1.y - 1), 0);
        const int dx2 = MIN((int)(p2.x + 1), target.w);
        const int dy2 = MIN((int)(p2.y + 1), target.h);
        p.y -= 0.5f; // rounding - midpoint of screen pixel
        p.x -= 0.5f;

        // loop over target area and map
        // each SCREEN pixel back to a SOURCE pixel
        for (int dy = dy1; dy < dy2; dy++) {
                const float dyt = dy - p.y;
                const float c1 = m_inv.m12 * dyt + o.x;
                const float c2 = m_inv.m22 * dyt + o.y;
                const int cc = dy << target.shift;
                for (int dx = dx1; dx < dx2; dx++) {
                        const float dxt = dx - p.x;
                        int spx = (int)(m_inv.m11 * dxt + c1);
                        if (spx < r.x || spx >= x2)
                                continue;
                        int spy = (int)(m_inv.m21 * dxt + c2);
                        if (spy < r.y || spy >= y2)
                                continue;
                        uchar col = palette[s.px[spx + (spy << s.shift)]];
                        if (col < COL_TRANSPARENT)
                                target.px[dx + cc] = col;
                }
        }
}

// bresenham
void gfx_line(int x1, int y1, int x2, int y2, uchar col)
{
        col = palette[col];
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
                        target.px[xi + (yi << target.shift)] = col;
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
        col = palette[col];
        if (col >= COL_TRANSPARENT)
                return;
        x += offsetx;
        y += offsety;
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int x2 = MIN(x + (int)w, target.w);
        const int y2 = MIN(y + (int)h, target.h);
#if 1
        const int l = x2 - x1;
        for (int yi = y1; yi < y2; yi++)
                memset(&target.px[x1 + (yi << target.shift)], col, l);
#else
        for (int yi = y1; yi < y2; yi++) {
                const int c = yi << target.shift;
                for (int xi = x1; xi < x2; xi++)
                        target.px[xi + c] = col;
        }
#endif
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

uchar gfx_pal_get(uchar index)
{
        return palette[index];
}

void gfx_pal_shift(int s)
{
}

void gfx_pal_set(uchar index, uchar col)
{
        palette[index] = col;
}

void gfx_pal_reset()
{
        for (int n = 0; n < 256; n++)
                palette[n] = n < COL_TRANSPARENT ? n : 255;
}
