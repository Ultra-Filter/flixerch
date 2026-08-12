#include "math_engine.h"
#include "math.h"

vec2_t vec2(f32 x, f32 y)
{
    vec2_t result =  (vec2_t){ .x = x, .y = y };

    return result;
}

vec2_t vec2v(f32 v)
{
    vec2_t result = (vec2_t){ .x = v, .y = v };

    return result;
}

vec2_t vec2_add(vec2_t v1, vec2_t v2)
{
    vec2_t result = (vec2_t){ .x = v1.x + v2.x, .y = v1.y + v2.y };

    return result;
}

vec2_t vec2_sub(vec2_t v1, vec2_t v2)
{
    vec2_t result = (vec2_t){ .x = v1.x - v2.x, .y = v1.y - v2.y };

    return result;
}

vec2_t vec2_mul(vec2_t v1, vec2_t v2)
{
    vec2_t result = (vec2_t){ .x = v1.x * v2.x, .y = v1.y * v2.y };

    return result;
}

vec2_t vec2_div(vec2_t v1, vec2_t v2)
{
    vec2_t result = (vec2_t){ .x = v2.x != 0.0f ? v1.x / v2.x : 0.0f, .y = v2.y != 0.0f ? v1.y / v2.y : 0.0f };

    return result;
}

vec2_t vec2_scale(vec2_t v, f32 s)
{
    vec2_t result = (vec2_t){ .x = v.x * s, .y = v.y * s};

    return result;
}

vec2_t vec2_add_val(vec2_t v, f32 s)
{
    vec2_t result = (vec2_t){ .x = v.x + s, .y = v.y + s};

    return result;
}

vec2_t vec2_sub_val(vec2_t v, f32 s)
{
    vec2_t result = (vec2_t){ .x = v.x - s, .y = v.y - s};

    return result;
}

vec2_t vec2_normalize(vec2_t v)
{
    f32 length = v.x * v.x + v.y * v.y;
    if (length == 0.0f) return v;
    vec2_t result = (vec2_t){ .x = v.x / length, .y = v.y / length };

    return result;
}

vec2_t vec2_linear_combination(vec2_t v1, f32 alpha1, vec2_t v2, f32 alpha2)
{
    vec2_t result = (vec2_t){ .x = v1.x * alpha1 + v2.x * alpha2, .y = v1.y * alpha1 + v2.y * alpha2 };

    return result;
}

vec2_t vec2_map(vec2_t v, vec2_t a, vec2_t b, vec2_t target_a, vec2_t target_b)
{
    vec2_t result = { 0 };

    if (target_a.x - a.x == 0.0f)
    {
        if (b.x - a.x == 0.0f)
        {
            result = v;
        }
        else
        {
            f32 s = (target_b.x - target_a.x) / (b.x - a.x);
            result.x = s * v.x;
            result.y = s * v.y;
        }
    }
    else
    {
        f32 m = (target_b.x - b.x) / (target_a.x - a.x);
        f32 b = target_a.x - a.x * m;
        result.x = v.x * m + b;
    }

    if (target_a.y - a.y == 0.0f)
    {
        if (b.y - a.y == 0.0f)
        {
            result = v;
        }
        else
        {
            f32 s = (target_b.y - target_a.y) / (b.y - a.y);
            result.x = s * v.x;
            result.y = s * v.y;
        }
    }
    else
    {
        f32 m = (target_b.y - b.y) / (target_a.y - a.y);
        f32 b = target_a.y - a.y * m;
        result.y = v.y * m + b;
    }

    return result;
}

vec2_t vec2_transform(mat2x2_t m, vec2_t v)
{
    vec2_t result = { 0 };

    result.x = m.a11 * v.x + m.a12 * v.y;
    result.y = m.a21 * v.x + m.a22 * v.y;

    return result;
}

f32 vec2_length(vec2_t v)
{
    f32 length_sqr = v.x * v.x + v.y * v.y;

    return sqrtf(length_sqr);
}

f32 vec2_length_sqr(vec2_t v)
{
    f32 length_sqr = v.x * v.x + v.y * v.y;

    return length_sqr;
}

f32 vec2_distance(vec2_t v1, vec2_t v2)
{
    f32 dist_sqr = (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y); 

    return sqrtf(dist_sqr);
}

f32 vec2_distance_sqr(vec2_t v1, vec2_t v2)
{
    f32 dist_sqr = (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y); 
    
    return dist_sqr;
}

f32 vec2_dot(vec2_t v1, vec2_t v2)
{
    f32 dot = v1.x * v2.x + v1.y * v2.y;

    return dot;
}


vec3_t vec3(f32 x, f32 y, f32 z)
{
    vec3_t result = (vec3_t){ .x = x, .y = y, .z = z };

    return result;
}

vec3_t vec3v(f32 x)
{
    vec3_t result = (vec3_t){ .x = x, .y = x, .z = x };

    return result;
}

