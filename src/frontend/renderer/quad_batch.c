#include "quad_batch.h"
#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
#include "frontend/renderer/window.h"
#include "glad.h"

typedef struct quad_batch_t {
    u32 sizeof_vertex_data;
    u64 quad_max_count;
    struct {
        u64 count;
        void* data;
    } vertices;
    struct {
        u32 VAO, VBO, shader_program, texture_id;
    } renderer;
} quad_batch_t;

// OpenGL Coordinates-system based
static const f32 QUAD_POSITIONS [] =
    {
        1.0F,  1.0F,  // top right
        1.0F,  -1.0F, // bottom right
        -1.0F, 1.0F,  // top left
        -1.0F, -1.0F, // bottom left
    };

static inline u32 sizeof_attribute_type(attributes_type_e type)
{
    switch (type)
    {
        case ATTRIBUTE_TYPE_FLOAT       : return sizeof(f32);
        case ATTRIBUTE_TYPE_UNSIGNED_INT: return sizeof(u32);
        default:
            LOG_ERROR("ATTRIBUTE_TYPE was not recognized!");
    }
    return 0;
}

static inline GLenum get_gl_attribute_type(attributes_type_e type)
{
    switch (type)
    {
        case ATTRIBUTE_TYPE_FLOAT: return GL_FLOAT;
        case ATTRIBUTE_TYPE_UNSIGNED_INT: return GL_UNSIGNED_INT;
        default:
            LOG_ERROR("ATTRIBUTE_TYPE was not recognized!");
    }
    return 0;
}

quad_batch_t* quad_batch_create(quad_batch_create_info_s create_info)
{
    quad_batch_t* batch = malloc(sizeof(quad_batch_t));
    assert(batch != NULL);

    u32 vertex_size = 0;
    for (u8 i = 0; i < create_info.vertex_attributes.count; i++)
    {
        vertex_size += sizeof_attribute_type(create_info.vertex_attributes.attributes_type[i]) * create_info.vertex_attributes.attributes_count[i];
    }
    batch->sizeof_vertex_data      = vertex_size;
    batch->quad_max_count          = create_info.quads_max_count;
    batch->renderer.shader_program = create_info.shader_program;
    batch->renderer.texture_id     = create_info.texture_id;
    batch->vertices.data           = malloc(batch->sizeof_vertex_data * batch->quad_max_count * 4);
    batch->vertices.count          = 0ULL;
    assert(batch->vertices.data != NULL);

    u32 VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, batch->quad_max_count * 4 * batch->sizeof_vertex_data, NULL, GL_DYNAMIC_DRAW);

    u64 location = 0, offset = 0;
    for (u8 i = 0; i < create_info.vertex_attributes.count; i++)
    {
        u8 count               = create_info.vertex_attributes.attributes_count[i];
        attributes_type_e type = create_info.vertex_attributes.attributes_type[i];
        u8 size                = sizeof_attribute_type(type);
        glVertexAttribPointer(location, count, get_gl_attribute_type(type), GL_FALSE, batch->sizeof_vertex_data, (void*)offset);
        glEnableVertexAttribArray(location);

        location++;
        offset += size * count;
    }

    u32 indices [ batch->quad_max_count * 6 ];
    u32 vertex_base = 0;
    u32 index_base  = 0;
    for (u32 i = 0; i < batch->quad_max_count; i++)
    {
        indices[ index_base + 0 ] = vertex_base + 0; 
        indices[ index_base + 1 ] = vertex_base + 1;
        indices[ index_base + 2 ] = vertex_base + 3;
        indices[ index_base + 3 ] = vertex_base + 0;
        indices[ index_base + 4 ] = vertex_base + 3;
        indices[ index_base + 5 ] = vertex_base + 2;

        vertex_base += 4;
        index_base  += 6;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    batch->renderer.VAO = VAO;
    batch->renderer.VBO = VBO;
    return batch;
}

