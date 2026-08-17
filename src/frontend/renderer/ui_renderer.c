#include "ui_renderer.h"
#include "frontend/renderer/color.h"
#include "quad_batch.h"
#include "core/utils/file_system.h"
#include "core/decoders/stb_image.h"
#include "glad.h"
#include "font.h"

typedef struct ui_panel_vtx_t
{
    f32 x, y;                                    // vertex_position           - vec2  
    f32 border_radius, border_thickness;         // border radius & thickness - vec2 
    f32 bg_r, bg_g, bg_b, bg_a;                  // background color          - vec4 
    f32 border_r, border_g, border_b, border_a;  // border color              - vec4 
    f32 u, v;                                    // texture coordinates       - vec2 
    f32 panel_scale_x, panel_scale_y;            // panel scale               - vec2 
    f32 shadow_r, shadow_g, shadow_b, shadow_a;  // shadow color              - vec4 
    f32 frag_coords_x, frag_coords_y;            // frag coords               - vec2
    u32 texture_index;                           // texture index             - uint 
} ui_panel_vtx_t;

typedef struct ui_text_vtx_t 
{
    f32 x, y;                           // vertex_position           - vec2 
    f32 text_r, text_g, text_b, text_a; // text color                - vec4
    f32 u, v;                           // texture coordinates       - vec2
    u32 texture_index;                  // texture index             - uint
} ui_text_vtx_t;

// -------------
//   Helpers
// -------------

static u32 create_shader_program(const char* vs_path, const char* fs_path);
static u32 create_texture(const char* image_path);

static struct ui_global_data 
{
    quad_batch_t* text_batch;
    quad_batch_t* panel_batch;
    font_type_e default_font_type;
} ui_context;

// ------------------------ Panel Configs ------------------------------------------------------
static const u8 panel_vertex_attribute_count = 9U;
static const u8 panel_vertex_attributes_counts [ 9 ] = { 2U, 2U, 4U, 4U, 2U, 2U, 4U, 2U, 1U };
static const attributes_type_e panel_attributes_types [ 9 ] = 
    {
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_UNSIGNED_INT
    };
static const u64 panel_quads_max_count = 100ULL;


// ------------------------ Panel Configs ------------------------------------------------------
static const u64 text_quads_max_count = 1000ULL;
static const u8 text_vertex_attribute_count = 4U;
static const u8 text_vertex_attributes_counts [ 4 ] = { 2U, 4U, 2U, 1U };
static const attributes_type_e text_vertex_attributes_types [ 4 ] = 
    {
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_UNSIGNED_INT
    };
  
// -----------------------------------------------------------------------------------------------

void ui_init(void)
{
    // ------------------ UI Panel batch creation -----------------------------------
    u32 panel_shader, panel_texture;
    panel_shader  = create_shader_program("assets/shaders/ui_panel.vs", "assets/shaders/ui_panel.fs");
    panel_texture = create_texture("assets/images/ui_panel_texture.jpg");

    quad_batch_create_info_s panel_batch_create_info = { 0 };
    panel_batch_create_info.quads_max_count = panel_quads_max_count;
    panel_batch_create_info.shader_program  = panel_shader;
    panel_batch_create_info.texture_id      = panel_texture;
    panel_batch_create_info.vertex_attributes.attributes_count = panel_vertex_attributes_counts;
    panel_batch_create_info.vertex_attributes.count            = panel_vertex_attribute_count;
    panel_batch_create_info.vertex_attributes.attributes_type  = panel_attributes_types;

    ui_context.panel_batch = quad_batch_create(panel_batch_create_info);

    // ------------------ UI Text batch creation -----------------------------------
    u32 text_shader, text_texture;
    text_shader = create_shader_program("assets/shaders/text_shader.vs", "assets/shaders/text_shader.fs");
    text_texture = create_texture("assets/images/text_texture.png");

    quad_batch_create_info_s text_batch_create_info = { 0 };
    text_batch_create_info.shader_program  = text_shader;
    text_batch_create_info.texture_id      = text_texture;
    text_batch_create_info.quads_max_count = text_quads_max_count;
    text_batch_create_info.vertex_attributes.count = text_vertex_attribute_count;
    text_batch_create_info.vertex_attributes.attributes_count = text_vertex_attributes_counts;
    text_batch_create_info.vertex_attributes.attributes_type  = text_vertex_attributes_types;

    ui_context.text_batch = quad_batch_create(text_batch_create_info);

    ui_context.default_font_type = FONT_TYPE_ROBOTO32;
}

