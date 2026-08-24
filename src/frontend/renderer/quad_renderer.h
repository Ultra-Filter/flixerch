#pragma once 

#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
#include "color.h"
#include "render_system.h"

#define GRADIENT_TYPE_NONE                      0.0F
#define GRADIENT_TYPE_LINEAR_HORIZONTAL         1.0F
#define GRADIENT_TYPE_LINEAR_VERTICAL           2.0F
#define GRADIENT_TYPE_LINEAR_IDENTITY           3.0F
#define GRADIENT_TYPE_LINEAR_IDENTITY_REVERSE   4.0F
#define GRADIENT_TYPE_LINEAR_CIRCULAR           5.0F
#define GRADIENT_TYPE_LINEAR_HYPERBOLA          6.0F
#define GRADIENT_TYPE_LINEAR_HYPERBOLA_REVERSE  7.0F

typedef struct quad_style_s {
    // Gradient 
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
} quad_style_s;

#define QUAD_STYLE_EMPTY() (quad_style_s){ 0 }

#define QUAD_STYLE_REGULAR(background_color)          \
    (quad_style_s){                                   \
        .background_color_left = background_color,    \
        .background_color_middle = background_color,  \
        .background_color_right = background_color,   \
        .gradient_type          = GRADIENT_TYPE_NONE, \
        .border_color = COLOR_TRANSPARENT,            \
        .border_radii = vec4v(0.0F),                  \
        .border_thickness = 0.0F,                     \
        .shadow_color = COLOR_TRANSPARENT,            \
        .shadow_offset = vec2v(0.0F),                 \
        .shadow_blur = 0.0F,                          \
        .shadow_spread = 0.0F,                        \
    }

#define QUAD_STYLE_GRADIENT(background_color_l, background_color_m, background_color_r, grad_type) \
    (quad_style_s){                                                                                \
        .background_color_left = background_color_l,                                               \
        .background_color_middle = background_color_m,                                             \
        .background_color_right = background_color_r,                                              \
        .gradient_type = grad_type,                                                                \
        .border_color = COLOR_TRANSPARENT,                                                         \
        .border_radii = vec4v(0.0F),                                                               \
        .border_thickness = 0.0F,                                                                  \
        .shadow_color = COLOR_TRANSPARENT,                                                         \
        .shadow_offset = vec2v(0.0F),                                                              \
        .shadow_blur = 0.0F,                                                                       \
        .shadow_spread = 0.0F,                                                                     \
    }

#define QUAD_STYLE_WITH_BORDERS(background_color, border_col, border_thick)  \
    (quad_style_s){                                                          \
        .background_color_left = background_color,                           \
        .background_color_middle = background_color,                         \
        .background_color_right = background_color,                          \
        .gradient_type = GRADIENT_TYPE_NONE,                                 \
        .border_color = border_col,                                          \
        .border_radii = vec4v(0.0F),                                         \
        .border_thickness = border_thick,                                    \
        .shadow_color = COLOR_TRANSPARENT,                                   \
        .shadow_offset = vec2v(0.0F),                                        \
        .shadow_blur = 0.0F,                                                 \
        .shadow_spread = 0.0F,                                               \
}

#define QUAD_STYLE_ROUND(background_color, border_radius) \
    (quad_style_s){                                       \
        .background_color_left = background_color,        \
        .background_color_middle = background_color,      \
        .background_color_right = background_color,       \
        .gradient_type = GRADIENT_TYPE_NONE,              \
        .border_color = COLOR_TRANSPARENT,                \
        .border_radii = border_radius,                    \
        .border_thickness = 0.0,                          \
        .shadow_color = COLOR_TRANSPARENT,                \
        .shadow_offset = vec2v(0.0F),                     \
        .shadow_blur = 0.0F,                              \
        .shadow_spread = 0.0F,                            \
}

#define QUAD_STYLE_ROUND_WITH_BORDER(background_color, border_col, border_thick, border_radius) \
    (quad_style_s){                                                                             \
        .background_color_left = background_color,                                              \
        .background_color_middle = background_color,                                            \
        .background_color_right = background_color,                                             \
        .gradient_type = GRADIENT_TYPE_NONE,                                                    \
        .border_color = border_col,                                                             \
        .border_radii = border_radius,                                                          \
        .border_thickness = border_thick,                                                       \
        .shadow_color = COLOR_TRANSPARENT,                                                      \
        .shadow_offset = vec2v(0.0F),                                                           \
        .shadow_blur = 0.0F,                                                                    \
        .shadow_spread = 0.0F,                                                                  \
}