vec3_t vec3_add(vec3_t v1, vec3_t v2)
{
    vec3_t result = (vec3_t){ .x = v1.x + v2.x, .y = v1.y + v2.y, .z = v1.z + v2.z };

    return result;
}

vec3_t vec3_sub(vec3_t v1, vec3_t v2)
{
    vec3_t result = (vec3_t){ .x = v1.x - v2.x, .y = v1.y - v2.y, .z = v1.z - v2.z };

    return result;
}

vec3_t vec3_mul(vec3_t v1, vec3_t v2)
{
    vec3_t result = (vec3_t){ .x = v1.x * v2.x, .y = v1.y * v2.y, .z = v1.z * v2.z };

    return result;
}

vec3_t vec3_div(vec3_t v1, vec3_t v2)
{
    vec3_t result = (vec3_t)
    { 
        .x = v2.x != 0.0f ?  v1.x / v2.x : 0.0f, 
        .y = v2.y != 0.0f ?  v1.y / v2.y : 0.0f, 
        .z = v2.z != 0.0f ?  v1.z / v2.z : 0.0f 
    };

    return result;
}

vec3_t vec3_scale(vec3_t v, f32 s)
{
    vec3_t result = (vec3_t){ .x = v.x * s, .y = v.y * s, .z = v.z * s };

    return result;
}

vec3_t vec3_add_val(vec3_t v, f32 s)
{
    vec3_t result = (vec3_t){ .x = v.x + s, .y = v.y + s, .z = v.z + s };

    return result;
}

vec3_t vec3_sub_val(vec3_t v, f32 s)
{
    vec3_t result = (vec3_t){ .x = v.x - s, .y = v.y - s, .z = v.z - s };

    return result;
}

vec3_t vec3_cross(vec3_t v1, vec3_t v2)
{
    vec3_t result = (vec3_t){ 0 };

    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;

    return result;
}

vec3_t vec3_normalize(vec3_t v)
{
    vec3_t result = v;
    
    f32 len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    
    if (len == 0.0f) return result;

    result.x /= len;
    result.y /= len;
    result.z /= len;

    return result;
}

vec3_t vec3_linear_combination(vec3_t v1, f32 alpha1, vec3_t v2, f32 alpha2, vec3_t v3, f32 alpha3)
{
    vec3_t result = (vec3_t){ 0 };

    result.x = v1.x * alpha1 + v2.x * alpha2 + v3.x * alpha3;
    result.y = v1.y * alpha1 + v2.y * alpha2 + v3.y * alpha3;
    result.z = v1.z * alpha1 + v2.z * alpha2 + v3.z * alpha3;

    return result;
}

f32 vec3_length(vec3_t v)
{
    f32 length_sqr = v.x * v.x + v.y * v.y + v.z * v.z;

    return sqrtf(length_sqr);
}

f32 vec3_length_sqr(vec3_t v)
{
    f32 length_sqr = v.x * v.x + v.y * v.y + v.z * v.z;

    return length_sqr;
}

f32 vec3_distance(vec3_t v1, vec3_t v2)
{
    f32 length_sqr = (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z);

    return sqrtf(length_sqr);
}

f32 vec3_distance_sqr(vec3_t v1, vec3_t v2)
{
    f32 length_sqr = (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z);

    return length_sqr;
}

f32 vec3_dot(vec3_t v1, vec3_t v2)
{
    f32 dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

    return dot;
}

vec3_t vec3_up(void)
{
    return vec3(0, 1, 0);
}

vec3_t vec3_down(void)
{
    return vec3(0, -1, 0);
}

vec3_t vec3_right(void)
{
    return vec3(1, 0, 0);
}

vec3_t vec3_left(void)
{
    return vec3(-1, 0, 0);
}

vec3_t vec3_forward(void)
{
    return vec3(0, 0, -1);
}

vec3_t vec3_backward(void)
{
    return vec3(0, 0, 1);
}

vec3_t vec3_transform(mat3x3_t m, vec3_t v)
{
    vec3_t result = { 0 };

    result.x = v.x * m.a11 + v.y * m.a12 + v.z * m.a13;
    result.y = v.x * m.a21 + v.y * m.a22 + v.z * m.a23;
    result.z = v.x * m.a31 + v.y * m.a32 + v.z * m.a33;

    return result;
}

vec4_t vec4(f32 x, f32 y, f32 z, f32 w)
{
    vec4_t result = { 0 };

    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;

    return result;
}
vec4_t vec4v(f32 val)
{
    vec4_t result = { 0 };

    result.x = val;
    result.y = val;
    result.z = val;
    result.w = val;

    return result;
}

vec4_t vec4_from_vec3_1(vec3_t v)
{
    vec4_t result = { 0 };

    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    result.w = 1.0f;

    return result;
}

