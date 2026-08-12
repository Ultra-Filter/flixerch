#pragma once

#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
#include "color.h"

// typedef struct ui_renderer_t ui_renderer_t;
//
// typedef struct ui_render_data {
//     ui_renderer_t *renderer;
//     rectangle_t rect, texture_rectangle;
//     float border_radius, border_thickness;
//     rgba_t background_color, border_color, shadow_color;
//     u32 texture_index; // TODO:
// } ui_render_data_t;
//
// ui_renderer_t* ui_renderer_create(u32 shader_program_id, u32 texture_id, u64 quads_max_count);
//
// void ui_renderer_destroy(ui_renderer_t* renderer);
//
// void ui_renderer_draw(ui_renderer_t* renderer);
//
// void ui_render_panel(ui_render_data_t render_data);
//

// -----------------------------------------------

void ui_init(void);
void ui_shutdown(void);
void ui_exec_draw_call(void);

void ui_panel(rectangle_t rect, rgba_t bg_color);
void ui_panel_with_borders(rectangle_t rect, rgba_t bg_color, rgba_t border_color, f32 border_thick);
void ui_panel_round(rectangle_t rect, rgba_t bg_color, f32 border_radius);
void ui_panel_round_with_borders(rectangle_t rect, rgba_t bg_color, rgba_t border_color, f32 border_thick, f32 border_radius);
void ui_text(s32* codepoints, u32 length, vec2_t top_left, rgba_t text_color);

