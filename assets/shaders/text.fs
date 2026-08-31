#version 330 core

out vec4 frag_color;

in vec3  uvw;
in vec4 color1;
in vec4 color2;
in vec4 color3;
in float grad_type;
in vec4 outline_color;
in float outline_thickness;
in vec4 shadow_color;
in vec2 shadow_blur_spread;
in vec2 text_quad_scale;
in vec2 frag_coords;

const float paddings = 200.0;

uniform sampler2D texs[32];

void main()
{
    vec2 norm_fg = 2.0 * (frag_coords - vec2(paddings)) / (text_quad_scale - vec2(2.0 * paddings)) - 1.0;
    vec2 pd_sc = vec2(paddings) / text_quad_scale;
    vec2 norm_uv = ((uvw.xy - pd_sc)/ (1.0 - 2.0 * pd_sc) );
    vec4 col = texture(texs[int(uvw.z)], uvw.xy);
    frag_color = vec4(color1.xyz, col.r);
}
