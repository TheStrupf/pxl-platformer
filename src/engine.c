#include "engine.h"
#include "jsonp.h"
#include "raylib.h"
#include "world.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ALIGN(s) ((s + ALIGNMENT - 1) & -ALIGNMENT)

typedef struct block {
        size_t size;
        struct block *next;
} block;

static const double DELTA = 1.0 / (double)FPS;
static const int ALIGNMENT = sizeof(void *);
static const int HEADER = ALIGN(sizeof(block));
static const int SPLIT_THRESH = HEADER + ALIGNMENT;

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

static struct engine {
        Texture2D ray_screentex;
        uchar *screenbuffer;
        tex screentex;
        tex target;
        int offsetx;
        int offsety;
        uchar palette[256];

        // game width/height might become variable?
        int width;
        int height;
        buttons btn_state;
        buttons btn_pstate;

        Sound sounds[256];
        int soundID;
        float volume;
        bool snd_okay;
        void *memory;
        block *b_free;
} eng;

static void bmbtn_update();

void vm_run()
{

        eng = (struct engine){0};
        eng.memory = malloc(MEMORY);
        if (!eng.memory) return;
        eng.b_free = (block *)eng.memory;
        eng.b_free->size = MEMORY;
        eng.b_free->next = NULL;
        eng.width = GAME_WIDTH;
        eng.height = GAME_HEIGHT;
        eng.volume = 1.0f;
        eng.soundID = -1;

        const int STARTWIDTH = 800;
        const int STARTHEIGHT = 600;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
        InitWindow(STARTWIDTH, STARTHEIGHT, "Hello Git");
        SetTargetFPS(144);
        Image img = GenImageColor(BUFFER_WIDTH, BUFFER_HEIGHT, BLACK);
        eng.ray_screentex = LoadTextureFromImage(img);
        UnloadImage(img);
        eng.ray_screentex.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        eng.screenbuffer = vmalloc(BUFFER_WIDTH * BUFFER_HEIGHT * 4); // 4 bytes per pixel
        eng.screentex = vmgfx_tex_create(BUFFER_WIDTH, BUFFER_HEIGHT);
        InitAudioDevice();
        eng.snd_okay = IsAudioDeviceReady();

        world_init();

        double acc = 0;
        double lasttime = GetTime();
        while (!WindowShouldClose()) {
                double time = GetTime();
                acc += time - lasttime;
                lasttime = time;

                // cap delta accumulator ("spiral of death")
                acc = MIN(acc, 0.1);
                if (acc >= DELTA) {
                        int loopruns = 0;
                        do {
                                bmbtn_update();
                                world_update();
                                acc -= DELTA;
                                loopruns++;
                        } while (acc >= DELTA);

                        if (loopruns > 1)
                                printf("Skipped %i frames\n", loopruns - 1);

                        eng.target = eng.screentex;
                        vmgfx_set_translation(0, 0);
                        vmgfx_clear(0);
                        vmgfx_pal_reset();
                        world_draw();
                        vmgfx_rec_filled(-20, 10, 15, 15, 1);
                        for (int y = 0, i = 0; y < BUFFER_HEIGHT; y++) {
                                const int c = y * BUFFER_WIDTH;
                                for (int x = 0; x < BUFFER_WIDTH; x++, i += 4)
                                        memcpy(&eng.screenbuffer[i], &pico_pal[eng.screentex.px[x + c] << 2], 4);
                        }
                        UpdateTexture(eng.ray_screentex, eng.screenbuffer);
                }
                // letterboxing
                int sw = GetScreenWidth();
                int sh = GetScreenHeight();
                float scalex = (float)sw / (float)eng.width;
                float scaley = (float)sh / (float)eng.height;
                int gw, gh, tx, ty;
                if (scalex < scaley) { // black bars top and bottom
                        gw = sw;
                        gh = eng.height * scalex;
                        tx = 0;
                        ty = (int)((sh - gh) * 0.5);
                } else { // black bars left and right
                        gw = eng.width * scaley;
                        gh = sh;
                        tx = (int)((sw - gw) * 0.5);
                        ty = 0;
                }

                BeginDrawing();
                ClearBackground(WHITE);
                DrawTexturePro(eng.ray_screentex,
                               (Rectangle){0, 0, eng.width, eng.height},
                               (Rectangle){tx, ty, gw + 0.5, gh + 0.5},
                               (Vector2){0, 0}, 0, WHITE);
                EndDrawing();
        }
        if (eng.snd_okay) {
                for (int n = eng.soundID; n >= 0; n--)
                        UnloadSound(eng.sounds[n]);
                CloseAudioDevice();
        }
        UnloadTexture(eng.ray_screentex);
        CloseWindow();
        free(eng.memory);
}