vec4_t vec4_transform(mat4x4_t m, vec4_t v)
{
    vec4_t result = { 0 };

    result.x = m.a11 * v.x + m.a12 * v.y + m.a13 * v.z + m.a14 * v.w;
    result.y = m.a21 * v.x + m.a22 * v.y + m.a23 * v.z + m.a24 * v.w;
    result.z = m.a31 * v.x + m.a32 * v.y + m.a33 * v.z + m.a34 * v.w;
    result.w = m.a41 * v.x + m.a42 * v.y + m.a43 * v.z + m.a44 * v.w;

    return result;
}

f32 vec4_dot(vec4_t v1, vec4_t v2)
{
    f32 result = 0.0f;

    result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;

    return result;
}

mat2x2_t mat2x2_unit(void)
{
    mat2x2_t result = { 0 };

    result.a11 = 1.0F; result.a12 = 0.0F;
    result.a21 = 0.0F; result.a22 = 1.0F;

    return result;
}

mat2x2_t mat2x2_rotation_matrix(f32 rotation)
{
    mat2x2_t result = { 0 };

    result.a11 = cosf(rotation);
    result.a12 = sinf(rotation);
    result.a21 = -sinf(rotation);
    result.a22 = cosf(rotation);

    return result;
}

#define mat3x3_for_each(m, iter) for ( f32 * iter = (f32 * )&m; iter < (f32 * )&m + 9; iter++)

mat3x3_t mat3x3_unit(void)
{
    mat3x3_t result = { 0 };

    result.a11 = 1.0f;
    result.a22 = 1.0f;
    result.a33 = 1.0f;

    return result;
}

mat3x3_t mat3x3(int val)
{
    mat3x3_t result = { 0 };

    mat3x3_for_each(result, it)
    {
        *it = val;
    }

    return result;
}

mat3x3_t mat3x3_add(mat3x3_t m1, mat3x3_t m2)
{
    mat3x3_t result = { 0 };

    f32 * iter_m1 = (f32 * )&m1;
    f32 * iter_m2 = (f32 * )&m2;
    f32 * iter_res = (f32 * )&result;

    for (size_t i = 0; i < 9; i++)
    {
        iter_res[i] = iter_m1[i] + iter_m2[i];
    }

    return result;
}

mat3x3_t mat3x3_sub(mat3x3_t m1, mat3x3_t m2)
{
    mat3x3_t result = { 0 };

    f32 * iter_m1 = (f32 * )&m1;
    f32 * iter_m2 = (f32 * )&m2;
    f32 * iter_res = (f32 * )&result;

    for (size_t i = 0; i < 9; i++)
    {
        iter_res[i] = iter_m1[i] - iter_m2[i];
    }

    return result;   
}

mat3x3_t mat3x3_mul(mat3x3_t m1, mat3x3_t m2)
{
    mat3x3_t result = { 0 };
    
    result.a11 = m1.a11 * m2.a11 + m1.a12 * m2.a21 + m1.a13 * m2.a31; 
    result.a21 = m1.a21 * m2.a11 + m1.a22 * m2.a21 + m1.a23 * m2.a31; 
    result.a31 = m1.a31 * m2.a11 + m1.a32 * m2.a21 + m1.a33 * m2.a31;

    result.a12 = m1.a11 * m2.a12 + m1.a12 * m2.a22 + m1.a13 * m2.a32; 
    result.a22 = m1.a21 * m2.a12 + m1.a22 * m2.a22 + m1.a23 * m2.a32; 
    result.a32 = m1.a31 * m2.a12 + m1.a32 * m2.a22 + m1.a33 * m2.a32; 

    result.a13 = m1.a11 * m2.a13 + m1.a12 * m2.a23 + m1.a13 * m2.a33; 
    result.a23 = m1.a21 * m2.a13 + m1.a22 * m2.a23 + m1.a23 * m2.a33; 
    result.a33 = m1.a31 * m2.a13 + m1.a32 * m2.a23 + m1.a33 * m2.a33;
    
    return result;
    
}

mat3x3_t mat3x3_scale(mat3x3_t m, f32 val)
{
    mat3x3_t result = m;

    mat3x3_for_each(result, it)
    {
        *it *= val;
    }

    return result;  
}

mat3x3_t mat3x3_add_val(mat3x3_t m, f32 val)
{
    mat3x3_t result = m;

    mat3x3_for_each(result, it)
    {
        *it += val;
    }

    return result; 
}

mat3x3_t mat3x3_sub_val(mat3x3_t m, f32 val)
{
    mat3x3_t result = m;

    mat3x3_for_each(result, it)
    {
        *it -= val;
    }

    return result; 
}

mat3x3_t mat3x3_div_val(mat3x3_t m, f32 val)
{
    mat3x3_t result = m;

    if (val == 0.0f) return m;

    mat3x3_for_each(result, it)
    {
        *it /= val;
    }

    return result; 
}

