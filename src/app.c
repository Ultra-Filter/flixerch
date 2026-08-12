#include "frontend/renderer/color.h"
#include "frontend/renderer/window.h"
#include "frontend/renderer/glad.h"
#include "frontend/renderer/ui_renderer.h"
#include "core/decoders/stb_image.h"

static void initiate(void)
{
    if (!window_init(1280, 800, "FliXerch"))
    {
        exit(EXIT_FAILURE);
    }
    ui_init();
}

static void mainloop(void)
{
    const f64 FPS = 90.0;
    window_prepare();
    f64 time = get_time();
    f64 last_render_time = time;

    while (!window_should_close())
    {   
        input_update();
        time_update();

        time = get_time();
        
        if (time - last_render_time >= 1.0 / FPS)
        {
            window_clear_screen();
            last_render_time = time;

            window_swap_buffers();
        }   
    }
}

static void shutdown(void)
{
    ui_shutdown();
    window_destroy();
}

int main (void) 
{
    initiate();
    mainloop();
    shutdown();
}

