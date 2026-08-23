#version 330 core

layout (location = 0) in vec4 position_frag_coords;
layout (location = 1) in vec3 in_uvw;
layout (location = 2) in vec4 tint_color_left;
layout (location = 3) in vec4 tint_color_mid;
layout (location = 4) in vec4 tint_color_right;
layout (location = 5) in vec4 border_color;
layout (location = 6) in vec4 borders_radii;
layout (location = 7) in float border_thickness;
layout (location = 8) in vec4 shadow_color;
layout (location = 9) in vec4 shadow_offset_blur_spread;
layout (location = 10) in vec4 image_scale_gradient_type_tint_strength;

out vec2 frag_coords;
out vec3 uvw;
out vec4 tint1;
out vec4 tint2;
out vec4 tint3;
out vec4 border_col;
out vec4 border_rad;
out float border_thick;
out vec4 shadow_col;
out vec2 shadow_off;
out float shadow_blur;
out float shadow_spread;
out vec2 image_scale;
out float grad_type;
out float tint_strength;

void main()
{
    gl_Position   = vec4(position_frag_coords.xy, 0.0, 1.0);
    frag_coords   = position_frag_coords.zw;
    uvw           = in_uvw;
    tint1         = tint_color_left;
    tint2         = tint_color_mid;
    tint3         = tint_color_right;
    border_col    = border_color;
    border_rad    = borders_radii;
    border_thick  = border_thickness;
    shadow_col    = shadow_color;
    shadow_off    = shadow_offset_blur_spread.xy;
    shadow_blur   = shadow_offset_blur_spread.z;
    shadow_spread = shadow_offset_blur_spread.w;
    image_scale   = image_scale_gradient_type_tint_strength.xy;
    grad_type     = image_scale_gradient_type_tint_strength.z;
    tint_strength = image_scale_gradient_type_tint_strength.w;
}
