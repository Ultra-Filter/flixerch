#version 330 core

layout (location = 0) in vec4 position_frag_coords;
layout (location = 1) in vec4 bg_color_left;
layout (location = 2) in vec4 bg_color_mid;
layout (location = 3) in vec4 bg_color_right;
layout (location = 4) in vec4 border_col;
layout (location = 5) in vec4 borders_radii;
layout (location = 6) in float border_thickness;
layout (location = 7) in vec4 shadow_color;
layout (location = 8) in vec4 shadow_offset_blur_spread;
layout (location = 9) in vec3 quad_scale_gradient_type;

out vec2 frag_coords;
out vec4 bg_col1;
out vec4 bg_col2;
out vec4 bg_col3;
out vec4 border_color;
out vec4 border_radii;
out float border_thick;
out vec4 shadow_col;
out vec2 shadow_offset;
out float shadow_blur;
out float shadow_spread;
out vec2 quad_scale;
out float gradient_type;

void main()
{
    gl_Position   = vec4(position_frag_coords.xy, 0.0, 1.0);
    frag_coords   = position_frag_coords.zw;
    bg_col1       = bg_color_left;
    bg_col2       = bg_color_mid;
    bg_col3       = bg_color_right;
    border_color  = border_col;
    border_radii  = borders_radii;
    border_thick  = border_thickness;
    shadow_col    = shadow_color;
    shadow_offset = shadow_offset_blur_spread.xy;
    shadow_blur   = shadow_offset_blur_spread.z;
    shadow_spread = shadow_offset_blur_spread.w;
    quad_scale    = quad_scale_gradient_type.xy;
    gradient_type = quad_scale_gradient_type.z;
}