mat3x3_t mat3x3_rotation_x(f32 angle)
{
    f32 cosa = cosf(angle);
    f32 sina = sinf(angle);

    mat3x3_t rotation = 
    {
        1.0f,  0.0f,  0.0f,
        0.0f,  cosa, -sina,
        0.0f,  sina,  cosa,
    };

    return rotation;
}

mat3x3_t mat3x3_rotation_y(f32 angle)
{
    f32 cosa = cosf(angle);
    f32 sina = sinf(angle);

    mat3x3_t rotation = 
    {
        cosa,  0.0f, -sina,
        0.0f,  1.0f,  0.0f,
        sina,  0.0f,  cosa,
    };

    return rotation;
}

mat3x3_t mat3x3_rotation_z(f32 angle)
{
    f32 cosa = cosf(angle);
    f32 sina = sinf(angle);

    mat3x3_t rotation = 
    {
        cosa, -sina, 0.0f,
        sina,  cosa, 0.0f,
        0.0f,  0.0f, 1.0f,
    };

    return rotation;
}

mat3x3_t mat3x3_rotation_along(vec3_t rotation_axis, f32 angle)
{
    f32 one_m_cosa = 1.0f - cosf(angle);
    f32 sina = sinf(angle);
    f32 rx = rotation_axis.x;
    f32 ry = rotation_axis.y;
    f32 rz = rotation_axis.z;
    f32 rxy = rx * ry;
    f32 rxz = rx * rz;
    f32 ryz = ry * rz;
    f32 rx2 = rx * rx;
    f32 ry2 = ry * ry;
    f32 rz2 = rz * rz;

    // R = I + sin(theta) * K + (1 - cos(theta)) * K^2 (by Rodrigues' rotation formula)

    mat3x3_t rotation = 
    {
        1.0f - one_m_cosa * (rz2 + ry2), -sina * rz + one_m_cosa * rxy  ,  sina * ry + one_m_cosa * rxz  ,
         sina * rz + one_m_cosa * rxy  , 1.0f - one_m_cosa * (rx2 + rz2), -sina * rx + one_m_cosa * ryz  ,
        -sina * ry + one_m_cosa * rxz  ,  sina * rx + one_m_cosa * ryz  , 1.0f - one_m_cosa * (rx2 + ry2),
    };

    return rotation;
}

mat3x3_t mat3x3_translate_matrix(mat3x3_t m, vec2_t translation)
{
    mat3x3_t result = { 0 };

    mat3x3_t translation_matrix = 
    {
        1.0F, 0.0F, translation.x,
        0.0F, 1.0F, translation.y,
        0.0F, 0.0F, 1.0F,
    };

    result = mat3x3_mul(m, translation_matrix);

    return result;
}

mat3x3_t mat3x3_rotate_matrix(mat3x3_t m, f32 rotation)
{
    mat3x3_t result = { 0 };

    mat3x3_t rotation_matrix = 
    {
         cosf(rotation), sinf(rotation), 0.0F,
        -sinf(rotation), cosf(rotation), 0.0F,
         0.0F          , 0.0F          , 1.0F
    };

    result = mat3x3_mul(m, rotation_matrix);

    return result;
}

mat3x3_t mat3x3_scale_matrix(mat3x3_t m, vec2_t scale)
{
    mat3x3_t result = { 0 };

    mat3x3_t scale_matrix = 
    {
        scale.x, 0.0F   , 0.0F,
        0.0F   , scale.y, 0.0F,
        0.0F   , 0.0F   , 1.0F
    };

    result = mat3x3_mul(m, scale_matrix);

    return result;
}

mat3x3_t mat3x3_orthogonal_projection(f32 w, f32 h)
{
    mat3x3_t result = { 0 };

    result.a11 = 2.0F / w; result.a12 = 0.0F    ; result.a13 = 1.0F;
    result.a11 = 0.0F    ; result.a12 = 2.0F / h; result.a13 = 1.0F;
    result.a11 = 0.0F    ; result.a12 = 0.0F    ; result.a13 = 1.0F;

    return result;
}

#define mat4x4_foreach(m,it) for( f32 * ptr = (f32* )&(m), it = * ptr; ptr < (f32* )&(m) + 16; *ptr = it, ptr++, it = * ptr )
#define mat4x4_foreach_ptr(m, iter) for ( f32 * iter = (f32 * )&m; iter < (f32 * )&m + 16; iter++)

mat4x4_t mat4x4_unit(void)
{
    mat4x4_t result = { 0 };    

    result.a11 = 1.0f;
    result.a22 = 1.0f;
    result.a33 = 1.0f;
    result.a44 = 1.0f;

    return result;
}

mat4x4_t mat4x4(f32 val)
{
    mat4x4_t result = { 0 };

    mat4x4_foreach(result, it)
    {
        it = val;
    }

    return result;
}

