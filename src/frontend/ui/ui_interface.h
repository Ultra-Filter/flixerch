#pragma once
#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
#include "frontend/renderer/color.h"
#include "frontend/renderer/quad_renderer.h"

typedef enum ui_element_type : u8 
{
    UI_ELEMENT_TYPE_PANEL
} ui_element_type;

typedef struct sizing {
    enum sizing_type : u8 {
        SIZING_FIT, SIZING_GROW, SIZING_FIXED
    } type;
    f32 value;
    f32 min, max;
} sizing;   

typedef struct padding_vec {
    f32 top, bottom, left, right;
} padding_vec;

typedef struct ui_create_data {
    struct self_data {
        bool visible; // ?
        z_index_type_e z_index;
        sizing width, height;
        f32 aspect_ratio;
    } self;
    struct childs_data {
        enum direction : u8 {
            HORIZONTAL, VERTICAL
        } direction;
        enum alignment : u8 {
            CENTER, START, END, SPACE_BETWEEN, SPACE_AROUND
        } align_main, align_cross;
        enum flex : u8 {
            NONE, GRID, ROW, COL
        } flex;
        f32 gap_main, gap_cross;
        padding_vec paddings;
    } childs;
    struct style_data {
        rgba_t background_color_left;
        rgba_t background_color_middle;
        rgba_t background_color_right;
        f32 gradient_type;
        rgba_t border_color;
        vec4_t border_radii;
        f32 border_thickness;
        rgba_t shadow_color;
        vec2_t shadow_offset;
        f32 shadow_blur, shadow_spread;
    } style;
} ui_create_data;

typedef char* dstr_t;

typedef struct ui_element {
    dstr_t name; // Do we need one?
    ui_element_type type;
    ui_create_data create_data;
    transform2D_s transform;
    u32 childs_count;
    f32 sum_sizes;
    f32 max_element_size;

    struct ui_element * first_child;
    struct ui_element * right_sibling;
    struct ui_element * parent;
} ui_element;