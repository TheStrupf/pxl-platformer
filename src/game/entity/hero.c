#include "engine.h"
#include "entity.h"
#include "world.h"

const int HERO_GROUND_ACC = 700;
const int HERO_GRAVITY = 600;
const int HERO_GROUND_DRAG = 230;

const int EDGE_TICKS = 6;
const int JUMP_TICKS = 24;
const int JUMP_MAX = 800;
const int JUMP_MIN = 0;
const int JUMP_INIT = 7000;

typedef struct hero {
        entity base;
        char *name;
        uchar jumpticks;
        uchar edgeticks;
} hero;

enum hero_actions {
        HERO_DEFAULT,
};

static void a_default(hero *this)
{
        bool grounded = en_grounded(super);
        if (grounded) {
                this->edgeticks = EDGE_TICKS;

        } else if (this->edgeticks) {
                this->edgeticks--;
        }

        if (vmbtn_pressed(BTN_FACE_DOWN)) {
                if (vmbtn_just_pressed(BTN_FACE_DOWN) && this->edgeticks) {
                        super->vel_q12.y = -JUMP_INIT;
                        this->edgeticks = 0;
                        this->jumpticks = JUMP_TICKS;
                } else if (this->jumpticks) { // linear interpolation, recude jump boost
                        super->vel_q12.y -= JUMP_MIN + (JUMP_MAX - JUMP_MIN) * this->jumpticks-- / JUMP_TICKS;
                }

        } else {
                this->jumpticks = 0;
        }

        // super->vel_q12.y += HERO_GRAVITY;

        // super->vel_q12.x += vmbtn_xdir() * HERO_GROUND_ACC;
}

static func functions[] = {a_default};

entity *en_hero()
{
        hero *this = (hero *)en_create_actor();
        if (!this)
                return NULL;
        super->drag_q8.x = HERO_GROUND_DRAG;
        super->tag = ENTAG_HERO;
        super->jump_table = functions;
        super->x = 5;
        super->y = 5;
        super->w = 8;
        super->h = 15;

        return super;
}