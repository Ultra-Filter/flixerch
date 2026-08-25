#include "quad_renderer.h"
#include "assets_loader.h"
#include "glad.h"
#include "window.h"

#define QUAD_RENDERER_VS_PATH "assets/shaders/quad.vs"
#define QUAD_RENDERER_FS_PATH "assets/shaders/quad.fs"
#define QUAD_RENDERER_MAX_QUADS_COUNT 128

/*
    Vertex attributes:

        vec4 position_frag_coords;
        vec4 bg_color_left;
        vec4 bg_color_mid;
        vec4 bg_color_right;
        vec4 border_col;
        vec4 border_radius;
        f32 border_thickness;
        vec4 shadow_color;
        vec4 shadow_offset_blur_spread;
*/

typedef struct quad_vertex_data
{
    vec4_t position_frag_coords;
    rgbaf_t bg_color_left;
    rgbaf_t bg_color_mid;
    rgbaf_t bg_color_right;
    rgbaf_t border_col;
    vec4_t border_radii;
    f32    border_thickness;
    rgbaf_t shadow_color;
    vec4_t shadow_offset_blur_spread;
    vec3_t quad_scale_gradient_type;
} quad_vertex_data;

static struct quad_context {
    struct {
        u64 count;
        quad_vertex_data* data;
    } vertices [ Z_INDEX_COUNT ];
    u32 VAO, VBO , shader_program;
} context;


