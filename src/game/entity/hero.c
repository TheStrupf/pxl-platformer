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
                en_move(super, -1, 0);
        if (btn_pressed(BTN_RIGHT))
                en_move(super, 1, 0);
        if (btn_pressed(BTN_UP))
                en_move(super, 0, -1);
        if (btn_pressed(BTN_DOWN))
                en_move(super, 0, 1);
}

static func functions[] = {a_default};

entity *en_hero()
{
        hero *this = (hero *)en_create_actor();
        if (!this)
                return NULL;
        super->tag = ENTAG_HERO;
        super->jump_table = functions;
        super->r.x = 5;
        super->r.y = 5;
        super->r.w = 8;
        super->r.h = 15;
        return super;
}