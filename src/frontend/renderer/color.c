#include "color.h"

rgba_t rgb(u8 r, u8 g, u8 b)
{
    rgba_t color = { 0 };
    
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 255U;

    return color;
}

rgba_t rgba(u8 r, u8 g, u8 b, u8 a)
{
    rgba_t color = { 0 };
    
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    
    return color;
}

rgbaf_t rgbaf_from_rgba(rgba_t c)
{
    rgbaf_t color = { 0 };
    
    color.r = (f32)c.r / 255.0F;
    color.g = (f32)c.g / 255.0F;
    color.b = (f32)c.b / 255.0F;
    color.a = (f32)c.a / 255.0F;
    
    return color;
}