void ui_shutdown(void)
{
    quad_batch_destroy(ui_context.panel_batch);
    quad_batch_destroy(ui_context.text_batch);
}

void ui_exec_draw_call(void)
{
    quad_batch_submit(ui_context.panel_batch);
    quad_batch_submit(ui_context.text_batch);
}

void ui_panel(rectangle_t rect, rgba_t bg_color)
{
    transform2D_s transform = { 0 };

    transform.position = vec2(rect.x, rect.y);
    transform.rotation = rect.r;
    transform.scale    = vec2(rect.w, rect.h);

    struct {
        f32 border_radius, border_thickness;
        f32 bg_r, bg_g, bg_b, bg_a;
        f32 border_r, border_g, border_b, border_a;
        f32 u, v;
        f32 panel_scale_x, panel_scale_y;
        f32 shadow_r, shadow_g, shadow_b, shadow_a;
        f32 frag_coords_x, frag_coords_y;
        u32 texture_index;
    } vd1, vd2, vd3, vd4;

    char vert_data [ 4U * sizeof(vd1) ];

    rgbaf_t bg = rgbaf_from_rgba(bg_color);

    vd1.border_radius    = 0.0F;
    vd1.border_thickness = 0.0F;
    vd1.bg_r             = bg.r;
    vd1.bg_g             = bg.g;
    vd1.bg_b             = bg.b;
    vd1.bg_a             = bg.a;
    vd1.border_r         = 0.0F;
    vd1.border_g         = 0.0F;
    vd1.border_b         = 0.0F;
    vd1.border_a         = 0.0F;
    vd1.u                = 1.0F;
    vd1.v                = 1.0F;
    vd1.panel_scale_x    = rect.w;
    vd1.panel_scale_y    = rect.h;
    vd1.shadow_r         = 0.0F;
    vd1.shadow_g         = 0.0F;
    vd1.shadow_b         = 0.0F;
    vd1.shadow_a         = 0.0F;
    vd1.frag_coords_x    = 1.0F;
    vd1.frag_coords_y    = 1.0F;
    vd1.texture_index    = 0U;

    vd2.border_radius    = 0.0F;
    vd2.border_thickness = 0.0F;
    vd2.bg_r             = bg.r;
    vd2.bg_g             = bg.g;
    vd2.bg_b             = bg.b;
    vd2.bg_a             = bg.a;
    vd2.border_r         = 0.0F;
    vd2.border_g         = 0.0F;
    vd2.border_b         = 0.0F;
    vd2.border_a         = 0.0F;
    vd2.u                = 1.0F;
    vd2.v                = 0.0F;
    vd2.panel_scale_x    = rect.w;
    vd2.panel_scale_y    = rect.h;
    vd2.shadow_r         = 0.0F;
    vd2.shadow_g         = 0.0F;
    vd2.shadow_b         = 0.0F;
    vd2.shadow_a         = 0.0F;
    vd2.frag_coords_x    = 1.0F;
    vd2.frag_coords_y    = 0.0F;
    vd2.texture_index    = 0U;

    vd3.border_radius    = 0.0F;
    vd3.border_thickness = 0.0F;
    vd3.bg_r             = bg.r;
    vd3.bg_g             = bg.g;
    vd3.bg_b             = bg.b;
    vd3.bg_a             = bg.a;
    vd3.border_r         = 0.0F;
    vd3.border_g         = 0.0F;
    vd3.border_b         = 0.0F;
    vd3.border_a         = 0.0F;
    vd3.u                = 0.0F;
    vd3.v                = 1.0F;
    vd3.panel_scale_x    = rect.w;
    vd3.panel_scale_y    = rect.h;
    vd3.shadow_r         = 0.0F;
    vd3.shadow_g         = 0.0F;
    vd3.shadow_b         = 0.0F;
    vd3.shadow_a         = 0.0F;
    vd3.frag_coords_x    = 0.0F;
    vd3.frag_coords_y    = 1.0F;
    vd3.texture_index    = 0U;

    vd4.border_radius    = 0.0F;
    vd4.border_thickness = 0.0F;
    vd4.bg_r             = bg.r;
    vd4.bg_g             = bg.g;
    vd4.bg_b             = bg.b;
    vd4.bg_a             = bg.a;
    vd4.border_r         = 0.0F;
    vd4.border_g         = 0.0F;
    vd4.border_b         = 0.0F;
    vd4.border_a         = 0.0F;
    vd4.u                = 0.0F;
    vd4.v                = 0.0F;
    vd4.panel_scale_x    = rect.w;
    vd4.panel_scale_y    = rect.h;
    vd4.shadow_r         = 0.0F;
    vd4.shadow_g         = 0.0F;
    vd4.shadow_b         = 0.0F;
    vd4.shadow_a         = 0.0F;
    vd4.frag_coords_x    = 0.0F;
    vd4.frag_coords_y    = 0.0F;
    vd4.texture_index    = 0U;

    memcpy(vert_data,                                           &vd1, sizeof(vd1));
    memcpy(vert_data + sizeof(vd1),                             &vd2, sizeof(vd2));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2),               &vd3, sizeof(vd3));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2) + sizeof(vd3), &vd4, sizeof(vd4));

    quad_batch_append_quad(ui_context.panel_batch, transform, vert_data);
}

