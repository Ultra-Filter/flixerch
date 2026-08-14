#version 330 core

out vec4 frag_color;

in vec2 border_radius_thickness;
in vec4 bg_color;
in vec4 border_color;
in vec2 uv;
in vec2 panel_scale;
in vec4 shadow_color;
in vec2 frag_coords;
flat in uint texture_index;

// uniform sampler2DArray texture_array;
uniform sampler2D tex;

float sdRoundRect( in vec2 p, in vec2 b, in float r ) {
    vec2 q = abs(p)-b+r;
    return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r;
}

vec4 colerp(vec4 s, vec4 t, float x)
{
    return s + (t - s) * x;
}

void main()
{
    vec2 fg = frag_coords; 
    vec4 final_color = vec4(1.);

     float dist = length(fg - vec2(.0,.0));
     final_color = vec4(vec3(dist), 1.);

     float sdf_rect = -2.* sdRoundRect(fg - vec2(.5), vec2(0.5), border_radius_thickness.x);
     float bt = border_radius_thickness.y / 10.;

     if (sdf_rect >= 0.)
     {
        if (bt != 0.0)
        {
            final_color = mix(border_color, bg_color, sdf_rect * 1. / bt);
        }
        else
        {
            final_color = bg_color;
        }
     }
     else if (sdf_rect <= 0.) 
     {
        if (bt != 0.0)
        {
            final_color = mix(vec4(0.0), border_color, sdf_rect * 1. / bt);
        }
        else
        {
            final_color = vec4(0.0);
        }
     }

     if (sdf_rect > bt)
     {
         final_color = bg_color;
     }
     else if (sdf_rect < -bt)
     {
         final_color = vec4(0.0);
     }

     // if (fg.x <= paddings / panel_scale.x || fg.y <= paddings / panel_scale.y)
     // {
     //     final_color.a = 0.0;
     // }

    frag_color = final_color; 
}

