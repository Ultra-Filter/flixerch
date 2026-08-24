#include "image_renderer.h"
#include "glad.h"
#include "assets_loader.h"
#include "window.h"

#define IMAGE_RENDERER_VS_PATH "assets/shaders/image.vs"
#define IMAGE_RENDERER_FS_PATH "assets/shaders/image.fs"

#define MAX_IMAGE_QUADS_COUNT 100

typedef struct image_vertex_data
{
    vec4_t position_frag_coords;
    vec3_t uvw;
    rgbaf_t tint_color_left;
    rgbaf_t tint_color_mid;
    rgbaf_t tint_color_right;
    rgbaf_t border_col;
    vec4_t border_radii;
    f32    border_thickness;
    rgbaf_t shadow_color;
    vec4_t shadow_offset_blur_spread;
    vec4_t image_scale_gradient_type_tint_strength;
} image_vertex_data;

static struct image_renderer_context {
    struct verts_data {
        u32 textures [ 32 ];
        u8 textures_count;
        image_vertex_data* data;
        u32 count;
    } vertices [ Z_INDEX_COUNT ];
    u32 VAO, VBO, shader;
} context;

static u8 context_is_tex_exists(u32 tex, z_index_type_e depth)
{
    for (u8 i = 0; i < context.vertices[depth].textures_count; i++)
    {
        if (context.vertices[depth].textures[ i ] == tex) return i;
    }
    return 32;
}

void image_renderer_init(void)
{
    s32 max_texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
    LOG_INFO("Max texture units on your machine: %d", max_texture_units);
    assert(max_texture_units >= 32);

    const u64 vertices_size = MAX_IMAGE_QUADS_COUNT * 4 * sizeof(image_vertex_data);
    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        context.vertices[i].data = malloc(vertices_size);
        assert(context.vertices);
        context.vertices[i].count = 0;
        context.vertices[i].textures_count = 0;
    }


    glGenVertexArrays(1, &context.VAO);
    glBindVertexArray(context.VAO);
    glGenBuffers(1, &context.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, context.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size * Z_INDEX_COUNT, NULL, GL_DYNAMIC_DRAW);

    u32 loc = 0, off = 0;
    // position, frag_coords
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // uvw
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 3 * sizeof(f32);
    // tint_color left
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // tint_color mid
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // tint_color right
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // border color
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // border radii
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // border thick
    glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 1 * sizeof(f32);
    // shadow col
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // shadow off, blur, spread
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);
    // image_scale_gradient_type_tint_strength
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_data), (void*)off);
    glEnableVertexAttribArray(loc);
    loc++; off += 4 * sizeof(f32);

    u32  indices [ Z_INDEX_COUNT * MAX_IMAGE_QUADS_COUNT * 6 ];

    for (u32 i = 0; i < MAX_IMAGE_QUADS_COUNT * Z_INDEX_COUNT; i++)
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * Z_INDEX_COUNT * MAX_IMAGE_QUADS_COUNT * 6, indices, GL_STATIC_DRAW);


    bool success = asset_loader_create_shader_from_file(IMAGE_RENDERER_VS_PATH, IMAGE_RENDERER_FS_PATH, &context.shader);
    if(!success) LOG_ERROR("Image renderer could not be initilized because shader compilation failed.");

    glUseProgram(context.shader);
    for (u32 i = 0; i < 32; i++)
    {
        char buff [ 32 ];
        sprintf(buff, "texs[%d]", i);
        u32 loc = glGetUniformLocation(context.shader, buff);
        glUniform1i(loc, i);
    }
}

void image_renderer_shutdown(void)
{
    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        if (context.vertices[i].data) free(context.vertices[i].data);
        context.vertices[i].count = 0;
        context.vertices[i].textures_count = 0;
    }
    context.shader         = 0;
    context.VAO            = 0;
    context.VBO            = 0;
}