void ui_panel_with_borders(rectangle_t rect, rgba_t bg_color, rgba_t border_color, f32 border_thick)
{
    transform2D_s transform = { 0 };

    transform.position = vec2(rect.x, rect.y);
    transform.rotation = rect.r;
    transform.scale    = vec2(rect.w, rect.h);

    struct {
        f32 border_radius, border_thickness;
        f32 bg_r, bg_g, bg_b, bg_a;
        f32 border_r, border_g, border_b, border_a;
        f32 u, v;
        f32 panel_scale_x, panel_scale_y;
        f32 shadow_r, shadow_g, shadow_b, shadow_a;
        f32 frag_coords_x, frag_coords_y;
        u32 texture_index;
    } vd1, vd2, vd3, vd4;

    char vert_data [ 4U * sizeof(vd1) ];

    rgbaf_t bg = rgbaf_from_rgba(bg_color);

    vd1.border_radius    = 0.0F;
    vd1.border_thickness = border_thick;
    vd1.bg_r             = bg.r;
    vd1.bg_g             = bg.g;
    vd1.bg_b             = bg.b;
    vd1.bg_a             = bg.a;
    vd1.border_r         = border_color.r;
    vd1.border_g         = border_color.g;
    vd1.border_b         = border_color.b;
    vd1.border_a         = border_color.a;
    vd1.u                = 1.0F;
    vd1.v                = 1.0F;
    vd1.panel_scale_x    = rect.w;
    vd1.panel_scale_y    = rect.h;
    vd1.shadow_r         = 0.0F;
    vd1.shadow_g         = 0.0F;
    vd1.shadow_b         = 0.0F;
    vd1.shadow_a         = 0.0F;
    vd1.frag_coords_x    = 1.0F;
    vd1.frag_coords_y    = 1.0F;
    vd1.texture_index    = 0U;

    vd2.border_radius    = 0.0F;
    vd2.border_thickness = border_thick;
    vd2.bg_r             = bg.r;
    vd2.bg_g             = bg.g;
    vd2.bg_b             = bg.b;
    vd2.bg_a             = bg.a;
    vd2.border_r         = border_color.r;
    vd2.border_g         = border_color.g;
    vd2.border_b         = border_color.b;
    vd2.border_a         = border_color.a;
    vd2.u                = 1.0F;
    vd2.v                = 0.0F;
    vd2.panel_scale_x    = rect.w;
    vd2.panel_scale_y    = rect.h;
    vd2.shadow_r         = 0.0F;
    vd2.shadow_g         = 0.0F;
    vd2.shadow_b         = 0.0F;
    vd2.shadow_a         = 0.0F;
    vd2.frag_coords_x    = 1.0F;
    vd2.frag_coords_y    = 0.0F;
    vd2.texture_index    = 0U;

    vd3.border_radius    = 0.0F;
    vd3.border_thickness = border_thick;
    vd3.bg_r             = bg.r;
    vd3.bg_g             = bg.g;
    vd3.bg_b             = bg.b;
    vd3.bg_a             = bg.a;
    vd3.border_r         = border_color.r;
    vd3.border_g         = border_color.g;
    vd3.border_b         = border_color.b;
    vd3.border_a         = border_color.a;
    vd3.u                = 0.0F;
    vd3.v                = 1.0F;
    vd3.panel_scale_x    = rect.w;
    vd3.panel_scale_y    = rect.h;
    vd3.shadow_r         = 0.0F;
    vd3.shadow_g         = 0.0F;
    vd3.shadow_b         = 0.0F;
    vd3.shadow_a         = 0.0F;
    vd3.frag_coords_x    = 0.0F;
    vd3.frag_coords_y    = 1.0F;
    vd3.texture_index    = 0U;

    vd4.border_radius    = 0.0F;
    vd4.border_thickness = border_thick;
    vd4.bg_r             = bg.r;
    vd4.bg_g             = bg.g;
    vd4.bg_b             = bg.b;
    vd4.bg_a             = bg.a;
    vd4.border_r         = border_color.r;
    vd4.border_g         = border_color.g;
    vd4.border_b         = border_color.b;
    vd4.border_a         = border_color.a;
    vd4.u                = 0.0F;
    vd4.v                = 0.0F;
    vd4.panel_scale_x    = rect.w;
    vd4.panel_scale_y    = rect.h;
    vd4.shadow_r         = 0.0F;
    vd4.shadow_g         = 0.0F;
    vd4.shadow_b         = 0.0F;
    vd4.shadow_a         = 0.0F;
    vd4.frag_coords_x    = 0.0F;
    vd4.frag_coords_y    = 0.0F;
    vd4.texture_index    = 0U;

    memcpy(vert_data,                                           &vd1, sizeof(vd1));
    memcpy(vert_data + sizeof(vd1),                             &vd2, sizeof(vd2));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2),               &vd3, sizeof(vd3));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2) + sizeof(vd3), &vd4, sizeof(vd4));

    quad_batch_append_quad(ui_context.panel_batch, transform, vert_data);
}

