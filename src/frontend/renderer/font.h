#pragma once

#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
#include "color.h"
 

typedef enum font_type_e 
{
    FONT_TYPE_ROBOTO32,
    FONT_TYPE_COUNT
} font_type_e;

typedef struct font_char_t 
{
    s32 codepoint, x, y, w, h, origin_x, origin_y;
} font_char_t;

typedef struct font_t 
{
    const char * font_name;
    s32 size, bold, italic, w, h, char_count;
    font_char_t* chars;
    const char* image_path;
    const font_type_e font_type; 
} font_t;

font_char_t font_get_codepoint_data(font_type_e, s32 codepoint);
rectangle_t font_get_rect_from_codepoint(font_type_e font_type, s32 codepoint);
f32 font_get_width(font_type_e font_type);
f32 font_get_height(font_type_e font_type);
