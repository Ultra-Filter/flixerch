#pragma once

#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
#include "color.h"
#include "render_system.h"

typedef struct image_style_s {
    // Gradient 
    rgba_t tint_color_left;
    rgba_t tint_color_middle;
    rgba_t tint_color_right;
    f32 gradient_type;
    f32 tint_strength;
    
    rgba_t border_color;
    vec4_t border_radii;
    f32 border_thickness;
    
    rgba_t shadow_color;
    vec2_t shadow_offset;
    f32 shadow_blur, shadow_spread;
} image_style_s;

// TODO:
// #define IMAGE_STYLE_REGULAR() (image_style_s){ 0 }
// #define IMAGE_STYLE_ROUNDED(border_radius) (image_style_s){ .border_radius = border_radius }
// #define IMAGE_STYLE_BORDER(border_thick, border_color) (image_style_s){ .border_thickness = border_thick, .border_color = border_color }
// ...


void image_renderer_init(void);
void image_renderer_shutdown(void);
void image_renderer_draw(u32 tex_id, transform2D_s transform, image_style_s style, z_index_type_e depth);
void image_renderer_push(void);
void image_renderer_render(z_index_type_e depth);
void image_renderer_submit(void);