void ui_panel_round(rectangle_t rect, rgba_t bg_color, f32 border_radius)
{
    transform2D_s transform = { 0 };

    transform.position = vec2(rect.x, rect.y);
    transform.rotation = rect.r;
    transform.scale    = vec2(rect.w, rect.h);

    struct {
        f32 border_radius, border_thickness;
        f32 bg_r, bg_g, bg_b, bg_a;
        f32 border_r, border_g, border_b, border_a;
        f32 u, v;
        f32 panel_scale_x, panel_scale_y;
        f32 shadow_r, shadow_g, shadow_b, shadow_a;
        f32 frag_coords_x, frag_coords_y;
        u32 texture_index;
    } vd1, vd2, vd3, vd4;

    char vert_data [ 4U * sizeof(vd1) ];

    rgbaf_t bg = rgbaf_from_rgba(bg_color);

    vd1.border_radius    = border_radius;
    vd1.border_thickness = 0.0F;
    vd1.bg_r             = bg.r;
    vd1.bg_g             = bg.g;
    vd1.bg_b             = bg.b;
    vd1.bg_a             = bg.a;
    vd1.border_r         = 0.0F;
    vd1.border_g         = 0.0F;
    vd1.border_b         = 0.0F;
    vd1.border_a         = 0.0F;
    vd1.u                = 1.0F;
    vd1.v                = 1.0F;
    vd1.panel_scale_x    = rect.w;
    vd1.panel_scale_y    = rect.h;
    vd1.shadow_r         = 0.0F;
    vd1.shadow_g         = 0.0F;
    vd1.shadow_b         = 0.0F;
    vd1.shadow_a         = 0.0F;
    vd1.frag_coords_x    = 1.0F;
    vd1.frag_coords_y    = 1.0F;
    vd1.texture_index    = 0U;

    vd2.border_radius    = border_radius;
    vd2.border_thickness = 0.0F;
    vd2.bg_r             = bg.r;
    vd2.bg_g             = bg.g;
    vd2.bg_b             = bg.b;
    vd2.bg_a             = bg.a;
    vd2.border_r         = 0.0F;
    vd2.border_g         = 0.0F;
    vd2.border_b         = 0.0F;
    vd2.border_a         = 0.0F;
    vd2.u                = 1.0F;
    vd2.v                = 0.0F;
    vd2.panel_scale_x    = rect.w;
    vd2.panel_scale_y    = rect.h;
    vd2.shadow_r         = 0.0F;
    vd2.shadow_g         = 0.0F;
    vd2.shadow_b         = 0.0F;
    vd2.shadow_a         = 0.0F;
    vd2.frag_coords_x    = 1.0F;
    vd2.frag_coords_y    = 0.0F;
    vd2.texture_index    = 0U;

    vd3.border_radius    = border_radius;
    vd3.border_thickness = 0.0F;
    vd3.bg_r             = bg.r;
    vd3.bg_g             = bg.g;
    vd3.bg_b             = bg.b;
    vd3.bg_a             = bg.a;
    vd3.border_r         = 0.0F;
    vd3.border_g         = 0.0F;
    vd3.border_b         = 0.0F;
    vd3.border_a         = 0.0F;
    vd3.u                = 0.0F;
    vd3.v                = 1.0F;
    vd3.panel_scale_x    = rect.w;
    vd3.panel_scale_y    = rect.h;
    vd3.shadow_r         = 0.0F;
    vd3.shadow_g         = 0.0F;
    vd3.shadow_b         = 0.0F;
    vd3.shadow_a         = 0.0F;
    vd3.frag_coords_x    = 0.0F;
    vd3.frag_coords_y    = 1.0F;
    vd3.texture_index    = 0U;

    vd4.border_radius    = border_radius;
    vd4.border_thickness = 0.0F;
    vd4.bg_r             = bg.r;
    vd4.bg_g             = bg.g;
    vd4.bg_b             = bg.b;
    vd4.bg_a             = bg.a;
    vd4.border_r         = 0.0F;
    vd4.border_g         = 0.0F;
    vd4.border_b         = 0.0F;
    vd4.border_a         = 0.0F;
    vd4.u                = 0.0F;
    vd4.v                = 0.0F;
    vd4.panel_scale_x    = rect.w;
    vd4.panel_scale_y    = rect.h;
    vd4.shadow_r         = 0.0F;
    vd4.shadow_g         = 0.0F;
    vd4.shadow_b         = 0.0F;
    vd4.shadow_a         = 0.0F;
    vd4.frag_coords_x    = 0.0F;
    vd4.frag_coords_y    = 0.0F;
    vd4.texture_index    = 0U;

    memcpy(vert_data,                                           &vd1, sizeof(vd1));
    memcpy(vert_data + sizeof(vd1),                             &vd2, sizeof(vd2));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2),               &vd3, sizeof(vd3));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2) + sizeof(vd3), &vd4, sizeof(vd4));

    quad_batch_append_quad(ui_context.panel_batch, transform, vert_data);
}

