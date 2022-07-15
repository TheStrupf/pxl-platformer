#include "entity.h"
#include "input.h"
#include "world.h"

typedef struct hero {
        entity base;
        char *name;
} hero;

static void a_default(hero *this)
{
        if (btn_pressed(BTN_LEFT))
                en_move_actor(super, -1, 0);
        if (btn_pressed(BTN_RIGHT))
                en_move_actor(super, 1, 0);
        if (btn_pressed(BTN_UP))
                en_move_actor(super, 0, -1);
        if (btn_pressed(BTN_DOWN))
                en_move_actor(super, 0, 1);
}

static func functions[] = {a_default};

entity *en_hero()
{
        hero *this = (hero *)en_create_actor();
        super->jump_table = functions;
        super->x = 5;
        super->y = 5;
        super->w = 8;
        super->h = 15;
        return super;
}