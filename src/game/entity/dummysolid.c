#include "engine.h"
#include "entity.h"
#include "world.h"

static void solid_default(entity *this)
{
        en_move(this, vmbtn_xdir(), vmbtn_ydir());
}

static func functions[] = {solid_default};

entity *en_dummysolid()
{
        entity *this = (entity *)en_create_solid(20, 20);
        if (!this)
                return NULL;
        for (int n = 0; n < 20 * 20; n++)
                this->px[n] = PX_SOLID;
        this->jump_table = functions;
        return super;
}