#define QUAD_STYLE_GRADIENT_ROUND_WITH_BORDER(bg_col1,bg_col2,bg_col3, grad_type, border_col, border_thick, border_radius) \
    (quad_style_s){                                                                                                        \
        .background_color_left = bg_col1,                                                                                  \
        .background_color_middle = bg_col2,                                                                                \
        .background_color_right = bg_col3,                                                                                 \
        .gradient_type = grad_type,                                                                                        \
        .border_color = border_col,                                                                                        \
        .border_radii = border_radius,                                                                                     \
        .border_thickness = border_thick,                                                                                  \
        .shadow_color = COLOR_TRANSPARENT,                                                                                 \
        .shadow_offset = vec2v(0.0F),                                                                                      \
        .shadow_blur = 0.0F,                                                                                               \
        .shadow_spread = 0.0F,                                                                                             \
}

#define QUAD_STYLE_GRADIENT_ROUND(bg_col1,bg_col2,bg_col3, grad_type, border_radius) \
    (quad_style_s){                                                                  \
        .background_color_left = bg_col1,                                            \
        .background_color_middle = bg_col2,                                          \
        .background_color_right = bg_col3,                                           \
        .gradient_type = grad_type,                                                  \
        .border_color = COLOR_TRANSPARENT,                                           \
        .border_radii = border_radius,                                               \
        .border_thickness = 0.0F,                                                    \
        .shadow_color = COLOR_TRANSPARENT,                                           \
        .shadow_offset = vec2v(0.0F),                                                \
        .shadow_blur = 0.0F,                                                         \
        .shadow_spread = 0.0F,                                                       \
}

#define QUAD_STYLE_GRADIENT_WITH_BORDER(bg_col1,bg_col2,bg_col3, grad_type, border_col, border_thick) \
    (quad_style_s){                                                                                   \
        .background_color_left = bg_col1,                                                             \
        .background_color_middle = bg_col2,                                                           \
        .background_color_right = bg_col3,                                                            \
        .gradient_type = grad_type,                                                                   \
        .border_color = border_col,                                                                   \
        .border_radii = vec4v(0.0F),                                                                  \
        .border_thickness = border_thick,                                                             \
        .shadow_color = COLOR_TRANSPARENT,                                                            \
        .shadow_offset = vec2v(0.0F),                                                                 \
        .shadow_blur = 0.0F,                                                                          \
        .shadow_spread = 0.0F,                                                                        \
}

#define QUAD_STYLE_DROP(bg_col, sh_col, sh_off, sh_blur, sh_spread) \
    (quad_style_s){                                                 \
        .background_color_left = bg_col,                            \
        .background_color_middle = bg_col,                          \
        .background_color_right = bg_col,                           \
        .gradient_type = GRADIENT_TYPE_NONE,                        \
        .border_color = COLOR_TRANSPARENT,                          \
        .border_radii = vec4v(0.0F),                                \
        .border_thickness = 0.0F,                                   \
        .shadow_color = sh_col,                                     \
        .shadow_offset = sh_off,                                    \
        .shadow_blur = sh_blur,                                     \
        .shadow_spread = sh_spread,                                 \
}

#define QUAD_STYLE_DROP_WITH_BORDER(bg_col, border_col, border_thick, sh_col, sh_off, sh_blur, sh_spread) \
    (quad_style_s){                                                                                       \
        .background_color_left = bg_col,                                                                  \
        .background_color_middle = bg_col,                                                                \
        .background_color_right = bg_col,                                                                 \
        .gradient_type = GRADIENT_TYPE_NONE,                                                              \
        .border_color = border_col,                                                                       \
        .border_radii = vec4v(0.0F),                                                                      \
        .border_thickness = border_thick,                                                                 \
        .shadow_color = sh_col,                                                                           \
        .shadow_offset = sh_off,                                                                          \
        .shadow_blur = sh_blur,                                                                           \
        .shadow_spread = sh_spread,                                                                       \
}

#define QUAD_STYLE_DROP_ROUNDED(bg_col, border_rad, sh_col, sh_off, sh_blur, sh_spread)                                              \
    (quad_style_s){                                                                                                                  \
        .background_color_left = bg_col,                                                                                             \
        .background_color_middle = bg_col,                                                                                           \
        .background_color_right = bg_col,                                                                                            \
        .gradient_type = GRADIENT_TYPE_NONE,                                                                                         \
        .border_color = COLOR_TRANSPARENT,                                                                                           \
        .border_radii = border_rad,                                                                                                  \
        .border_thickness = 0.0F,                                                                                                    \
        .shadow_color = sh_col,                                                                                                      \
        .shadow_offset = sh_off,                                                                                                     \
        .shadow_blur = sh_blur,                                                                                                      \
        .shadow_spread = sh_spread,                                                                                                  \
}

