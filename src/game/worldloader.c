#include "engine.h"
#include "entity.h"
#include "jsonp.h"
#include "world.h"
#include <stdbool.h>
#include <string.h>

static void load_tilelayer(jsn *j, int layer)
{
        tile *tile = &world->tiles[layer];
        jsn *tilearr = json_get(j, "data");
        for (jsn *t = tilearr->first_child; t; t = t->next_sibling) {
                uint tileID = json_i(t);

                // TODO: Tiled bitflags for transformations
                // doc.mapeditor.org/en/latest/reference/global-tile-ids/

                bool flipped_horizontally = (tileID & 0x80000000);
                bool flipped_vertically = (tileID & 0x40000000);
                bool flipped_diagonally = (tileID & 0x20000000);
                tileID = (tileID & 0xFFFFFFF) - 1;
                tile->ID = tileID;
        }
}

static void load_objects(jsn *j)
{
}

void world_load_map(const char *filename)
{
        char *text = read_txt(filename);
        en_reset_raw();
        world->entities->n = 0;
        world->solids->n = 0;
        world->actors->n = 0;

        const uint TOKS = 1 << 22;
        jsn *toks = vmalloc(sizeof(jsn) * TOKS);
        json_parse(text, toks, TOKS);
        jsn *root = &toks[0];
        world->w = json_ik(root, "width");
        world->h = json_ik(root, "height");
        world->pw = world->w * 8;
        world->ph = world->h * 8;

        jsn *layerarr = json_get(root, "layers");
        jsn *l = layerarr->first_child;
        for (jsn *l = layerarr->first_child; l; l = l->next_sibling) {
                char namebuffer[32];
                json_ck(l, "name", namebuffer, 32);
                if (STREQ(namebuffer, "MAIN"))
                        load_tilelayer(l, 0);
                else if (STREQ(namebuffer, "MAIN"))
                        load_tilelayer(l, 0);
                else if (STREQ(namebuffer, "FG"))
                        load_tilelayer(l, 0);
                else if (STREQ(namebuffer, "BG"))
                        load_tilelayer(l, 0);
                else
                        load_objects(l);
        }

        vmfree(toks);
        vmfree(text);
}