void ui_panel_round_with_borders(rectangle_t rect, rgba_t bg_color, rgba_t border_color, f32 border_thick, f32 border_radius)
{
    transform2D_s transform = { 0 };

    transform.position = vec2(rect.x, rect.y);
    transform.rotation = rect.r;
    transform.scale    = vec2(rect.w, rect.h);

    struct {
        f32 border_radius, border_thickness;
        f32 bg_r, bg_g, bg_b, bg_a;
        f32 border_r, border_g, border_b, border_a;
        f32 u, v;
        f32 panel_scale_x, panel_scale_y;
        f32 shadow_r, shadow_g, shadow_b, shadow_a;
        f32 frag_coords_x, frag_coords_y;
        u32 texture_index;
    } vd1, vd2, vd3, vd4;

    char vert_data [ 4U * sizeof(vd1) ];

    rgbaf_t bg = rgbaf_from_rgba(bg_color);

    vd1.border_radius    = border_radius;
    vd1.border_thickness = border_thick;
    vd1.bg_r             = bg.r;
    vd1.bg_g             = bg.g;
    vd1.bg_b             = bg.b;
    vd1.bg_a             = bg.a;
    vd1.border_r         = border_color.r;
    vd1.border_g         = border_color.g;
    vd1.border_b         = border_color.b;
    vd1.border_a         = border_color.a;
    vd1.u                = 1.0F;
    vd1.v                = 1.0F;
    vd1.panel_scale_x    = rect.w;
    vd1.panel_scale_y    = rect.h;
    vd1.shadow_r         = 0.0F;
    vd1.shadow_g         = 0.0F;
    vd1.shadow_b         = 0.0F;
    vd1.shadow_a         = 0.0F;
    vd1.frag_coords_x    = 1.0F;
    vd1.frag_coords_y    = 1.0F;
    vd1.texture_index    = 0U;

    vd2.border_radius    = border_radius;
    vd2.border_thickness = border_thick;
    vd2.bg_r             = bg.r;
    vd2.bg_g             = bg.g;
    vd2.bg_b             = bg.b;
    vd2.bg_a             = bg.a;
    vd2.border_r         = border_color.r;
    vd2.border_g         = border_color.g;
    vd2.border_b         = border_color.b;
    vd2.border_a         = border_color.a;
    vd2.u                = 1.0F;
    vd2.v                = 0.0F;
    vd2.panel_scale_x    = rect.w;
    vd2.panel_scale_y    = rect.h;
    vd2.shadow_r         = 0.0F;
    vd2.shadow_g         = 0.0F;
    vd2.shadow_b         = 0.0F;
    vd2.shadow_a         = 0.0F;
    vd2.frag_coords_x    = 1.0F;
    vd2.frag_coords_y    = 0.0F;
    vd2.texture_index    = 0U;

    vd3.border_radius    = border_radius;
    vd3.border_thickness = border_thick;
    vd3.bg_r             = bg.r;
    vd3.bg_g             = bg.g;
    vd3.bg_b             = bg.b;
    vd3.bg_a             = bg.a;
    vd3.border_r         = border_color.r;
    vd3.border_g         = border_color.g;
    vd3.border_b         = border_color.b;
    vd3.border_a         = border_color.a;
    vd3.u                = 0.0F;
    vd3.v                = 1.0F;
    vd3.panel_scale_x    = rect.w;
    vd3.panel_scale_y    = rect.h;
    vd3.shadow_r         = 0.0F;
    vd3.shadow_g         = 0.0F;
    vd3.shadow_b         = 0.0F;
    vd3.shadow_a         = 0.0F;
    vd3.frag_coords_x    = 0.0F;
    vd3.frag_coords_y    = 1.0F;
    vd3.texture_index    = 0U;

    vd4.border_radius    = border_radius;
    vd4.border_thickness = border_thick;
    vd4.bg_r             = bg.r;
    vd4.bg_g             = bg.g;
    vd4.bg_b             = bg.b;
    vd4.bg_a             = bg.a;
    vd4.border_r         = border_color.r;
    vd4.border_g         = border_color.g;
    vd4.border_b         = border_color.b;
    vd4.border_a         = border_color.a;
    vd4.u                = 0.0F;
    vd4.v                = 0.0F;
    vd4.panel_scale_x    = rect.w;
    vd4.panel_scale_y    = rect.h;
    vd4.shadow_r         = 0.0F;
    vd4.shadow_g         = 0.0F;
    vd4.shadow_b         = 0.0F;
    vd4.shadow_a         = 0.0F;
    vd4.frag_coords_x    = 0.0F;
    vd4.frag_coords_y    = 0.0F;
    vd4.texture_index    = 0U;

    memcpy(vert_data,                                           &vd1, sizeof(vd1));
    memcpy(vert_data + sizeof(vd1),                             &vd2, sizeof(vd2));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2),               &vd3, sizeof(vd3));
    memcpy(vert_data + sizeof(vd1) + sizeof(vd2) + sizeof(vd3), &vd4, sizeof(vd4));

    quad_batch_append_quad(ui_context.panel_batch, transform, vert_data);
}

