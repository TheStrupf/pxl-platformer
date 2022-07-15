#include "entity.h"
#include "world.h"

typedef struct en {
        entity base;
} en;

static void a_default(en *this)
{
}

static func functions[] = {a_default};

entity *en_template()
{
        en *this = (en *)en_create_actor();
        super->jump_table = functions;
        return super;
}