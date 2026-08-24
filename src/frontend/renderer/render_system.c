#include "render_system.h"
#include "quad_renderer.h"
#include "image_renderer.h"

void render_system_init(void)
{
    quad_renderer_init();
    image_renderer_init();
}

void render_system_shutdown(void)
{
    image_renderer_shutdown();
    quad_renderer_shutdown();
}

void render_system_render(void)
{
    quad_renderer_push();
    image_renderer_push();

    for (u8 i = 0; i < Z_INDEX_COUNT; i++)
    {
        quad_renderer_render(i);
        image_renderer_render(i);
    }
}

void render_system_draw(drawable_type_e draw_type, transform2D_s transform, z_index_type_e z_index, ...)
{
    va_list args;
    va_start(args, z_index);

    switch (draw_type)
    {
        case DRAWABLE_TYPE_IMAGE:
        {
            u32 tex_id = va_arg(args, u32);
            image_style_s style = va_arg(args, image_style_s);
            image_renderer_draw(tex_id, transform, style, z_index);
        } break;
        case DRAWABLE_TYPE_QUAD:
        {
            quad_style_s style = va_arg(args, quad_style_s); 
            quad_renderer_draw(transform, style, z_index);
        } break;
        // TODO: case DRAWABLE_TYPE_TEXT: ...
        default: 
            break;
    }

    va_end(args);
}
