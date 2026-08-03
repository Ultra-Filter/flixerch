#include "window.h"

static void initiate(void);
static void mainloop(void);
static void shutdown(void);

int main (void)
{
    initiate();
    mainloop();
    shutdown();

    return 0;
}

static void initiate(void)
{
    if (!window_init(1920, 720, "Hello"))
    {
        exit(EXIT_FAILURE);
    }
}

static void mainloop(void)
{
    while (!window_should_close())
    {   
        window_clear_screen();

        if (is_key_down(KEY_U))
        {
            fprintf(stdout, "U pressed the 'U' key\n");\
        }
        if (is_key_pressed(KEY_S))
        {
            fprintf(stdout, "U pressed the 'S' key\n");\
        }

        window_update();
    }
}

static void shutdown(void)
{
    window_destroy();
}