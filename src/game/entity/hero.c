#include "entity.h"
#include "input.h"
#include "world.h"

typedef struct hero {
        entity base;
        char *name;
} hero;

#define HERO_GROUND_ACC 256

static void a_default(hero *this)
{
        bool grounded = en_grounded(super);
        if (grounded) {
                if (btn_just_pressed(BTN_FACE_DOWN)) {
                        super->vel_q12.y = -4000;
                }
        }

        super->vel_q12.y += 100;

        if (btn_pressed(BTN_LEFT))
                super->vel_q12.x -= HERO_GROUND_ACC;
        if (btn_pressed(BTN_RIGHT))
                super->vel_q12.x += HERO_GROUND_ACC;
}

static func functions[] = {a_default};

entity *en_hero()
{
        hero *this = (hero *)en_create_actor();
        if (!this)
                return NULL;
        super->drag_q8.x = 240;
        super->tag = ENTAG_HERO;
        super->jump_table = functions;
        super->x = 5;
        super->y = 5;
        super->w = 8;
        super->h = 15;

        return super;
}