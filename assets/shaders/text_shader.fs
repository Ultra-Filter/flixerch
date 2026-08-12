#version 330 core

out vec4 frag_color;

in vec4 text_color;
in vec2 uv;
flat in uint texture_index;

// uniform sampler2DArray tex_arr;
uniform sampler2D tex;

void main()
{
    float a = texture(tex, uv).r;
    frag_color = vec4(text_color.xyz, a);
}
