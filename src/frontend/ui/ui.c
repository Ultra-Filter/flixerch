#include "ui_interface.h"
#include "core/utils/containers/dstack.h"
#include "core/utils/containers/dhash_table.h"
#include "core/utils/containers/dlinked_list.h"
#include "core/utils/containers/dqueue.h"
#include "frontend/renderer/render_system.h"
#include "frontend/renderer/quad_renderer.h"
#include "frontend/renderer/window.h"


typedef struct UI_element {
    UI_type type;
    struct ID {
        const char* key;
        u64 hash;
    } ID;
    u32 childs_count;
    UI_childs_style childs_style;
    struct dlinked_list* childs;
    transform2D_s transform;
    sizing_s sizing;
    bool active; 
} UI_element;

typedef struct UI_panel {
    UI_element element;
    UI_panel_style style;
} UI_panel;

typedef struct UI_button {
    UI_element element;
    UI_button_style style;
    void (*on_click)(struct UI_button* btn);
} UI_button;

#define UI_MAX_STACK_DEEP 32
#define UI_INITIAL_CHILDS_ARRAY_COUNT 2

#define UI_MALLOC malloc
#define UI_FREE   free

#define AS_PANEL(e) ((UI_panel*)(e))
#define AS_BUTTON(e) ((UI_button*)(e))

typedef struct page 
{
    const char* name;
    u64 hash;
    UI_element* root;
    dhash_table_t* elements_table;
    f32 ww, wh;
} ui_page;

static struct UI_context
{
    dhash_table_t* pages_table;
    ui_page curr_page;
    dstack_t stack;
    UI_element* head;
} context;

static inline u64 HASH (const char* key, const u32 len)
{
    const u64 offset = 14695981039346656037ULL;
    const u64 prime  = 1099511628211ULL;
    u64 h = offset;
    for (const u8* k = (u8*)key; k < (u8*)key + len; k++)
    {
        h ^= (u64)(*k);
        h *= prime;
    }
    return h;
}

static inline int _compare_child_func_(void*k1, void* k2)
{
    char* s1 = (char*)k1;
    char* s2 = (char*)k2;
    while (s1 && s2 && *s1 == *s2 && s1++ && s2++);
    return *s1 - *s2;
}

static void _calculate_root_layout(UI_element * root)
{
    if (!root->active) return;
    vec4_t pad = root->childs_style.paddings;
    f32 x = root->transform.position.x + pad.z;
    f32 y = root->transform.position.y + pad.x;
    f32 gap = root->childs_style.gap;
    f32 sx, sy;
    f32 max_w = root->transform.scale.x - pad.x - pad.w;
    f32 max_h = root->transform.scale.y - pad.x - pad.y;
    bool horizontal = root->childs_style.direction == UI_CHILDS_ALIGN_HORIZONTAL; 
    if (horizontal)
    {
        max_w = (root->transform.scale.x - pad.z - pad.w - (root->childs_count - 1) * gap) / root->childs_count;
    } 
    else 
    {
        max_h = (root->transform.scale.y - pad.x - pad.y - (root->childs_count - 1) * gap) / root->childs_count;
    }

    dlinked_list_iter it = { 0 };
    UI_element* child_ptr;

    while (dlinked_list_iterator_next(root->childs, &it, &child_ptr))
    {
        if (child_ptr)
        {
            sx = fmaxf(fminf(max_w, child_ptr->sizing.max_w), child_ptr->sizing.min_w);
            sy = fmaxf(fminf(max_h, child_ptr->sizing.max_h), child_ptr->sizing.min_h);
        
            transform2D_s transform = transform2D(
                vec2(x, y),
                vec2(sx, sy),
                0
            );
            child_ptr->transform = transform;
            
            if (horizontal)
            {
                x += sx + gap;
            }
            else
            {
                y += sy + gap;
            }
        }
    }

    it = (dlinked_list_iter){ 0 };
    while (dlinked_list_iterator_next(root->childs, &it, &child_ptr))
    {
        if (child_ptr) _calculate_root_layout(child_ptr);
    }
}