mat4x4_t mat4x4_add(mat4x4_t m1, mat4x4_t m2)
{
    mat4x4_t result = { 0 };

    f32 * iter_m1  = (f32 * )&m1;
    f32 * iter_m2  = (f32 * )&m2;
    f32 * iter_res = (f32 * )&result;
    
    for (size_t i = 0; i < 16; i++)
    {
        iter_res[i] = iter_m1[i] + iter_m2[i];
    }

    return result;
}

mat4x4_t mat4x4_sub(mat4x4_t m1, mat4x4_t m2)
{
    mat4x4_t result = { 0 };
    
    f32 * iter_m1  = (f32 * )&m1;
    f32 * iter_m2  = (f32 * )&m2;
    f32 * iter_res = (f32 * )&result;
    
    for (size_t i = 0; i < 16; i++)
    {
        iter_res[i] = iter_m1[i] - iter_m2[i];
    }
    
    return result;
}

mat4x4_t mat4x4_mul(mat4x4_t m1, mat4x4_t m2)
{
    mat4x4_t result = { 0 };
    
    // result.a11 = vec4_dot(mat4x4_get_row(m1, 1), mat4x4_get_col(m2, 1));
    result.a11 = m1.a11 * m2.a11 + m1.a12 * m2.a21 + m1.a13 * m2.a31 + m1.a14 * m2.a41; 
    // result.a21 = vec4_dot(mat4x4_get_row(m1, 2), mat4x4_get_col(m2, 1));
    result.a21 = m1.a21 * m2.a11 + m1.a22 * m2.a21 + m1.a23 * m2.a31 + m1.a24 * m2.a41; 
    // result.a31 = vec4_dot(mat4x4_get_row(m1, 3), mat4x4_get_col(m2, 1));
    result.a31 = m1.a31 * m2.a11 + m1.a32 * m2.a21 + m1.a33 * m2.a31 + m1.a34 * m2.a41; 
    // result.a41 = vec4_dot(mat4x4_get_row(m1, 4), mat4x4_get_col(m2, 1));
    result.a41 = m1.a41 * m2.a11 + m1.a42 * m2.a21 + m1.a43 * m2.a31 + m1.a44 * m2.a41; 
    
    // result.a12 = vec4_dot(mat4x4_get_row(m1, 1), mat4x4_get_col(m2, 2));
    result.a12 = m1.a11 * m2.a12 + m1.a12 * m2.a22 + m1.a13 * m2.a32 + m1.a14 * m2.a42; 
    // result.a22 = vec4_dot(mat4x4_get_row(m1, 2), mat4x4_get_col(m2, 2));
    result.a22 = m1.a21 * m2.a12 + m1.a22 * m2.a22 + m1.a23 * m2.a32 + m1.a24 * m2.a42; 
    // result.a32 = vec4_dot(mat4x4_get_row(m1, 3), mat4x4_get_col(m2, 2));
    result.a32 = m1.a31 * m2.a12 + m1.a32 * m2.a22 + m1.a33 * m2.a32 + m1.a34 * m2.a42; 
    // result.a42 = vec4_dot(mat4x4_get_row(m1, 4), mat4x4_get_col(m2, 2));
    result.a42 = m1.a41 * m2.a12 + m1.a42 * m2.a22 + m1.a43 * m2.a32 + m1.a44 * m2.a42; 
    
    // result.a13 = vec4_dot(mat4x4_get_row(m1, 1), mat4x4_get_col(m2, 3));
    result.a13 = m1.a11 * m2.a13 + m1.a12 * m2.a23 + m1.a13 * m2.a33 + m1.a14 * m2.a43; 
    // result.a23 = vec4_dot(mat4x4_get_row(m1, 2), mat4x4_get_col(m2, 3));
    result.a23 = m1.a21 * m2.a13 + m1.a22 * m2.a23 + m1.a23 * m2.a33 + m1.a24 * m2.a43; 
    // result.a33 = vec4_dot(mat4x4_get_row(m1, 3), mat4x4_get_col(m2, 3));
    result.a33 = m1.a31 * m2.a13 + m1.a32 * m2.a23 + m1.a33 * m2.a33 + m1.a34 * m2.a43; 
    // result.a43 = vec4_dot(mat4x4_get_row(m1, 4), mat4x4_get_col(m2, 3));
    result.a43 = m1.a41 * m2.a13 + m1.a42 * m2.a23 + m1.a43 * m2.a33 + m1.a44 * m2.a43; 
    
    // result.a14 = vec4_dot(mat4x4_get_row(m1, 1), mat4x4_get_col(m2, 4));
    result.a14 = m1.a11 * m2.a14 + m1.a12 * m2.a24 + m1.a13 * m2.a34 + m1.a14 * m2.a44; 
    // result.a24 = vec4_dot(mat4x4_get_row(m1, 2), mat4x4_get_col(m2, 4));
    result.a24 = m1.a21 * m2.a14 + m1.a22 * m2.a24 + m1.a23 * m2.a34 + m1.a24 * m2.a44; 
    // result.a34 = vec4_dot(mat4x4_get_row(m1, 3), mat4x4_get_col(m2, 4));
    result.a34 = m1.a31 * m2.a14 + m1.a32 * m2.a24 + m1.a33 * m2.a34 + m1.a34 * m2.a44; 
    // result.a44 = vec4_dot(mat4x4_get_row(m1, 4), mat4x4_get_col(m2, 4));
    result.a44 = m1.a41 * m2.a14 + m1.a42 * m2.a24 + m1.a43 * m2.a34 + m1.a44 * m2.a44;
    
    return result;
}

