#pragma once 

#include "core/utils/defines.h"
#include "core/utils/math_engine.h"

typedef enum drawable_type_e
{
    DRAWABLE_TYPE_IMAGE,
    DRAWABLE_TYPE_QUAD,
    DRAWABLE_TYPE_TEXT,
    DRAWABLE_TYPE_COUNT,
} drawable_type_e;

typedef enum z_index_type_e
{
    Z_INDEX_0 = 0,
    Z_INDEX_1 = 1,
    Z_INDEX_2 = 2,
    Z_INDEX_3 = 3,
    Z_INDEX_4 = 4,
    Z_INDEX_COUNT = 5,
} z_index_type_e;

void render_system_init(void);
void render_system_shutdown(void);
void render_system_render(void);

void render_system_draw(drawable_type_e draw_type, transform2D_s transform, z_index_type_e z_index, ...);