void quad_renderer_init(void)
{
    bool success = asset_loader_create_shader_from_file(QUAD_RENDERER_VS_PATH, QUAD_RENDERER_FS_PATH, &context.shader_program); 
    if (!success)
    {
        LOG_ERROR("Quad shader could not be created.");
        return;
    } 
    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        context.vertices[i].count = 0;
        context.vertices[i].data  = malloc(4 * sizeof(quad_vertex_data) * QUAD_RENDERER_MAX_QUADS_COUNT);
        assert(context.vertices[i].data != NULL);
    }

    glGenVertexArrays(1, &context.VAO);
    glBindVertexArray(context.VAO);

    glGenBuffers(1, &context.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, context.VBO);
    glBufferData(
        GL_ARRAY_BUFFER, 
        4 * sizeof(quad_vertex_data) * QUAD_RENDERER_MAX_QUADS_COUNT * Z_INDEX_COUNT, 
        NULL, 
        GL_DYNAMIC_DRAW
    );
    u32 loc = 0, off = 0;
    // Position, frag coords
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // background color left
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // background color middle
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // background color right
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // border color 
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // border radius
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // border thickness
    glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 1 * sizeof(f32);
    // shadow color
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // shadow offset, blur, spreead
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // quad_scale
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(quad_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 3 * sizeof(f32);

        
    u32 indices [ 6 * QUAD_RENDERER_MAX_QUADS_COUNT * Z_INDEX_COUNT ];
    for (u32 i = 0; i < QUAD_RENDERER_MAX_QUADS_COUNT * Z_INDEX_COUNT; i++)
    {
        indices [ 6 * i + 0 ] = 4 * i + 0;
        indices [ 6 * i + 1 ] = 4 * i + 1;
        indices [ 6 * i + 2 ] = 4 * i + 3;

        indices [ 6 * i + 3 ] = 4 * i + 0;
        indices [ 6 * i + 4 ] = 4 * i + 3;
        indices [ 6 * i + 5 ] = 4 * i + 2;
    }

    u32 IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

void quad_renderer_shutdown(void)
{
    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
        if (context.vertices[i].data) 
            free(context.vertices[i].data);

    glDeleteProgram(context.shader_program);
}

void quad_renderer_draw(transform2D_s trans, quad_style_s style, z_index_type_e depth)
{
    assert(depth < Z_INDEX_COUNT);
    assert(context.vertices[ depth ].count < QUAD_RENDERER_MAX_QUADS_COUNT * 4); 
    assert(context.vertices[ depth ].data != NULL);
        
    f32 window_width  = (f32)get_window_width();
    f32 window_height = (f32)get_window_height();
    const f32 paddings = 200.0F;
    const f32 QUAD_POSITIONS [] =
    {
        1.0F,  1.0F ,  // top right
        1.0F,  -1.0F, // bottom right
        -1.0F, 1.0F ,  // top left
        -1.0F, -1.0F, // bottom left
    };

    transform2D_s transform = transform2D(vec2_sub_val(trans.position, paddings), vec2_add_val(trans.scale, 2.0F * paddings), trans.rotation);

    mat3x3_t model = mat3x3_unit();

    model = mat3x3_translate_matrix(
        model, 
        vec2(
            transform.position.x + transform.scale.x / 2.0F, 
            window_height - transform.position.y - transform.scale.y / 2.0F
        )
    );
    model = mat3x3_rotate_matrix(model, transform.rotation);
    model = mat3x3_scale_matrix(model, vec2_scale(transform.scale, 0.5F));

    vec3_t u1 = vec3_transform(model, vec3(QUAD_POSITIONS[ 0 ], QUAD_POSITIONS[ 1 ], 1.0F));
    vec3_t u2 = vec3_transform(model, vec3(QUAD_POSITIONS[ 2 ], QUAD_POSITIONS[ 3 ], 1.0F));
    vec3_t u3 = vec3_transform(model, vec3(QUAD_POSITIONS[ 4 ], QUAD_POSITIONS[ 5 ], 1.0F));
    vec3_t u4 = vec3_transform(model, vec3(QUAD_POSITIONS[ 6 ], QUAD_POSITIONS[ 7 ], 1.0F));

    vec2_t v1 = vec2(2.0F * u1.x / window_width - 1.0F, 2.0F * u1.y / window_height - 1.0F);
    vec2_t v2 = vec2(2.0F * u2.x / window_width - 1.0F, 2.0F * u2.y / window_height - 1.0F);
    vec2_t v3 = vec2(2.0F * u3.x / window_width - 1.0F, 2.0F * u3.y / window_height - 1.0F);
    vec2_t v4 = vec2(2.0F * u4.x / window_width - 1.0F, 2.0F * u4.y / window_height - 1.0F);

    quad_vertex_data vd = { 0 };
    vd.bg_color_left             = rgbaf_from_rgba(style.background_color_left);
    vd.bg_color_mid              = rgbaf_from_rgba(style.background_color_middle);
    vd.bg_color_right            = rgbaf_from_rgba(style.background_color_right);
    vd.border_col                = rgbaf_from_rgba(style.border_color);
    vd.border_radii              = style.border_radii;
    vd.border_thickness          = style.border_thickness;
    vd.shadow_color              = rgbaf_from_rgba(style.shadow_color);
    vd.shadow_offset_blur_spread = vec4(style.shadow_offset.x, style.shadow_offset.y, style.shadow_blur, style.shadow_spread);
    vd.quad_scale_gradient_type  = vec3(transform.scale.x, transform.scale.y, style.gradient_type);


    quad_vertex_data vd1 = vd, vd2 = vd, vd3 = vd, vd4 = vd;

    vd1.position_frag_coords = vec4(v1.x, v1.y, transform.scale.x, transform.scale.y);
    vd2.position_frag_coords = vec4(v2.x, v2.y, transform.scale.x, 0.0F             );
    vd3.position_frag_coords = vec4(v3.x, v3.y, 0.0F             , transform.scale.y);
    vd4.position_frag_coords = vec4(v4.x, v4.y, 0.0F             , 0.0F             );

    context.vertices[ depth ].data [ context.vertices[ depth ].count++ ] = vd1;
    context.vertices[ depth ].data [ context.vertices[ depth ].count++ ] = vd2;
    context.vertices[ depth ].data [ context.vertices[ depth ].count++ ] = vd3;
    context.vertices[ depth ].data [ context.vertices[ depth ].count++ ] = vd4;

}

void quad_renderer_push(void)
{
    glBindVertexArray(context.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, context.VBO);

    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        glBufferSubData(
            GL_ARRAY_BUFFER, 
            sizeof(quad_vertex_data) * QUAD_RENDERER_MAX_QUADS_COUNT * i * 4, 
            sizeof(quad_vertex_data) * context.vertices[i].count, 
            context.vertices[i].data
        );
    }
}

void quad_renderer_render(z_index_type_e depth)
{
    assert(depth < Z_INDEX_COUNT);

    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(context.shader_program);
    
    glBindVertexArray(context.VAO);
    const u32 indices_count = 3 * context.vertices[depth].count / 2;
    glDrawElements(
        GL_TRIANGLES, 
        indices_count, 
        GL_UNSIGNED_INT, 
        (void*)(depth * QUAD_RENDERER_MAX_QUADS_COUNT * 6 * sizeof(u32))
    );
    context.vertices[depth].count = 0;
}

void quad_renderer_submit(void)
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(context.shader_program);
    
    glBindVertexArray(context.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, context.VBO);
    u64 num_verts = 0;
    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        glBufferSubData(
            GL_ARRAY_BUFFER, 
            sizeof(quad_vertex_data) * num_verts, 
            sizeof(quad_vertex_data) * context.vertices[i].count, 
            context.vertices[i].data
        );
        num_verts += context.vertices[i].count;
        context.vertices[i].count = 0;
    }

    const u32 indices_count = 3 * num_verts / 2; 
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, (void*)0);

    glDisable(GL_BLEND);
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}