void vmgfx_set_translation(int x, int y)
{
        eng.offsetx = x;
        eng.offsety = y;
}

int vmgfx_width()
{
        return eng.width;
}

int vmgfx_height()
{
        return eng.height;
}

tex vmgfx_screen_tex()
{
        return eng.screentex;
}

tex vmgfx_tex_create(uint w, uint h)
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
        while (t.ih < h) t.ih <<= 1;
        t.px = vmalloc(t.iw * t.ih);
        if (t.px) {
                t.w = w;
                t.h = h;
                vmgfx_clear_tex(t, 0);
        }
        return t;
}

tex vmgfx_tex_load(const char *filename)
{
        char *text = read_txt(filename);
        jsn toks[16];
        json_parse(text, toks, 16);
        jsn *root = &toks[0];
        tex t = vmgfx_tex_create(json_ik(root, "width"), json_ik(root, "height"));

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
        vmfree(text);
        return t;
}

void vmgfx_tex_destroy(tex t)
{
        if (t.px) vmfree(t.px);
}

void vmgfx_clear_tex(tex t, uchar c)
{
        if (t.px) memset(t.px, c, t.iw * t.ih);
}

void vmgfx_clear(uchar col)
{
        vmgfx_clear_tex(eng.target, col);
}

void vmgfx_px(int x, int y, uchar col)
{
        col = eng.palette[col];
        if (col == COL_TRANSPARENT) return;
        x += eng.offsetx;
        y += eng.offsety;
        if ((uint)x < eng.target.w && (uint)y < eng.target.h)
                eng.target.px[x + (y << eng.target.shift)] = col;
}

uchar vmgfx_closest_col(uchar r, uchar g, uchar b)
{
        return 0;
}

uchar vmgfx_closest_col_merged(uchar col, uchar r, uchar g, uchar b, uchar a)
{
        return 0;
}

void vmgfx_sprite(tex s, int x, int y, rec r, uchar flags)
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
        default: return;
        }
        x += eng.offsetx;
        y += eng.offsety;
        int x1 = MAX(0, -x);
        int y1 = MAX(0, -y);
        int x2 = MIN((flags & 0x4) ? r.h : r.w, eng.target.w - x);
        int y2 = MIN((flags & 0x4) ? r.w : r.h, eng.target.h - y);
        int c1 = x + (y << eng.target.shift);
        int c2 = r.x + xa + ((r.y + ya) << s.shift);

        // Loops over target area and maps back to source pixel
        for (int yi = y1; yi < y2; yi++) {
                int cc = (yi << eng.target.shift) + c1;
                int cz = yi * srcy + c2;
                for (int xi = x1; xi < x2; xi++) {
                        uchar col = eng.palette[s.px[srcx * xi + cz]];
                        if (col != COL_TRANSPARENT)
                                eng.target.px[xi + cc] = col;
                }
        }
}