static void _calculate_ui_layout(ui_page page)
{
    f32 ww = (f32)get_window_width();
    f32 wh = (f32)get_window_height();
    if (!page.root) return;
    UI_element* root = page.root;
    root->transform = transform2D(
        vec2(0, 0),
        vec2(ww, wh),
        0
    );
    _calculate_root_layout(root);
}

static void _ui_element_append_child(UI_element* parent, UI_element* child)
{
    if (parent->childs == NULL)
    {
        parent->childs = dlinked_list_create(sizeof(UI_element*), NULL);
    }
    dlinked_list_push_back(parent->childs, &child);
    parent->childs_count++;
}

static inline UI_element* _ui_page_find_element_(ui_page page, const char* name)
{
    UI_element* e = NULL;

    dhash_table_get(page.elements_table, name, &e);

    return e;
}

void ui_page_begin(const char* page_name)
{
    u64 hash = HASH(page_name, strlen(page_name));
    ui_page page;
    if (!dhash_table_get(context.pages_table, page_name, &page))
    {
        page = (ui_page){
            .hash = hash,
            .name = page_name,
            .root = NULL
        };

        dhash_table_add(context.pages_table, page_name, &page);
    }
    context.curr_page = page;
}

void ui_page_end()
{
    if (context.curr_page.name) 
    {
        dhash_table_set(context.pages_table, context.curr_page.name, &context.curr_page);
        _calculate_ui_layout(context.curr_page);
        context.curr_page.ww = (f32)get_window_width();
        context.curr_page.wh = (f32)get_window_height();
    }
    context.curr_page = (ui_page){ 0 };
    dstack_clear(context.stack);
}

void ui_element_begin(UI_type element_type, const char* element_name, ...)
{
    if (!context.curr_page.elements_table)
    {
        context.curr_page.elements_table = dhash_table_create(
            128, 
            sizeof(UI_element*), 
            HASH_TYPE_FNV1A, 
            DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST, 
            NULL
        );
    }

    va_list args;
    va_start(args, element_name);

    if (!context.curr_page.name)
    {
        LOG_ERROR("UI - you can not insert an element without an active page.\n"
                  "\tTry using ui_begin_page(\"page_name\") before calling this function again.\n"
                  "All ui commands will be ignore until you bind a page as active."); 
        return;
    }
    
    UI_element* e;

    if (dhash_table_get(context.curr_page.elements_table, element_name, &e))
    {
        switch (element_type)
        {
            case UI_TYPE_BUTTON: 
            {
                UI_button_style style = va_arg(args, UI_button_style);
                UNUSED(style);
                // ....
                
            } break;
            case UI_TYPE_PANEL: 
            {
                UI_panel_style style = va_arg(args, UI_panel_style);

                AS_PANEL(e)->element.type = UI_TYPE_PANEL;
                AS_PANEL(e)->element.ID.key = element_name;
                AS_PANEL(e)->element.ID.hash = HASH(element_name, strlen(element_name));
                AS_PANEL(e)->element.childs = NULL;
                AS_PANEL(e)->element.childs_count = 0;
                AS_PANEL(e)->style = style;
                e->childs_style = style.childs_style;
                // ....
                
            } break;
            default: break;
        }
    }
    else
    {
        switch (element_type)
        {
            case UI_TYPE_BUTTON: 
            {
                e = UI_MALLOC(sizeof(UI_button));
                UI_button_style style = va_arg(args, UI_button_style);
                sizing_s sizing = va_arg(args, sizing_s);
                UNUSED(style);
                UNUSED(sizing);
                // ....
                
            } break;
            case UI_TYPE_PANEL: 
            {
                e = UI_MALLOC(sizeof(UI_panel));
                UI_panel_style style = va_arg(args, UI_panel_style);
                sizing_s sizing = va_arg(args, sizing_s);
                
                AS_PANEL(e)->element.type = UI_TYPE_PANEL;
                AS_PANEL(e)->element.ID.key = element_name;
                AS_PANEL(e)->element.ID.hash = HASH(element_name, strlen(element_name));
                AS_PANEL(e)->element.childs = NULL;
                AS_PANEL(e)->element.childs_count = 0;
                AS_PANEL(e)->style = style;
                e->childs_style = style.childs_style;
                e->transform = transform2D(
                    vec2(0,0), 
                    vec2(0,0),
                    0
                );
                e->sizing = sizing;
                
                // ....
                
            } break;
            default: break;
        }
    }

    if (!context.curr_page.root)
    {
        context.curr_page.root = e;
    }


    dhash_table_add(context.curr_page.elements_table, e->ID.key, &e);
    
    if (context.head)
    {
        _ui_element_append_child(context.head, e);
        dstack_push(context.stack, &context.head);
    }

    context.head = e;
}

