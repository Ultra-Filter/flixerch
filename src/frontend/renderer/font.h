#pragma once

#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
 
typedef enum font_type_e 
{
    FONT_TYPE_ROBOTO32,
    FONT_TYPE_TIMES_20,
    FONT_TYPE_TIMES_40,
    FONT_TYPE_TIMES_60,
    FONT_TYPE_TIMES_80,
    FONT_TYPE_TIMES_100,
    FONT_TYPE_COUNT
} font_type_e;

typedef struct font_char_t 
{
    s32 codepoint, x, y, w, h, origin_x, origin_y;
} font_char_t;
typedef enum font_atlas_type_e {
    FONT_ATLAS_TYPE_TIMES_100_80_60_40_20,
    FONT_ATLAS_TYPE_COUNT
} font_atlas_type_e;

font_char_t font_atlas_get_codepoint_data(font_atlas_type_e atlas_type, u8 font_index, s32 codepoint);
u64         font_atlas_get_offset        (font_atlas_type_e atlas_type, u8 font_index);
u64         font_atlas_get_width         (font_atlas_type_e atlas_type);
u64         font_atlas_get_height        (font_atlas_type_e atlas_type);

font_char_t font_get_codepoint_data(font_type_e, s32 codepoint);
rectangle_t font_get_rect_from_codepoint(font_type_e font_type, s32 codepoint);
f32 font_get_width(font_type_e font_type);
f32 font_get_height(font_type_e font_type);
