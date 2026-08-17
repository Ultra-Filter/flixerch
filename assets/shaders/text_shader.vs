#version 330 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec4 in_text_color;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in uint in_texture_index;

out vec4 text_color;
out vec2 uv;
flat out uint texture_index;

void main()
{
    gl_Position = vec4(in_position, 0.0, 1.0);

    text_color    = in_text_color;
    uv            = in_uv;
    texture_index = in_texture_index;
}