void ui_text(s32* codepoints, u32 length, vec2_t top_left, rgba_t text_color)
{
    rgbaf_t tc = rgbaf_from_rgba(text_color);

    f32 font_width = font_get_width(ui_context.default_font_type);
    f32 font_height = font_get_height(ui_context.default_font_type);

    f32 x_off = 0.0F;
    for (u32 i = 0; i < length; i++)
    {
        font_char_t font_char = font_get_codepoint_data(ui_context.default_font_type, codepoints[i]); 
        rectangle_t char_rect = rectangle((f32)font_char.x / (f32)font_width, (f32)font_char.y / (f32)font_height, (f32)font_char.w / (f32)font_width, (f32)font_char.h / (f32)font_height, 0);


        struct {
            f32 text_r, text_g, text_b, text_a;
            f32 u, v;
            u32 texture_index;
        } vd1, vd2, vd3, vd4;

        transform2D_s transform = (transform2D_s) {
            .position = vec2(top_left.x + x_off - font_char.origin_x, top_left.y - font_char.origin_y),
            .scale    = vec2(char_rect.w * font_width, char_rect.h * font_height),
            .rotation = 0.0F
        };
        x_off += char_rect.w * font_width;

        // Top right
        vd1.text_r        = tc.r;
        vd1.text_g        = tc.g;
        vd1.text_b        = tc.b;
        vd1.text_a        = tc.a;
        vd1.texture_index = 0U;
        vd1.u             = char_rect.x + char_rect.w;
        vd1.v             = char_rect.y;

        // Bottom right
        vd2.text_r        = tc.r;
        vd2.text_g        = tc.g;
        vd2.text_b        = tc.b;
        vd2.text_a        = tc.a;
        vd2.texture_index = 0U;
        vd2.u             = char_rect.x + char_rect.w;
        vd2.v             = char_rect.y + char_rect.h;

        // Top left
        vd3.text_r        = tc.r;
        vd3.text_g        = tc.g;
        vd3.text_b        = tc.b;
        vd3.text_a        = tc.a;
        vd3.texture_index = 0U;
        vd3.u             = char_rect.x;
        vd3.v             = char_rect.y;

        // Bottom left
        vd4.text_r        = tc.r;
        vd4.text_g        = tc.g;
        vd4.text_b        = tc.b;
        vd4.text_a        = tc.a;
        vd4.texture_index = 0U;
        vd4.u             = char_rect.x;
        vd4.v             = char_rect.y + char_rect.h;

        char data [ 4U * sizeof(vd1) ];

        memcpy(data,                                           &vd1, sizeof(vd1));
        memcpy(data + sizeof(vd1),                             &vd2, sizeof(vd2));
        memcpy(data + sizeof(vd1) + sizeof(vd2),               &vd3, sizeof(vd3));
        memcpy(data + sizeof(vd1) + sizeof(vd2) + sizeof(vd3), &vd4, sizeof(vd4));

        quad_batch_append_quad(ui_context.text_batch, transform, data);
    }
}



