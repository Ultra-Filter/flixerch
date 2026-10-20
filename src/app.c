#include "frontend/renderer/window.h"
#include "frontend/ui/ui.h"

int main(void)
{
    window_init(1200, 800, "Flixerch");
    window_prepare();
    ui_init();

    while(!window_should_close())
    {
        window_clear_screen(COLOR_GRUVBOX_DARK_SMOKY_CHARCOAL);
        input_update();
        if (is_key_pressed(KEY_ESCAPE))
        {
            window_close();
        }

        ui_end_frame();
        window_swap_buffers();
    }

    ui_shutdown();
    window_destroy();
}
