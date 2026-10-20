#pragma once

typedef char * dstr_t;
typedef enum ui_element_type ui_element_type;
typedef struct ui_create_data ui_create_data;
typedef struct dstr_memory_allocator dstr_memory_allocator;

void ui_init(void);
void ui_shutdown(void);
void ui_end_frame(void);
void ui_element_end(void);
void ui_element_begin(dstr_t name, ui_element_type type, ui_create_data create_data);

dstr_memory_allocator* ui_allocator(void);

#define UI_ELEMENT(name, type, create_data) \
    for ( \
        int __x__ = (ui_element_begin(dstr_create_from_cstr(name, ui_allocator()), type, create_data), 0); \
        __x__ != 1; \
        ui_element_end(), __x__ = 1 \
    )