void vmgfx_sprite_affine(tex s, v2 p, v2 o, rec r, m2 m)
{
        p.x += eng.offsetx;
        p.y += eng.offsety;
        p = v2_add(p, o);
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
        v2 v = m2v2_mul(m_inv, p);
        v2 t = (v2){r.x - 1, r.y - 1};
        t = v2_add(t, v);
        t = v2_sub(t, o);
        v = m2v2_mul(m, t);
        v2 p1 = v;
        v2 p2 = v;
        t.y += r.h + 2; // transform point c
        v = m2v2_mul(m, t);
        p1 = v2_min(p1, v);
        p2 = v2_max(p2, v);
        t.x += r.w + 2; // transform point d
        v = m2v2_mul(m, t);
        p1 = v2_min(p1, v);
        p2 = v2_max(p2, v);
        t.y -= r.h + 2; // transform point b
        v = m2v2_mul(m, t);
        p1 = v2_min(p1, v);
        p2 = v2_max(p2, v);

        // calculate target area
        int dx1 = MAX((int)(p1.x - 1), 0);
        int dy1 = MAX((int)(p1.y - 1), 0);
        int dx2 = MIN((int)(p2.x + 1), eng.target.w);
        int dy2 = MIN((int)(p2.y + 1), eng.target.h);
        p.y -= 0.5f; // rounding - midpoint of screen pixel
        p.x -= 0.5f;

        // loop over target area and map
        // each SCREEN pixel back to a SOURCE pixel
        for (int dy = dy1; dy < dy2; dy++) {
                float dyt = dy - p.y;
                float c1 = m_inv.m12 * dyt + o.x;
                float c2 = m_inv.m22 * dyt + o.y;
                int cc = dy << eng.target.shift;
                for (int dx = dx1; dx < dx2; dx++) {
                        float dxt = dx - p.x;
                        int spx = (int)(m_inv.m11 * dxt + c1);
                        if (spx < r.x || spx >= x2) continue;
                        int spy = (int)(m_inv.m21 * dxt + c2);
                        if (spy < r.y || spy >= y2) continue;
                        uchar col = eng.palette[s.px[spx + (spy << s.shift)]];
                        if (col != COL_TRANSPARENT)
                                eng.target.px[dx + cc] = col;
                }
        }
}