#define QUAD_STYLE_DROP_ROUNDED_WITH_BORDER(bg_col, border_col, border_thick, border_rad, sh_col, sh_off, sh_blur, sh_spread)        \
    (quad_style_s){                                                                                                                  \
        .background_color_left = bg_col,                                                                                             \
        .background_color_middle = bg_col,                                                                                           \
        .background_color_right = bg_col,                                                                                            \
        .gradient_type = GRADIENT_TYPE_NONE,                                                                                         \
        .border_color = border_col,                                                                                                  \
        .border_radii = border_rad,                                                                                                  \
        .border_thickness = border_thick,                                                                                            \
        .shadow_color = sh_col,                                                                                                      \
        .shadow_offset = sh_off,                                                                                                     \
        .shadow_blur = sh_blur,                                                                                                      \
        .shadow_spread = sh_spread,                                                                                                  \
}

#define QUAD_STYLE_GRADIENT_DROP(bg1, bg2, bg3, grad_type, sh_col, sh_off, sh_blur, sh_spread)                                       \
    (quad_style_s){                                                                                                                  \
        .background_color_left = bg1,                                                                                                \
        .background_color_middle = bg2,                                                                                              \
        .background_color_right = bg3,                                                                                               \
        .gradient_type = grad_type,                                                                                                  \
        .border_color = COLOR_TRANSPARENT,                                                                                           \
        .border_radii = vec4v(0.0F),                                                                                                 \
        .border_thickness = 0.0F,                                                                                                    \
        .shadow_color = sh_col,                                                                                                      \
        .shadow_offset = sh_off,                                                                                                     \
        .shadow_blur = sh_blur,                                                                                                      \
        .shadow_spread = sh_spread,                                                                                                  \
}

#define QUAD_STYLE_GRADIENT_DROP_ROUNDED(bg1, bg2, bg3, grad_type, border_rad, sh_col, sh_off, sh_blur, sh_spread)                   \
    (quad_style_s){                                                                                                                  \
        .background_color_left = bg1,                                                                                                \
        .background_color_middle = bg2,                                                                                              \
        .background_color_right = bg3,                                                                                               \
        .gradient_type = grad_type,                                                                                                  \
        .border_color = COLOR_TRANSPARENT,                                                                                           \
        .border_radii = border_rad,                                                                                                  \
        .border_thickness = 0.0F,                                                                                                    \
        .shadow_color = sh_col,                                                                                                      \
        .shadow_offset = sh_off,                                                                                                     \
        .shadow_blur = sh_blur,                                                                                                      \
        .shadow_spread = sh_spread,                                                                                                  \
}

#define QUAD_STYLE_GRADIENT_DROP_WITH_BORDER(bg1, bg2, bg3, grad_type, border_col, border_thick, sh_col, sh_off, sh_blur, sh_spread) \
    (quad_style_s){                                                                                                                  \
        .background_color_left = bg1,                                                                                                \
        .background_color_middle = bg2,                                                                                              \
        .background_color_right = bg3,                                                                                               \
        .gradient_type = grad_type,                                                                                                  \
        .border_color = border_col,                                                                                                  \
        .border_radii = vec4v(0.0),                                                                                                  \
        .border_thickness = border_thick,                                                                                            \
        .shadow_color = sh_col,                                                                                                      \
        .shadow_offset = sh_off,                                                                                                     \
        .shadow_blur = sh_blur,                                                                                                      \
        .shadow_spread = sh_spread,                                                                                                  \
}

#define QUAD_STYLE_GRADIENT_DROP_ROUNDED_WITH_BORDER(bg1, bg2, bg3, grad_type, border_rad, border_col, border_thick, sh_col, sh_off, sh_blur, sh_spread) \
    (quad_style_s){                                                                                                                  \
        .background_color_left = bg1,                                                                                                \
        .background_color_middle = bg2,                                                                                              \
        .background_color_right = bg3,                                                                                               \
        .gradient_type = grad_type,                                                                                                  \
        .border_color = border_col,                                                                                                  \
        .border_radii = border_rad,                                                                                                  \
        .border_thickness = border_thick,                                                                                            \
        .shadow_color = sh_col,                                                                                                      \
        .shadow_offset = sh_off,                                                                                                     \
        .shadow_blur = sh_blur,                                                                                                      \
        .shadow_spread = sh_spread,                                                                                                  \
}

void quad_renderer_init(void);
void quad_renderer_shutdown(void);
void quad_renderer_draw(transform2D_s transform, quad_style_s style, z_index_type_e depth);
void quad_renderer_push(void);
void quad_renderer_render(z_index_type_e depth);
void quad_renderer_submit(void); // legacy - keep for now
