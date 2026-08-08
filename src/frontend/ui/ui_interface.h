#pragma once
#include "../../core/utils/defines.h"

// ==============================
//       Should NOT be here
// ==============================
// TODO: Move it to correct location

typedef struct Vec2  { float x, y;    } Vec2;
typedef struct Color { float r,g,b,a; } Color;

// ----------------------------------
//          Componenets
// ----------------------------------

typedef struct transform_c
{
    Vec2 top_left;
    Vec2 scale;
    f32 rotation;
} transform_c;

typedef enum align_childs_e 
{ 
    ALIGN_CHILDS_CENTER, 
    ALIGN_CHILDS_START, 
    ALIGN_CHILDS_END
} align_childs_e;

typedef struct style_c 
{
    // ------- General ----------
    Color background_color; 
    Vec2 padding_vertical;  
    Vec2 padding_horizontal;
    Vec2 margin_vertical;
    Vec2 margin_horizontal;
    f32 width;      // fraction 
    f32 height;     // fraction 
    f32 min_width;  // in pixels
    f32 min_height; // in pixels
    f32 max_width;  // in pixels
    f32 max_height; // in pixels
    s32 z_index; 

    // ------- Borders ----------
    Color border_color;
    f32 border_radius;
    f32 border_thickness;
    
    // ------- Childs ----------
    f32 childs_gap;
    align_childs_e align_childs_vertical;
    align_childs_e align_childs_horizontal;
} style_c;

// ----------------------------------
//          Elements
// ----------------------------------

typedef struct UI_element
{
    u32 ID;
    transform_c transform;
    style_c style;
    struct UI_element* father;
    struct UI_element** childs;
    u32 childs_count;
} UI_element;

typedef struct Text_element 
{
    UI_element ui;
    char * text;
    Color text_color;
    f32 font_size;
} Text_element;