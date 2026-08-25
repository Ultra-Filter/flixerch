#version 330 core

out vec4 frag_color;

#define GRADIENT_TYPE_NONE                      0.0
#define GRADIENT_TYPE_LINEAR_HORIZONTAL         1.0
#define GRADIENT_TYPE_LINEAR_VERTICAL           2.0
#define GRADIENT_TYPE_LINEAR_IDENTITY           3.0
#define GRADIENT_TYPE_LINEAR_IDENTITY_REVERSE   4.0
#define GRADIENT_TYPE_LINEAR_CIRCULAR           5.0
#define GRADIENT_TYPE_LINEAR_HYPERBOLA          6.0
#define GRADIENT_TYPE_LINEAR_HYPERBOLA_REVERSE  7.0

in vec2 frag_coords;
in vec3 uvw;
in vec4 tint1;
in vec4 tint2;
in vec4 tint3;
in vec4 border_col;
in vec4 border_rad;
in float border_thick;
in vec4 shadow_col;
in vec2 shadow_off;
in float shadow_blur;
in float shadow_spread;
in vec2 image_scale;
in float grad_type;
in float tint_strength;

const float paddings = 200.0;

uniform sampler2D texs[ 32 ];



float avg(float x, float y)
{
    return (x + y) * 0.5;
}

vec4 gradient_horizontal(vec2 fg, vec4 bg1, vec4 bg2, vec4 bg3)
{
    float factor = fg.x;
    factor = clamp(0.0, 1.0, factor);
    factor = smoothstep(0.0, 1.0, factor);
    return mix(mix(bg1, bg2, 2.0 * factor), bg3, factor * 2.0 - 1.0); 
}

vec4 gradient_vertical(vec2 fg, vec4 bg1, vec4 bg2, vec4 bg3)
{
    float factor = fg.y;
    factor = clamp(0.0, 1.0, factor);
    factor = smoothstep(0.0, 1.0, factor);
    return mix(mix(bg1, bg2, 2.0 * factor), bg3, factor * 2.0 - 1.0);
}

vec4 gradient_circular(vec2 fg, vec4 bg1, vec4 bg2, vec4 bg3)
{

    float factor = smoothstep(0.2, 1.5, 2.0 * length(fg - vec2(0.5)));
    return mix(mix(bg1, bg2, 2.0 * factor), bg3, factor * 2.0 - 1.0);
}

vec4 grad_circ(float dist, vec4 bg1, vec4 bg2, vec4 bg3)
{
    float d = clamp(-dist, 0.0, 1.0);
    d = smoothstep(0.0, 1.0, d);
    return d < 0.5 ? mix(bg1, bg2, d * 2.0) : mix(bg2, bg3, (d * 2.0 - 1.0));
}

vec4 gradient_identity(vec2 fg, vec4 bg1, vec4 bg2, vec4 bg3)
{
    float factor = fg.x - fg.y + 0.5;
    return mix(mix(bg1, bg2, 2.0 * factor), bg3, factor * 2.0 - 1.0); 
}

vec4 gradient_identity_reverse(vec2 fg, vec4 bg1, vec4 bg2, vec4 bg3)
{

    float factor = -1.0 + fg.x + fg.y;
    return mix(mix(bg1, bg2, 2.0 * factor), bg3, factor * 2.0 - 1.0); 
}



float sdRoundBox( in vec2 p, in vec2 b, in vec4 r ) 
{
    p.x = p.x > 0 ? pow(p.x, image_scale.x / image_scale.y) : - pow(p.x, image_scale.x / image_scale.y);
    r.xy = (p.x>0.0)?r.xy : r.zw;
    r.x  = (p.y>0.0)?r.x  : r.y;
    vec2 q = abs(p)-b+r.x;
    return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r.x;
}

void main()
{
    vec2 norm_fg = 2.0 * (frag_coords - vec2(paddings)) / (image_scale - vec2(2.0 * paddings)) - 1.0;
    vec2 pd_sc = vec2(paddings) / image_scale;
    vec2 norm_uv = ((uvw.xy - pd_sc)/ (1.0 - 2.0 * pd_sc) );

    float include_dist = sdRoundBox(norm_fg, vec2(1.0), border_rad);
    vec4 tex_res = texture(texs[int(uvw.z)],norm_uv.xy);

    vec4 bg1 = mix(tex_res, tex_res * tint1, tint_strength); 
    vec4 bg2 = mix(tex_res, tex_res * tint2, tint_strength);
    vec4 bg3 = mix(tex_res, tex_res * tint3, tint_strength);

    vec4 bg = grad_type == GRADIENT_TYPE_NONE ? bg1 : 
              grad_type == GRADIENT_TYPE_LINEAR_HORIZONTAL ? gradient_horizontal              ((1.0 + norm_fg) / 2.0, bg1, bg2, bg3) :
              grad_type == GRADIENT_TYPE_LINEAR_VERTICAL   ? gradient_vertical                ((1.0 + norm_fg) / 2.0, bg1, bg2, bg3) :
              grad_type == GRADIENT_TYPE_LINEAR_CIRCULAR   ? grad_circ                        (include_dist         , bg1, bg2, bg3) :
              grad_type == GRADIENT_TYPE_LINEAR_IDENTITY   ? gradient_identity                ((1.0 + norm_fg) / 2.0, bg1, bg2, bg3) :
              grad_type == GRADIENT_TYPE_LINEAR_IDENTITY_REVERSE   ? gradient_identity_reverse((1.0 + norm_fg) / 2.0, bg1, bg2, bg3) :
              vec4(1.0);
            //   grad_type == GRADIENT_TYPE_LINEAR_CIRCULAR   ? gradient_circular((1.0 + norm_fg) / 2.0) :

    float der = fwidth(include_dist);

    vec2 so = shadow_off;
    float shadow_alpha = 1.0 - sdRoundBox(norm_fg - so, vec2(1.0 + 2.0 * shadow_spread), border_rad);
    float sb = shadow_blur * (frag_coords / image_scale).x;
    shadow_alpha = smoothstep(-sb,sb,shadow_alpha);
    shadow_alpha *= 1.0 - smoothstep(-shadow_spread, shadow_spread, include_dist);

    float quad_alpha = 1.0 - smoothstep(-der,der,include_dist);

    float bt = avg(border_thick / (image_scale.x - 2.0 * paddings), border_thick / (image_scale.y - 2.0 * paddings));
    float border_alpha = sdRoundBox(norm_fg, vec2(1.0) - bt, border_rad);
    float bw = fwidth(border_alpha);

    border_alpha = smoothstep(-bw,bw,border_alpha); 
    border_alpha *= quad_alpha;

    vec4 col = mix(vec4(shadow_col.xyz, 0.0), shadow_col, shadow_alpha);
    col = mix(col, bg, quad_alpha);
    col = mix(col, border_col, border_alpha);

    if (col.a < 0.001) discard;

    frag_color =  col;
}
