#pragma once

#include "core/utils/defines.h"
#include "color.h"
#include "core/utils/math_engine.h"

typedef struct quad_batch_t quad_batch_t;

typedef enum attributes_type_e 
{
    ATTRIBUTE_TYPE_FLOAT        = 0,
    ATTRIBUTE_TYPE_UNSIGNED_INT = 1,
    ATTRIBUTE_TYPE_COUNT        = 2
} attributes_type_e;

typedef struct quad_batch_create_info_s {
    u64 quads_max_count;
    struct 
    {
        u8 count;
        const u8* attributes_count;
        const attributes_type_e* attributes_type;
    } vertex_attributes;
    u32 shader_program, texture_id;
} quad_batch_create_info_s;

quad_batch_t* quad_batch_create(quad_batch_create_info_s create_info);
void quad_batch_destroy(quad_batch_t* batch);
// Appending new quad for next draw call.
void quad_batch_append_quad(quad_batch_t* batch, transform2D_s transform, void* vertex_data);
// Execute draw call.
void quad_batch_submit(quad_batch_t* batch);
