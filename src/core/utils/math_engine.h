#pragma once
#include "defines.h"

typedef struct vec2_t
{
    float x, y;
} vec2_t;

typedef struct vec3_t
{
    float x, y, z;
} vec3_t;

typedef struct vec4_t
{
    float x, y, z, w;
} vec4_t;

typedef struct mat2x2_t
{
    float a11, a12;
    float a21, a22;
} mat2x2_t;

typedef struct mat3x3_t
{
    float a11, a12, a13;
    float a21, a22, a23;
    float a31, a32, a33;
} mat3x3_t;

typedef struct mat4x4_t
{
    float a11, a12, a13, a14;
    float a21, a22, a23, a24;
    float a31, a32, a33, a34;
    float a41, a42, a43, a44;
} mat4x4_t;

vec2_t vec2(f32 x, f32 y);
vec2_t vec2v(f32 v);
vec2_t vec2_add(vec2_t v1, vec2_t v2);
vec2_t vec2_sub(vec2_t v1, vec2_t v2);
vec2_t vec2_mul(vec2_t v1, vec2_t v2);
vec2_t vec2_div(vec2_t v1, vec2_t v2);
vec2_t vec2_scale(vec2_t v, f32 s);
vec2_t vec2_add_val(vec2_t v, f32 s);
vec2_t vec2_sub_val(vec2_t v, f32 s);
vec2_t vec2_normalize(vec2_t v);
vec2_t vec2_linear_combination(vec2_t v1, f32 alpha1, vec2_t v2, f32 alpha2);
vec2_t vec2_map(vec2_t v, vec2_t a, vec2_t b, vec2_t target_a, vec2_t target_b);
f32    vec2_length(vec2_t v);
f32    vec2_length_sqr(vec2_t v);
f32    vec2_distance(vec2_t v1, vec2_t v2);
f32    vec2_distance_sqr(vec2_t v1, vec2_t v2);
f32    vec2_dot(vec2_t v1, vec2_t v2);

vec3_t vec3(f32 x, f32 y, f32 z);
vec3_t vec3v(f32 x);
vec3_t vec3_add(vec3_t v1, vec3_t v2);
vec3_t vec3_sub(vec3_t v1, vec3_t v2);
vec3_t vec3_mul(vec3_t v1, vec3_t v2);
vec3_t vec3_div(vec3_t v1, vec3_t v2);
vec3_t vec3_scale(vec3_t v, f32 s);
vec3_t vec3_add_val(vec3_t v, f32 s);
vec3_t vec3_sub_val(vec3_t v, f32 s);
vec3_t vec3_cross(vec3_t v1, vec3_t v2);
vec3_t vec3_normalize(vec3_t v);
vec3_t vec3_linear_combination(vec3_t v1, f32 alpha1, vec3_t v2, f32 alpha2, vec3_t vce3, f32 alpha3);
f32    vec3_length(vec3_t v);
f32    vec3_length_sqr(vec3_t v);
f32    vec3_distance(vec3_t v1, vec3_t v2);
f32    vec3_distance_sqr(vec3_t v1, vec3_t v2);
f32    vec3_dot(vec3_t v1, vec3_t v2);
vec3_t vec3_up(void);
vec3_t vec3_down(void);
vec3_t vec3_right(void);
vec3_t vec3_left(void);
vec3_t vec3_forward(void);
vec3_t vec3_backward(void);

vec4_t vec4(f32 x, f32 y, f32 z, f32 w);
vec4_t vec4v(f32 val);
vec4_t vec4_from_vec3_1(vec3_t v);
vec4_t vec4_transform(mat4x4_t m, vec4_t v);
f32    vec4_dot(vec4_t v1, vec4_t v2);

mat3x3_t mat3x3_unit(void);
mat3x3_t mat3x3(int val);
mat3x3_t mat3x3_add(mat3x3_t m1, mat3x3_t m2);
mat3x3_t mat3x3_sub(mat3x3_t m1, mat3x3_t m2);
mat3x3_t mat3x3_mul(mat3x3_t m1, mat3x3_t m2);
mat3x3_t mat3x3_scale(mat3x3_t m, f32 val);
mat3x3_t mat3x3_add_val(mat3x3_t m, f32 val);
mat3x3_t mat3x3_sub_val(mat3x3_t m, f32 val);
mat3x3_t mat3x3_div_val(mat3x3_t m, f32 val);
mat3x3_t mat3x3_rotation_x(f32 angle);
mat3x3_t mat3x3_rotation_y(f32 angle);
mat3x3_t mat3x3_rotation_z(f32 angle);
mat3x3_t mat3x3_rotation_along(vec3_t rotation_axis, f32 angle);

mat4x4_t mat4x4_unit(void);
mat4x4_t mat4x4(f32 val);
mat4x4_t mat4x4_add(mat4x4_t m1, mat4x4_t m2);
mat4x4_t mat4x4_sub(mat4x4_t m1, mat4x4_t m2);
mat4x4_t mat4x4_mul(mat4x4_t m1, mat4x4_t m2);
mat4x4_t mat4x4_scale(mat4x4_t m, f32 s);
mat4x4_t mat4x4_add_val(mat4x4_t m, f32 val);
mat4x4_t mat4x4_sub_val(mat4x4_t m, f32 val);
mat4x4_t mat4x4_div_val(mat4x4_t m, f32 val);
mat4x4_t mat4x4_scale_matrix(mat4x4_t m, vec3_t scale);
mat4x4_t mat4x4_translate_matrix(mat4x4_t m, vec3_t translation);
mat4x4_t mat4x4_rotate_matrix(mat4x4_t m, vec3_t rotation_axis, f32 angle);
mat4x4_t mat4x4_look_at_matrix(vec3_t position, vec3_t target, vec3_t up);
mat4x4_t mat4x4_perspective_matrix(f32 fovy, f32 aspect, f32 near, f32 far);
mat4x4_t mat4x4_transpose(mat4x4_t m);
mat4x4_t mat4x4_inverse(mat4x4_t m);
vec4_t   mat4x4_get_col(mat4x4_t m, int col_index);
vec4_t   mat4x4_get_row(mat4x4_t m, int row_index);
f32 *    mat4x4_to_f32_array(mat4x4_t m, f32 array[16]);
f32      mat4x4_trace(mat4x4_t m);