void ui_element_end(void)
{
    if (context.head) // Silent error otherwise [ On multiple calls to ui_element_end ]
    {
        UI_element* new_head = NULL;
        dstack_pop(context.stack, &new_head);
        context.head = new_head;
    }
}

void ui_element_begin_and_end(UI_type element_type, const char* element_name, ...);

void ui_edit_element_begin(const char* element_name)
{
    if (!context.curr_page.name)
    {
        LOG_ERROR("UI - you can not insert an element without an active page.\n"
                  "\tTry using ui_begin_page(\"page_name\") before calling this function again.\n"
                  "All ui commands will be ignore until you bind a page as active."); 
        return;
    }

    UI_element* e = _ui_page_find_element_(context.curr_page, element_name);
    if (!e)
    {
        LOG_ERROR("You provided to ui_edit_element_begin an element name which does not exist in the current page.");
        return;
    }

}

void ui_edit_element_end(void);

void ui_init(void)
{
    context.pages_table = dhash_table_create(32, sizeof(ui_page), HASH_TYPE_FNV1A, DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST, NULL);
    context.stack = dstack_create(8, sizeof(UI_element*), NULL);
    render_system_init();
}

void ui_shutdown(void)
{
    render_system_shutdown();
}

static void draw_ui_panel(UI_panel panel, transform2D_s transform)
{
    if (!panel.element.active || !panel.style.visible) return;

    render_system_draw(
            DRAWABLE_TYPE_QUAD,
            transform,
            Z_INDEX_0,
            panel.style.style
        );
}

static void draw_ui_button(UI_button button, transform2D_s transform)
{
    UNUSED(button); UNUSED(transform);
}


void ui_render(const char* page_name)
{
    if(!dhash_table_get(context.pages_table, page_name, &context.curr_page))
    {
        return;
    }

    if ((f32)get_window_width() != context.curr_page.ww || (f32)get_window_height() != context.curr_page.wh)
    {
        _calculate_ui_layout(context.curr_page);
    }

    dqueue_t queue = dqueue_create(128, sizeof(UI_element*), NULL);
    dqueue_push(&queue, &context.curr_page.root);
    
    while (dqueue_len(queue))
    {
        UI_element* n = NULL;
        if (dqueue_pop(queue, &n) && !n) continue;
    
        dlinked_list_iter iter = { 0 };
        
        UI_element* c;
        
        while (dlinked_list_iterator_next(n->childs, &iter, &c))
        {
            dqueue_push(&queue, &c);
        }
        
        switch (n->type)
        {
            case UI_TYPE_PANEL: draw_ui_panel(*AS_PANEL(n), n->transform); break;
            case UI_TYPE_BUTTON: draw_ui_button(*AS_BUTTON(n), n->transform); break;
            default: UNREACHABLE("NOT - SUPPORTED");
        }
    }
    
    dqueue_destroy(queue);

    render_system_render();
}

void ui_update(f32 delta_time)
{
    UNUSED(delta_time);
}