#if !defined(DYNAMIC_UI_LIB) 
#define UI_LIB_PATH "build/shared/main.dll"


#include "frontend/renderer/font.h"
#include "frontend/renderer/window.h"
#define UI_DECL_IMPLEMENTATION
#include "frontend/ui/ui_decl.h"
#include "frontend/ui/ui_interface.h"
#include "core/utils/ddy_lib_loader.h"
static rgba_t BACKGROUND_COLOR  = {211, 134, 155, 255};

#include <windows.h>
static SYSTEMTIME last_time = { 0 };
static ddy_lib_loader_t lib_loader;
HANDLE event = INVALID_HANDLE_VALUE;

typedef void (*_elem_start)(UI_type, const char *, UI_panel_style, UI_childs_style);
typedef void (*_elem_end)(void);
typedef void (*_page_start)(const char*);
typedef void (*_page_end)(void);
typedef void (*_page_render)(const char*);
typedef void (*_bg_col)(rgba_t);

void (*dll_init)(_elem_start, _elem_end, _page_start, _page_end, _page_render, _bg_col) = NULL;
void (*dll_update)(void) = NULL;
 


void element_start(UI_type type, const char* name, UI_panel_style style, UI_childs_style childs_style)
{
    ui_element_begin(type, name, style, childs_style);
}

void element_end()
{
    ui_element_end();
}

void page_start(const char* name)
{
    ui_page_begin(name);
}

void page_end()
{
    ui_page_end();
}

void page_render(const char* name)
{
    ui_render(name);
}

void set_bg_col(rgba_t col)
{
    BACKGROUND_COLOR = col;
}

static void init_lib(void)
{
    event = CreateEventA(NULL, FALSE, FALSE, "Local\\UPDATE_LIB");
    if (event == INVALID_HANDLE_VALUE || !event)
    {
        printf("somthing went wrong creating the update lib event.\n");
        return EXIT_FAILURE;
    }
    lib_loader = ddy_lib_loader_create();
    if (!ddy_lib_loader_load_copy_lib(&lib_loader, UI_LIB_PATH, "UI_LIB_COPY.dll"))
    {
        printf("Could not load library '"UI_LIB_PATH"' is it exists?\n");
        exit(1);
    }
    else
    {
        printf("Dynamic library loaded successfully!.\n");
    }

    void* f;
    if (!ddy_lib_loader_load_sym(lib_loader, "dll_init", &f))
    {
        printf("Could not load symbol '%s' from library '%s'\n", "dll_init", "UI_LIB_COPY.dll");
        exit(1);
    }
    *((void**)(&dll_init)) = f;
    if (!ddy_lib_loader_load_sym(lib_loader, "dll_update", &f))
    {
        printf("Could not load symbol '%s' from library '%s'\n", "dll_update", "UI_LIB_COPY.dll");
        exit(1);
    }
    *((void**)(&dll_update)) = f;
    
    WIN32_FIND_DATAA f_d;
    HANDLE h = FindFirstFileA(UI_LIB_PATH, &f_d);
    FileTimeToSystemTime(&f_d.ftLastWriteTime, &last_time);
    CloseHandle(h);
}

static void update_lib(void)
{
    if (!ddy_lib_loader_reload_copy_lib(&lib_loader, UI_LIB_PATH, "UI_LIB_COPY.dll"))
    {
        printf("Could not load library 'UI_LIB.dll' is it exists?\n");
        exit(1);
    }
    void* f;
    if (!ddy_lib_loader_load_sym(lib_loader, "dll_init", &f))
    {
        printf("Could not load symbol '%s' from library '%s'\n", "dll_init", "UI_LIB_COPY.dll");
        exit(1);
    }
    *((void**)(&dll_init)) = f;
    if (dll_init) dll_init(element_start, element_end, page_start, page_end, page_render, set_bg_col);
    if (!ddy_lib_loader_load_sym(lib_loader, "dll_update", &f))
    {
        printf("Could not load symbol '%s' from library '%s'\n", "dll_update", "UI_LIB_COPY.dll");
        exit(1);
    }
    *((void**)(&dll_update)) = f;
}

static void initiate(void)
{
    if (!window_init(2800, 1200, "FliXerch"))
    {
        exit(EXIT_FAILURE);
    }

    ui_init();
    init_lib();
    if (dll_init) dll_init(element_start, element_end, page_start, page_end, page_render, set_bg_col);
}

static void mainloop(void)
{
    // create_main_page();
    const f64 FPS = 90.0;
    window_prepare();
    f64 time = get_time();
    f64 last_render_time = time;
    
    while (!window_should_close())
    {   
        input_update();
        time_update();
        
        time = get_time();
        
        DWORD wait = WaitForSingleObject(event, 0);

        if (wait == WAIT_OBJECT_0)
        {
            update_lib();
        }
        
        if (time - last_render_time >= 1.0 / FPS)
        {
            window_clear_screen(BACKGROUND_COLOR);
            
            if (dll_update) dll_update();

            // ui_render("main-page");
            window_swap_buffers();
            
            last_render_time = time;
        }
    }
}

static void _shutdown(void)
{
    ui_shutdown();
    window_destroy();
}

int main (void) 
{
    initiate();
    mainloop();
    _shutdown();
}
#endif