mat4x4_t mat4x4_scale(mat4x4_t m, f32 s)
{
    mat4x4_t result = m;

    mat4x4_foreach(result, it)
    {
        it *= s;
    }

    return result;
}

mat4x4_t mat4x4_add_val(mat4x4_t m, f32 val)
{
    mat4x4_t result = m;

    mat4x4_foreach(result, it)
    {
        it += val;
    }

    return result;
}

mat4x4_t mat4x4_sub_val(mat4x4_t m, f32 val)
{
    mat4x4_t result = m;

    mat4x4_foreach(result, it)
    {
        it -= val;
    }

    return result;
}

mat4x4_t mat4x4_div_val(mat4x4_t m, f32 val)
{
    mat4x4_t result = m;

    if (val == 0.0f) return m;

    mat4x4_foreach(result, it)
    {
        it /= val;
    }

    return result;
}

mat4x4_t mat4x4_scale_matrix(mat4x4_t m, vec3_t scale)
{
    mat4x4_t result = m;

    result.a11 *= scale.x;
    result.a22 *= scale.y;
    result.a33 *= scale.z;

    return result;
}

mat4x4_t mat4x4_translate_matrix(mat4x4_t m, vec3_t translation)
{
    mat4x4_t result = { 0 };
    mat4x4_t T = 
    {
        1.0f, 0.0f, 0.0f, translation.x,
        0.0f, 1.0f, 0.0f, translation.y,
        0.0f, 0.0f, 1.0f, translation.z,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    result = mat4x4_mul(m, T);

    return result;
}

mat4x4_t mat4x4_rotate_matrix(mat4x4_t m, vec3_t rotation_axis, f32 angle)
{
    mat4x4_t result = { 0 };

    const f32 a = angle;
    const f32 c = cosf(a);
    const f32 s = sinf(a);

    vec3_t axis = vec3_normalize(rotation_axis);
    vec3_t temp = vec3_scale(axis, 1.0f - c);

    mat4x4_t rotate = { 0 };

    rotate.a11 = c + temp.x * axis.x;
    rotate.a21 = temp.x * axis.y + s * axis.z;
    rotate.a31 = temp.x * axis.z - s * axis.y;

    rotate.a12 = temp.y * axis.x - s * axis.z;
    rotate.a22 = c + temp.y * axis.y;
    rotate.a32 = temp.y * axis.z - s * axis.x;
    
    rotate.a13 = temp.z * axis.x + s * axis.y;
    rotate.a23 = temp.z * axis.y - s * axis.x;
    rotate.a33 = c + temp.z * axis.z;

    result.a11 = m.a11 * rotate.a11 + m.a12 * rotate.a21 + m.a13 * rotate.a31;
    result.a21 = m.a21 * rotate.a11 + m.a22 * rotate.a21 + m.a23 * rotate.a31;
    result.a31 = m.a31 * rotate.a11 + m.a32 * rotate.a21 + m.a33 * rotate.a31;
    result.a41 = m.a41 * rotate.a11 + m.a42 * rotate.a21 + m.a43 * rotate.a31;
    
    result.a12 = m.a11 * rotate.a12 + m.a12 * rotate.a22 + m.a13 * rotate.a32;
    result.a22 = m.a21 * rotate.a12 + m.a22 * rotate.a22 + m.a23 * rotate.a32;
    result.a32 = m.a31 * rotate.a12 + m.a32 * rotate.a22 + m.a33 * rotate.a32;
    result.a42 = m.a41 * rotate.a12 + m.a42 * rotate.a22 + m.a43 * rotate.a32;
    
    result.a13 = m.a11 * rotate.a13 + m.a12 * rotate.a23 + m.a13 * rotate.a33;
    result.a23 = m.a21 * rotate.a13 + m.a22 * rotate.a23 + m.a23 * rotate.a33;
    result.a33 = m.a31 * rotate.a13 + m.a32 * rotate.a23 + m.a33 * rotate.a33;
    result.a43 = m.a41 * rotate.a13 + m.a42 * rotate.a23 + m.a43 * rotate.a33;
    
    result.a14 = m.a14;
    result.a24 = m.a24;
    result.a34 = m.a34;
    result.a44 = m.a44;

    return result; 
}

mat4x4_t mat4x4_look_at_matrix(vec3_t position, vec3_t target, vec3_t up)
{
    mat4x4_t result = { 0 };
    
    vec3_t D = vec3_normalize(vec3_sub(target, position));
    vec3_t R = vec3_normalize(vec3_cross(D, up));
    vec3_t U = vec3_cross(R, D);
    vec3_t P = position;

    result = (mat4x4_t)
    {
         R.x , R.y , R.z , -vec3_dot(R, P),
         U.x , U.y , U.z , -vec3_dot(U, P),
        -D.x , -D.y , -D.z , vec3_dot(D ,P),
         0.0f, 0.0f, 0.0f, 1.0f,   
    };   

    return result;
}

mat4x4_t mat4x4_perspective_matrix(f32 fovy, f32 aspect, f32 near, f32 far)
{
    mat4x4_t result = { 0 };

    f32 t     = tanf(0.5f * fovy);

    result.a11 =1.0f / (t * aspect) ;
    result.a22 = 1.0f / t;
    result.a33 = -(far + near) / (far - near);
    result.a43 = -1.0f;
    result.a34 = -2.0f * far * near / (far - near);

    return result;
}

mat4x4_t mat4x4_transpose(mat4x4_t m)
{
    mat4x4_t result = { 0 };

    result.a11 = m.a11;
    result.a12 = m.a21;
    result.a13 = m.a31;
    result.a14 = m.a41;
    
    
    result.a21 = m.a12;
    result.a22 = m.a22;
    result.a23 = m.a32;
    result.a24 = m.a42;

    result.a31 = m.a13;
    result.a32 = m.a23;
    result.a33 = m.a33;
    result.a34 = m.a43;
    
    result.a41 = m.a14;
    result.a42 = m.a24;
    result.a43 = m.a34;
    result.a44 = m.a44;

    return result;
}

mat4x4_t mat4x4_inverse(mat4x4_t m)
{
    mat4x4_t result = { 0 };

    f32 b00 = m.a11 * m.a22 - m.a12 * m.a21;
    f32 b01 = m.a11 * m.a23 - m.a13 * m.a21;
    f32 b02 = m.a11 * m.a24 - m.a14 * m.a21;
    f32 b03 = m.a12 * m.a23 - m.a13 * m.a22;
    f32 b04 = m.a12 * m.a24 - m.a14 * m.a22;
    f32 b05 = m.a13 * m.a24 - m.a14 * m.a23;
    f32 b06 = m.a31 * m.a42 - m.a32 * m.a41;
    f32 b07 = m.a31 * m.a43 - m.a33 * m.a41;
    f32 b08 = m.a31 * m.a44 - m.a34 * m.a41;
    f32 b09 = m.a32 * m.a43 - m.a33 * m.a42;
    f32 b10 = m.a32 * m.a44 - m.a34 * m.a42;
    f32 b11 = m.a33 * m.a44 - m.a34 * m.a43;

    // Calculate the invert determinant (inlined to avoid double-caching)
    f32 invDet = 1.0f / ( b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);

    result.a11 = ( m.a22 * b11 - m.a23 * b10 + m.a24 * b09) * invDet;
    result.a12 = (-m.a21 * b11 + m.a23 * b08 - m.a24 * b07) * invDet;
    result.a13 = ( m.a21 * b10 - m.a22 * b08 + m.a24 * b06) * invDet;
    result.a14 = (-m.a21 * b09 + m.a22 * b07 - m.a23 * b06) * invDet;
    result.a21 = (-m.a12 * b11 + m.a13 * b10 - m.a14 * b09) * invDet;
    result.a22 = ( m.a11 * b11 - m.a13 * b08 + m.a14 * b07) * invDet;
    result.a23 = (-m.a11 * b10 + m.a12 * b08 - m.a14 * b06) * invDet;
    result.a24 = ( m.a11 * b09 - m.a12 * b07 + m.a13 * b06) * invDet;
    result.a31 = ( m.a42 * b05 - m.a43 * b04 + m.a44 * b03) * invDet;
    result.a32 = (-m.a41 * b05 + m.a43 * b02 - m.a44 * b01) * invDet;
    result.a33 = ( m.a41 * b04 - m.a42 * b02 + m.a44 * b00) * invDet;
    result.a34 = (-m.a41 * b03 + m.a42 * b01 - m.a43 * b00) * invDet;
    result.a41 = (-m.a32 * b05 + m.a33 * b04 - m.a34 * b03) * invDet;
    result.a42 = ( m.a31 * b05 - m.a33 * b02 + m.a34 * b01) * invDet;
    result.a43 = (-m.a31 * b04 + m.a32 * b02 - m.a34 * b00) * invDet;
    result.a44 = ( m.a31 * b03 - m.a32 * b01 + m.a33 * b00) * invDet;

    return result;
}

mat4x4_t mat4x4_orthogonal(f32 left, f32 right, f32 bottom, f32 top, f32 far, f32 near)
{
    mat4x4_t result = { 0 };

    result = (mat4x4_t)
    {
        2.0F / (right - left), 0.0F                 , 0.0F                , -(right + left  ) / (right - left  ),
        0.0F                 , 2.0F / (top - bottom), 0.0F                , -(top   + bottom) / (top   - bottom),
        0.0F                 , 0.0F                 , -2.0F / (far - near), -(far   + near  ) / (far   - near  ),
        0.0F                 , 0.0F                 , 0.0F                , 1.0F                                
    };

    return result;
}

vec4_t mat4x4_get_row(mat4x4_t m, int row_index)
{
    vec4_t result = { 0 };

    if (row_index == 1)
    {
        result.x = m.a11;
        result.y = m.a12;
        result.z = m.a13;
        result.w = m.a14;
    }
    else if (row_index == 2)
    {
        result.x = m.a21;
        result.y = m.a22;
        result.z = m.a23;
        result.w = m.a24; 
    }
    else if (row_index == 3)
    {
        result.x = m.a31;
        result.y = m.a32;
        result.z = m.a33;
        result.w = m.a34;
    }
    else if (row_index == 4)
    {
        result.x = m.a41;
        result.y = m.a42;
        result.z = m.a43;
        result.w = m.a44;
    }

    return result;
}

vec4_t mat4x4_get_col(mat4x4_t m, int col_index)
{
    vec4_t result = { 0 };

    if (col_index == 1)
    {
        result.x = m.a11;
        result.y = m.a21;
        result.z = m.a31;
        result.w = m.a41;
    }
    else if (col_index == 2)
    {
        result.x = m.a12;
        result.y = m.a22;
        result.z = m.a32;
        result.w = m.a42; 
    }
    else if (col_index == 3)
    {
        result.x = m.a13;
        result.y = m.a23;
        result.z = m.a33;
        result.w = m.a43;
    }
    else if (col_index == 4)
    {
        result.x = m.a14;
        result.y = m.a24;
        result.z = m.a34;
        result.w = m.a44;
    }

    return result;   
}

f32 * mat4x4_to_f32_array(mat4x4_t m, f32 array[16])
{
    f32 * iter = (f32 * )&m;
    for (size_t i = 0; i < 16; i++)
    {
        array[i] = iter[i];
    }
    return array;
}

f32 mat4x4_trace(mat4x4_t m)
{
    f32 trace = m.a11 + m.a22 + m.a33 + m.a44;

    return trace;
}

rectangle_t rectangle(f32 x, f32 y, f32 w, f32 h, f32 r)
{
    rectangle_t result = { 0 };

    result.x = x;
    result.y = y;
    result.w = w;
    result.h = h;
    result.r = r;

    return result;
}

rectangle_t rectangle_get_padded_rect(rectangle_t src, f32 padding)
{
    rectangle_t result = { 0 };

    result.x = src.x - padding;
    result.y = src.y - padding;
    result.w = src.w + 2 * padding;
    result.h = src.h + 2 * padding;
    result.r = src.r;

    return result;
}

rectangle_t rectangle_scale_around_top_left(rectangle_t src, vec2_t scale)
{
    rectangle_t result = { 0 };
    
    result.x = src.x;
    result.y = src.y;
    result.w = src.w * scale.x;
    result.h = src.h * scale.y;
    result.r = src.r;
    
    return result;
}

rectangle_t rectangle_scale_around_center(rectangle_t src, vec2_t scale)
{
    rectangle_t result = { 0 };
    
    result.w = scale.x * src.w;
    result.h = scale.y * src.h;

    result.x = src.x - (result.w - src.w);
    result.y = src.y - (result.h - src.h);
    
    result.r = src.r;
    
    return result;
}

bool rectangle_is_point_inside(rectangle_t rect, vec2_t p)
{
    vec2_t center = vec2(rect.x + rect.w / 2, rect.y + rect.h / 2);
    vec2_t t = p;
    t = vec2_sub(t, center);
    vec2_t cis = vec2(cosf(-rect.r), sinf(-rect.r));
    t = vec2(t.x * cis.x - t.y * cis.y, t.x * cis.y + t.y * cis.x);
    t = vec2_add(t, center);

    return t.x <= rect.x + rect.w && t.x >= rect.x && t.y <= rect.y + rect.h && t.y >= rect.y;
}

transform2D_s transform2D(vec2_t position, vec2_t scale, f32 rotation)
{
    transform2D_s result = { 0 };

    result.position = position;
    result.scale    = scale;
    result.rotation = rotation;

    return result;
}