// bresenham
void vmgfx_line(int x1, int y1, int x2, int y2, uchar col)
{
        col = eng.palette[col];
        if (col == COL_TRANSPARENT) return;
        const int dx = ABS(x2 - x1);
        const int dy = -ABS(y2 - y1);
        const int sx = x2 > x1 ? 1 : -1;
        const int sy = y2 > y1 ? eng.target.iw : -eng.target.iw;
        const int steps = MAX(dx, -dy);
        int err = dx + dy;
        int xi = x1 + eng.offsetx;
        int yi = (y1 + eng.offsety) << eng.target.shift;
        for (int n = 0; n <= steps; n++) {
                if ((uint)xi < eng.target.w && (uint)yi < eng.target.h)
                        eng.target.px[xi + yi] = col;
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

void vmgfx_rec_filled(int x, int y, uint w, uint h, uchar col)
{
        col = eng.palette[col];
        if (col == COL_TRANSPARENT) return;
        x += eng.offsetx;
        y += eng.offsety;
        const int x2 = MIN(x + (int)w, eng.target.w);
        const int y2 = MIN(y + (int)h, eng.target.h);
        if (x2 <= 0 || y2 <= 0 || x >= eng.target.w || y >= eng.target.h)
                return;
        const int x1 = MAX(x, 0);
        const int y1 = MAX(y, 0);
        const int l = x2 - x1;
        for (int yi = y1; yi < y2; yi++)
                memset(&eng.target.px[x1 + (yi << eng.target.shift)], col, l);
}

void vmgfx_rec_outline(int x, int y, uint w, uint h, uchar col)
{
        const int x1 = x;
        const int y1 = y;
        const int x2 = x + (int)w;
        const int y2 = y + (int)h;
        vmgfx_line(x1, y1, x1, y2, col);
        vmgfx_line(x1, y1, x2, y1, col);
        vmgfx_line(x2, y1, x2, y2, col);
        vmgfx_line(x1, y2, x2, y2, col);
}

static void vmgfx_internal_circle(int y, int x1, int x2, uchar col, bool filled)
{
        if ((uint)y >= eng.target.h) return;
        if (filled) {
                y <<= eng.target.shift;
                x1 = MAX(x1, 0) + y;
                x2 = MIN(x2, eng.target.w - 1) + y;
                for (int n = x1; n <= x2; n++)
                        eng.target.px[n] = col;
        } else {
                y <<= eng.target.shift;
                if ((uint)x1 < eng.target.w)
                        eng.target.px[x1 + y] = col;
                if ((uint)x2 < eng.target.w)
                        eng.target.px[x2 + y] = col;
        }
}

static void vmgfx_circle(int xm, int ym, int r, uchar col, bool filled)
{
        col = eng.palette[col];
        if (col == COL_TRANSPARENT) return;
        xm += eng.offsetx;
        ym += eng.offsety;

        int x = 0;
        int y = r;
        int d = 3 - 2 * r;
        vmgfx_internal_circle(ym + y, xm, xm, col, filled);
        vmgfx_internal_circle(ym - y, xm, xm, col, filled);
        vmgfx_internal_circle(ym, xm - y, xm + y, col, filled);
        while (y >= x++) {
                if (d > 0) d += ((x - --y) << 2) + 10;
                else d += (x << 2) + 6;
                vmgfx_internal_circle(ym + y, xm - x, xm + x, col, filled);
                vmgfx_internal_circle(ym - y, xm - x, xm + x, col, filled);
                vmgfx_internal_circle(ym + x, xm - y, xm + y, col, filled);
                vmgfx_internal_circle(ym - x, xm - y, xm + y, col, filled);
        }
}

void vmgfx_circle_filled(int xm, int ym, int r, uchar col)
{
        vmgfx_circle(xm, ym, r, col, true);
}

void vmgfx_circle_outline(int xm, int ym, int r, uchar col)
{
        vmgfx_circle(xm, ym, r, col, false);
}

uchar vmgfx_pal_get(int index)
{
        return eng.palette[index];
}

void vmgfx_pal_shift(int s)
{
}

void vmgfx_pal_set(uchar index, uchar col)
{
        eng.palette[index] = col;
}

void vmgfx_pal_reset()
{
        for (int n = 0; n < 256; n++)
                eng.palette[n] = n != COL_TRANSPARENT ? n : 255;
}

snd vmsnd_load(const char *filename)
{
        snd s;
        s.ID = 0;
        if (eng.snd_okay) {
                Sound sound = LoadSound(filename);
                s.ID = ++eng.soundID;
        }
        return s;
}

void vmsnd_play_sound(snd s)
{
        if (eng.snd_okay && s.ID >= 0)
                PlaySound(eng.sounds[s.ID]);
}

void vmsnd_set_volume(float v)
{
        if (!eng.snd_okay) return;
        v = MAX(v, 1.0f);
        v = MIN(v, 0.0f);
        SetMasterVolume(v);
        eng.volume = v;
}

float vmsnd_volume()
{
        if (!eng.snd_okay) return 0;
        return eng.volume;
}

// Hardcoded input for now
// Will be changed to be flexible in the future

static void bmbtn_update()
{
        eng.btn_pstate = eng.btn_state;
        eng.btn_state = 0;
        if (IsKeyDown(KEY_ENTER))
                eng.btn_state |= BTN_START;
        if (IsKeyDown(KEY_BACKSPACE))
                eng.btn_state |= BTN_SELECT;
        if (IsKeyDown(KEY_A))
                eng.btn_state |= BTN_LEFT;
        if (IsKeyDown(KEY_D))
                eng.btn_state |= BTN_RIGHT;
        if (IsKeyDown(KEY_W))
                eng.btn_state |= BTN_UP;
        if (IsKeyDown(KEY_S))
                eng.btn_state |= BTN_DOWN;
        if (IsKeyDown(KEY_LEFT))
                eng.btn_state |= BTN_FACE_LEFT;
        if (IsKeyDown(KEY_RIGHT))
                eng.btn_state |= BTN_FACE_RIGHT;
        if (IsKeyDown(KEY_UP))
                eng.btn_state |= BTN_FACE_UP;
        if (IsKeyDown(KEY_DOWN))
                eng.btn_state |= BTN_FACE_DOWN;
        if (IsKeyDown(KEY_Q))
                eng.btn_state |= BTN_L_SHOULDER;
        if (IsKeyDown(KEY_R))
                eng.btn_state |= BTN_R_SHOULDER;
}

buttons vmbtn_state()
{
        return eng.btn_state;
}

buttons vmbtn_pstate()
{
        return eng.btn_pstate;
}

int vmbtn_xdir()
{
        if (vmbtn_pressed(BTN_LEFT | BTN_RIGHT)) return 0;
        if (vmbtn_pressed(BTN_RIGHT)) return 1;
        if (vmbtn_pressed(BTN_LEFT)) return -1;
        return 0;
}

int vmbtn_ydir()
{
        if (vmbtn_pressed(BTN_UP | BTN_DOWN)) return 0;
        if (vmbtn_pressed(BTN_DOWN)) return 1;
        if (vmbtn_pressed(BTN_UP)) return -1;
        return 0;
}

bool vmbtn_pressed(buttons b)
{
        return (eng.btn_state & b) == b;
}

bool vmbtn_was_pressed(buttons b)
{
        return (eng.btn_pstate & b) == b;
}

bool vmbtn_just_pressed(buttons b)
{
        return (!vmbtn_was_pressed(b) && vmbtn_pressed(b));
}

bool vmbtn_just_released(buttons b)
{
        return (vmbtn_was_pressed(b) && !vmbtn_pressed(b));
}

void *vmalloc(size_t s)
{
        if (s == 0) return NULL;
        const size_t ALLOC_SIZE = ALIGN(s) + HEADER;
        for (block *b = eng.b_free, *p = NULL; b; p = b, b = b->next) {
                if (b->size >= ALLOC_SIZE) {
                        size_t excess = b->size - ALLOC_SIZE;
                        block *tmp = b->next;
                        if (excess >= SPLIT_THRESH) {
                                // split blocks
                                b->size = ALLOC_SIZE;
                                tmp = (block *)((char *)b + ALLOC_SIZE);
                                tmp->size = excess;
                                tmp->next = b->next;
                        }
                        // re-link free list
                        if (p) p->next = tmp;
                        else eng.b_free = tmp;
                        return (void *)((char *)b + HEADER);
                }
        }
        printf("+++ ERR: Couldn't alloc memory\n");
        return NULL;
}

void *vmrealloc(void *p, size_t s)
{
        if (!p) return vmalloc(s);
        const block *b = (block *)((char *)p - HEADER);
        const size_t usersize = b->size - HEADER;
        if (usersize < s) {
                void *newblock = vmalloc(s);
                if (newblock) {
                        memcpy(newblock, p, usersize);
                        vmfree(p);
                        return newblock;
                }
                return NULL;
        }
        return p;
}

void vmfree(void *p)
{
        if (!p) return;
        block *b = (block *)((char *)p - HEADER);
        block *prev = b->next = NULL;
        const char *b_ptr = (char *)b;

        // iterate free list to insert b (memory sorted)
        for (block *it = eng.b_free; it; prev = it, it = it->next) {
                if (b_ptr < (char *)it) {
                        // insert b before it
                        if (b_ptr + b->size == (char *)it) {
                                // end of block b ends at it: merge blocks
                                b->size += it->size;
                                b->next = it->next;
                        } else b->next = it;
                        break;
                }
        }

        if (prev) {
                if ((char *)prev + prev->size == (char *)b) {
                        // previous block p ends at b: merge blocks
                        prev->size += b->size;
                        prev->next = b->next;
                } else prev->next = b;
        } else eng.b_free = b;
}

size_t mem_align(size_t s)
{
        return ALIGN(s);
}

char *read_txt(const char *filename)
{
        FILE *fptr = fopen(filename, "r");
        if (!fptr) {
                printf("Error opening txt\n");
                return NULL;
        }
        fseek(fptr, 0, SEEK_END);
        long length = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        char *txt = vmalloc(length + 1);
        if (!txt) {
                printf("Error allocating txt buffer\n");
                fclose(fptr);
                return NULL;
        }
        fread(txt, 1, length, fptr);
        txt[length] = '\0';
        fclose(fptr);
        return txt;
}

ulong isqrt(ulong x)
{
        ulong r = x, q = 0, b = 0x40000000;
        while (b > r) b >>= 2;
        while (b > 0) {
                ulong t = q + b;
                q >>= 1;
                if (r >= t) {
                        r -= t;
                        q += b;
                }
                b >>= 2;
        }
        return q;
}

bool try_rec_intersection(rec a, rec b, rec *r)
{
        rec c = rec_intersection(a, b);
        if (c.w > 0 && c.h > 0) {
                *r = c;
                return true;
        }
        return false;
}

rec rec_intersection(rec a, rec b)
{
        rec c;
        c.x = MAX(a.x, b.x);
        c.y = MAX(a.y, b.y);
        c.w = MIN(a.x + a.w, b.x + b.w) - c.x;
        c.h = MIN(a.y + a.h, b.y + b.h) - c.y;
        return c;
}

bool rec_overlap(rec a, rec b)
{
        return (
            a.x < b.x + b.w &&
            a.y < b.y + b.h &&
            b.x < a.x + a.w &&
            b.y < a.y + a.h);
}

m2 m2_I()
{
        m2 r;
        r.m11 = r.m22 = 1;
        r.m12 = r.m21 = 0;
        return r;
}

m2 m2_add(m2 a, m2 b)
{
        m2 r;
        r.m11 = a.m11 + b.m11;
        r.m12 = a.m12 + b.m12;
        r.m21 = a.m21 + b.m21;
        r.m22 = a.m22 + b.m22;
        return r;
}

m2 m2_sub(m2 a, m2 b)
{
        m2 r;
        r.m11 = a.m11 - b.m11;
        r.m12 = a.m12 - b.m12;
        r.m21 = a.m21 - b.m21;
        r.m22 = a.m22 - b.m22;
        return r;
}

m2 m2_mul(m2 a, m2 b)
{
        m2 r;
        r.m11 = a.m11 * b.m11 + a.m12 * b.m21;
        r.m12 = a.m11 * b.m12 + a.m12 * b.m22;
        r.m21 = a.m21 * b.m11 + a.m22 * b.m21;
        r.m22 = a.m21 * b.m12 + a.m22 * b.m22;
        return r;
}

m2 m2_scl(m2 a, float s)
{
        m2 r;
        r.m11 = a.m11 * s;
        r.m12 = a.m12 * s;
        r.m21 = a.m21 * s;
        r.m22 = a.m22 * s;
        return r;
}

m2 m2_inv(m2 a)
{
        m2 r;
        r.m11 = a.m22;
        r.m12 = -a.m12;
        r.m21 = -a.m21;
        r.m22 = a.m11;
        return m2_scl(r, 1.0 / m2_det(r));
}

m2 m2_aff_rot(float a)
{
        m2 r;
        float s = sinf(a);
        float c = cosf(a);
        r.m11 = c;
        r.m12 = -s;
        r.m21 = s;
        r.m22 = c;
        return r;
}

m2 m2_aff_scl(float x, float y)
{
        m2 r;
        r.m11 = x;
        r.m22 = y;
        r.m12 = r.m21 = 0;
        return r;
}

m2 m2_aff_shr(float x, float y)
{
        m2 r;
        r.m11 = r.m22 = 1;
        r.m21 = y;
        r.m12 = x;
        return r;
}

float m2_det(m2 a)
{
        return a.m11 * a.m22 - a.m21 * a.m12;
}