void quad_batch_destroy(quad_batch_t* batch)
{
    if (batch != NULL)
    {
        if (batch->vertices.data != NULL)
        {
            free(batch->vertices.data);
        }
        free(batch);
    }
}

void quad_batch_append_quad(quad_batch_t* batch, transform2D_s transform, void* vertex_data)
{
    assert(batch->vertices.count < batch->quad_max_count * 4);

    f32 window_width  = (f32)get_window_width();
    f32 window_height = (f32)get_window_height();

    mat3x3_t model = mat3x3_unit();
    model = mat3x3_translate_matrix(model, vec2(transform.position.x + transform.scale.x / 2.0F, window_height - transform.position.y - transform.scale.y / 2.0F));
    model = mat3x3_rotate_matrix(model, transform.rotation);
    model = mat3x3_scale_matrix(model, vec2_scale(transform.scale, 0.5F));

    vec3_t u1 = vec3_transform(model, vec3(QUAD_POSITIONS[ 0 ], QUAD_POSITIONS[ 1 ], 1.0F));
    vec3_t u2 = vec3_transform(model, vec3(QUAD_POSITIONS[ 2 ], QUAD_POSITIONS[ 3 ], 1.0F));
    vec3_t u3 = vec3_transform(model, vec3(QUAD_POSITIONS[ 4 ], QUAD_POSITIONS[ 5 ], 1.0F));
    vec3_t u4 = vec3_transform(model, vec3(QUAD_POSITIONS[ 6 ], QUAD_POSITIONS[ 7 ], 1.0F));

    vec2_t v1 = vec2_sub(vec2(2.0F * u1.x / window_width, 2.0F * u1.y / window_height ), vec2(1.0F, 1.0F));
    vec2_t v2 = vec2_sub(vec2(2.0F * u2.x / window_width, 2.0F * u2.y / window_height ), vec2(1.0F, 1.0F));
    vec2_t v3 = vec2_sub(vec2(2.0F * u3.x / window_width, 2.0F * u3.y / window_height ), vec2(1.0F, 1.0F));
    vec2_t v4 = vec2_sub(vec2(2.0F * u4.x / window_width, 2.0F * u4.y / window_height ), vec2(1.0F, 1.0F));

    u32 vtx_size = 2 * sizeof(f32);
    u64 offset   = 0;
    u64 size_to_copy = batch->sizeof_vertex_data - vtx_size; // 84

    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data, &v1, vtx_size);
    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data + vtx_size, vertex_data + offset, size_to_copy);
    batch->vertices.count++;
    offset += size_to_copy;

    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data, &v2, vtx_size);
    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data + vtx_size, vertex_data + offset, size_to_copy);
    batch->vertices.count++;
    offset += size_to_copy;

    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data, &v3, vtx_size);
    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data + vtx_size, vertex_data + offset, size_to_copy);
    batch->vertices.count++;
    offset += size_to_copy;

    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data, &v4, vtx_size);
    memcpy(batch->vertices.data + batch->vertices.count * batch->sizeof_vertex_data + vtx_size, vertex_data + offset, size_to_copy);
    batch->vertices.count++;
}

void quad_batch_submit(quad_batch_t* batch)
{
    glClear(GL_DEPTH_BUFFER_BIT); 
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(batch->renderer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, batch->renderer.VBO);
    // Populate buffer data
    glBufferSubData(
        GL_ARRAY_BUFFER, 
        0, 
        batch->sizeof_vertex_data * batch->vertices.count, 
        batch->vertices.data
    );

    if (glIsProgram(batch->renderer.shader_program)) {
        glUseProgram(batch->renderer.shader_program);   
    } 

    if (glIsTexture(batch->renderer.texture_id))
    {
        glBindTexture(GL_TEXTURE_2D, batch->renderer.texture_id);
    }

    // Draw call
    u64 indices_count = batch->vertices.count * 3 / 2;
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, (void*)0);
    batch->vertices.count = 0;
    
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
