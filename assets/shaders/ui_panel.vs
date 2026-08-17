#version 330 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_border_radius_thickness;
layout (location = 2) in vec4 in_bg_color;
layout (location = 3) in vec4 in_border_color;
layout (location = 4) in vec2 in_uv;
layout (location = 5) in vec2 in_panel_scale;
layout (location = 6) in vec4 in_shadow_color;
layout (location = 7) in vec2 in_frag_coords;
layout (location = 8) in uint in_texture_index;

out vec2 border_radius_thickness;
out vec4 bg_color;
out vec4 border_color;
out vec2 uv;
out vec2 panel_scale;
out vec4 shadow_color;
out vec2 frag_coords;
flat out uint texture_index;

uniform vec2 resolution;
uniform mat4 orthogonal_projection_matrix;

void main()
{
    gl_Position = vec4(in_position, 0.0, 1.0);

    border_radius_thickness = in_border_radius_thickness;
    bg_color                = in_bg_color;
    border_color            = in_border_color;
    uv                      = in_uv;
    // texture_index           = in_texture_index;
    panel_scale             = in_panel_scale;
    shadow_color            = in_shadow_color;
    frag_coords             = in_frag_coords;
}