u32 create_shader_program(const char* vs_path, const char* fs_path)
{
    u32 prog;

    char* vs = read_entire_file(vs_path);
    char* fs = read_entire_file(fs_path);

    prog = glCreateProgram();

    u32 v_id = glCreateShader(GL_VERTEX_SHADER);
    u32 f_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(v_id, 1, (const char* const*)&vs, NULL );
    glShaderSource(f_id, 1, (const char* const*)&fs, NULL );

    int success;
    char buff [1024];

    glCompileShader(v_id);
    glGetShaderiv(v_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(v_id, 1024, NULL, buff);
        LOG_ERROR("'%s': vertex shader error: \n%s\n", vs_path, buff);
        return 0;
    }
    glCompileShader(f_id);
    glGetShaderiv(f_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(f_id, 1024, NULL, buff);
        LOG_ERROR("'%s': fragmnet shader error: \n%s\n", fs_path, buff);
        return 0;
    }

    glAttachShader(prog, v_id);
    glAttachShader(prog, f_id);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(prog, 1024, NULL, buff);
        LOG_ERROR("'%s', '%s': Linking shaders error: \n%s\n", vs_path, fs_path, buff);
        return 0;
    }

    glDeleteShader(v_id);
    glDeleteShader(f_id);
    free(vs);
    free(fs);

    return prog;
}

u32 create_texture(const char* image_path)
{
    u32 tex;

    s32 width, height, channels; 
    u8* pixels = stbi_load(image_path, &width, &height, &channels, STBI_rgb_alpha);

    UNUSED(width); UNUSED(height); UNUSED(channels);

    if (!pixels)
    {
        LOG_ERROR("Failed loading image: '%s'\n", image_path);
        return 0;
    }

    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);

    return tex;
}
