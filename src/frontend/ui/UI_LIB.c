#if defined(DYNAMIC_UI_LIB)

#include "ui_interface.h"
#include "frontend/renderer/font.h"
#include "frontend/renderer/window.h"
#define UI_DECL_IMPLEMENTATION
#include "frontend/ui/ui_decl.h"
#include "frontend/ui/ui_interface.h"
#include "core/utils/ddy_lib_loader.h"

typedef void (*_elem_start)(UI_type, const char *, UI_panel_style, UI_childs_style);
typedef void (*_elem_end)(void);
typedef void (*_page_start)(const char*);
typedef void (*_page_end)(void);
typedef void (*_page_render)(const char*);
typedef void (*_bg_col)(rgba_t);

_elem_start  _element_start = NULL;
_elem_end    _element_end   = NULL;
_page_start  _start_page    = NULL;
_page_end    _end_page      = NULL;
_page_render _render_page   = NULL;
_bg_col      _set_bg_col    = NULL;

void dll_update(void)
{
    if (!_element_start || !_element_end || !_start_page || !_end_page || !_render_page || !_set_bg_col) return;

    _set_bg_col(COLOR_WHITE);

    // _start_page("MAIN");

    // _element_start(UI_TYPE_PANEL, "panel", (UI_panel_style){ .style.background_color_left = COLOR_BLUE, .style.gradient_type = GRADIENT_TYPE_NONE, .visible = true }, (UI_childs_style){ 0 });

    // _element_end();
    // _end_page();


    // _render_page("MAIN");
}


void dll_init(_elem_start __elem_start, _elem_end __elem_end, _page_start __page_start, _page_end __page_end, _page_render __page_render, _bg_col __bg_col)
{
    _element_start = __elem_start;
    _element_end   = __elem_end;
    _start_page    = __page_start;
    _end_page      = __page_end;
    _render_page   = __page_render;
    _set_bg_col    = __bg_col;
}

#endif