void image_renderer_draw(u32 tex_id, transform2D_s trans, image_style_s style, z_index_type_e depth)
{
    assert(depth < Z_INDEX_COUNT);
    if (context.vertices[depth].count >= MAX_IMAGE_QUADS_COUNT * 4 || context.vertices[depth].textures_count == 32)
    {
        image_renderer_submit(); // Flush ????????????????
    }

    u32 tex_index = context_is_tex_exists(tex_id, depth);
    if (tex_index == 32 /* Does not exists */) // TODO: atlasing
    {
        tex_index = context.vertices[depth].textures_count;
        context.vertices[depth].textures[ context.vertices[depth].textures_count++ ] = tex_id;
    }

    // OpenGL Coordinates-system based
    const f32 QUAD_POSITIONS [] =
        {
            1.0F,  1.0F ,  // top right
            1.0F,  -1.0F, // bottom right
            -1.0F, 1.0F ,  // top left
            -1.0F, -1.0F, // bottom left
        };
    const f32 paddings = 200.0F;

    f32 window_width  = (f32)get_window_width();
    f32 window_height = (f32)get_window_height();
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

    image_vertex_data vd = (image_vertex_data)
    {   
        .border_col = rgbaf_from_rgba(style.border_color),
        .border_radii = style.border_radii,
        .border_thickness = style.border_thickness,
        .image_scale_gradient_type_tint_strength = vec4(transform.scale.x, transform.scale.y, style.gradient_type, style.tint_strength),
        .shadow_color = rgbaf_from_rgba(style.shadow_color),
        .shadow_offset_blur_spread = vec4(style.shadow_offset.x, style.shadow_offset.y, style.shadow_blur, style.shadow_spread),
        .tint_color_left = rgbaf_from_rgba(style.tint_color_left),
        .tint_color_right = rgbaf_from_rgba(style.tint_color_right),
        .tint_color_mid = rgbaf_from_rgba(style.tint_color_middle)
    };

    image_vertex_data vd1 = vd, vd2 = vd, vd3 = vd, vd4 = vd;

    vd1.position_frag_coords = vec4(v1.x, v1.y, transform.scale.x, transform.scale.y);
    vd1.uvw                  = vec3(1.0F, 1.0F, (f32)tex_index);

    vd2.position_frag_coords = vec4(v2.x, v2.y, transform.scale.x, 0.0F);
    vd2.uvw                  = vec3(1.0F, 0.0F, (f32)tex_index);

    vd3.position_frag_coords = vec4(v3.x, v3.y, 0.0F, transform.scale.y);
    vd3.uvw                  = vec3(0.0F, 1.0F, (f32)tex_index);

    vd4.position_frag_coords = vec4(v4.x, v4.y, 0.0F, 0.0F);
    vd4.uvw                  = vec3(0.0F, 0.0F, (f32)tex_index);
    
    context.vertices[ depth ].data[ context.vertices[ depth ].count++ ] = vd1;
    context.vertices[ depth ].data[ context.vertices[ depth ].count++ ] = vd2;
    context.vertices[ depth ].data[ context.vertices[ depth ].count++ ] = vd3;
    context.vertices[ depth ].data[ context.vertices[ depth ].count++ ] = vd4;
}

void image_renderer_push(void)
{
    glBindVertexArray(context.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, context.VBO);
    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        if (context.vertices[i].count != 0)
        {

            glBufferSubData(
                GL_ARRAY_BUFFER, 
                i * MAX_IMAGE_QUADS_COUNT * 4 * sizeof(image_vertex_data), 
                sizeof(image_vertex_data) * context.vertices[i].count, 
                context.vertices[i].data
            );
        }
    }
}

void image_renderer_render(z_index_type_e depth)
{
    assert(depth < Z_INDEX_COUNT);

    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(context.shader);
    
    for (u32 i = 0; i < context.vertices[depth].textures_count; i++)
    {    
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, context.vertices[depth].textures[ i ]);    
    }
    context.vertices[depth].textures_count = 0; // TODO: cache

    glBindVertexArray(context.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, context.VBO);

    const u32 indices_count = 3 * context.vertices[depth].count / 2;
    if (indices_count > 0)
    {

        glDrawElements(
            GL_TRIANGLES, 
            MAX_IMAGE_QUADS_COUNT * 6 * Z_INDEX_COUNT,
            GL_UNSIGNED_INT,  
            (void*)(depth * MAX_IMAGE_QUADS_COUNT * 6 * sizeof(u32))
        );
    }
    context.vertices[depth].count = 0;
}

void image_renderer_submit(void)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glUseProgram(context.shader);
    
    glBindVertexArray(context.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, context.VBO);
    
    u64 verts_count = 0;
    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        glBufferSubData(GL_ARRAY_BUFFER, verts_count, sizeof(image_vertex_data) * context.vertices[i].count, context.vertices[i].data);
        verts_count += context.vertices[i].count;
        context.vertices[i].count = 0;
    }

    for (u8 depth = 0; depth < Z_INDEX_COUNT; depth++)
    {
        for (u32 i = 0; i < context.vertices[depth].textures_count; i++)
        {    
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, context.vertices[depth].textures[ i ]);    
        }
        context.vertices[depth].textures_count = 0;
        
        glDrawElements(GL_TRIANGLES, 3 * context.vertices[depth].count / 2, GL_UNSIGNED_INT, (void*)(depth * MAX_IMAGE_QUADS_COUNT * 6 * sizeof(u32)));
        context.vertices[depth].count = 0;
    }
}