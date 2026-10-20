#pragma once
#include "ui_interface.h"
#include "frontend/renderer/color.h"



#define UI_GROW(...) __extension__ ({                                  \
    _Pragma("GCC diagnostic push")                                   \
    _Pragma("GCC diagnostic ignored \"-Woverride-init\"")            \
    sizing ___s___ = { .type = SIZING_GROW, .min = 0.0F, .max = 10000.0F, __VA_ARGS__ };                         \
    _Pragma("GCC diagnostic pop")                                    \
    ___s___;                                                              \
})
#define UI_FIT(...) __extension__ ({                                  \
    _Pragma("GCC diagnostic push")                                   \
    _Pragma("GCC diagnostic ignored \"-Woverride-init\"")            \
    sizing ___s___ = { .type = SIZING_FIT, .min = 0.0F, .max = 10000.0F, __VA_ARGS__ };                         \
    _Pragma("GCC diagnostic pop")                                    \
    ___s___;                                                              \
})
#define UI_FIXED(val) (sizing){ .type = SIZING_FIXED, .value = val, .min = val, .max = val }

// #define UI_FIT(...) (sizing){ .type = SIZING_FIT, .min = 0.0F, .max = 10000.0F, __VA_ARGS__ }
// #define UI_GROW(...) (sizing){ .type = SIZING_GROW, .min = 0.0F, .max = 10000.0F, __VA_ARGS__ }
// #define UI_FIXED(val) (sizing){ .type = SIZING_FIXED, .value = val, .min = 0.0F, .max = 10000.0F }

#define UI_SELF(...) (struct self_data){ __VA_ARGS__ }
#define UI_CHILDS(...) (struct childs_data){ __VA_ARGS__ }
#define UI_STYLE(...) (struct style_data){ __VA_ARGS__ }
#define UI_CREATE_DATA(...) (ui_create_data){ __VA_ARGS__ }
#define UI_PADDING(...) (padding_vec){ __VA_ARGS__ }
#define UI_PADDING_V(v) (padding_vec){ v, v, v, v }



#define CREATE_DATA_DEFAULT                         \
    UI_CREATE_DATA(                                 \
        .childs = UI_CHILDS(                        \
            .direction = HORIZONTAL,                \
            .gap_cross = 10,                        \
            .gap_main = 10,                         \
            .paddings = UI_PADDING_V(10.0F)         \
        ),                                          \
        .self = UI_SELF(                            \
            .aspect_ratio = 1.0F,                   \
            .height = UI_FIT(),                     \
            .width = UI_FIT(),                      \
            .visible = true,                        \
            .z_index = Z_INDEX_0                    \
        ),                                          \
        .style = UI_STYLE(                          \
            .background_color_left = COLOR_WHITE,   \
            .background_color_middle = COLOR_WHITE, \
            .background_color_right = COLOR_WHITE,  \
            .border_color = COLOR_WHITE,            \
            .shadow_color = COLOR_BLACK,            \
        ),                                          \
    )
