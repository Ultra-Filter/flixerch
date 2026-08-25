#version 330 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec3 in_uvw;
layout (location = 2) in vec4 in_color1;
layout (location = 3) in vec4 in_color2;
layout (location = 4) in vec4 in_color3;
layout (location = 5) in float in_grad_type;
layout (location = 6) in vec4 in_outline_color;
layout (location = 7) in float in_outline_thickness;
layout (location = 8) in vec4 in_shadow_color;
layout (location = 9) in vec2 in_shadow_blur_spread;
layout (location = 10) in vec4 in_text_quad_scale_frag_coords;

out vec3  uvw;
out vec4 color1;
out vec4 color2;
out vec4 color3;
out float grad_type;
out vec4 outline_color;
out float outline_thickness;
out vec4 shadow_color;
out vec2 shadow_blur_spread;
out vec2 text_quad_scale;
out vec2 frag_coords;

void main()
{
    gl_Position = vec4(in_position, 0.0, 1.0);
    uvw                = in_uvw;
    color1             = in_color1;
    color2             = in_color2;
    color3             = in_color3;
    grad_type          = in_grad_type;
    outline_color      = in_outline_color;
    outline_thickness  = in_outline_thickness;
    shadow_color       = in_shadow_color;
    shadow_blur_spread = in_shadow_blur_spread;
    text_quad_scale    = in_text_quad_scale_frag_coords.xy;
    frag_coords        = in_text_quad_scale_frag_coords.zw;
}
