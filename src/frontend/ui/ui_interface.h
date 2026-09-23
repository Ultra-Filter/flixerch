#pragma once
#include "core/utils/defines.h"
#include "core/utils/math_engine.h"
#include "frontend/renderer/color.h"
#include "frontend/renderer/quad_renderer.h"

typedef enum UI_type {
    UI_TYPE_NONE,
    UI_TYPE_BUTTON,
    UI_TYPE_PANEL,
    UI_TYPE_COUNT
} UI_type;

typedef enum UI_childs_align {
    UI_CHILDS_ALIGN_VERTICAL,
    UI_CHILDS_ALIGN_HORIZONTAL,
    UI_CHILDS_ALIGN_SPACE_BETWEEN,
    UI_CHILDS_ALIGN_SPACE_AROUND,
    UI_CHILDS_ALIGN_CENTER,
    UI_CHILDS_ALIGN_START,
    UI_CHILDS_ALIGN_END,
    UI_CHILDS_ALIGN_COUNT
} UI_childs_align;

typedef struct UI_childs_style {
    f32 gap;
    UI_childs_align direction;
    UI_childs_align align_main;
    UI_childs_align align_cross;
    vec4_t paddings; // top, bottom, right, left
} UI_childs_style;

typedef struct sizing_s {
    bool grow;
    bool keep_aspect;
    f32 min_w, min_h, max_w, max_h;
    f32 aspect_ratio;
} sizing_s;

typedef struct UI_panel_style 
{
    bool visible;
    quad_style_s style;
    UI_childs_style childs_style;
} UI_panel_style;

typedef struct UI_button_style {
    struct borders_style {
        vec4_t radii;
        f32 thickness;
        rgba_t color;
    } borders;
    struct background_style {
        union {
            struct gradient {
                rgba_t colors[ 3 ];
                enum gradient_type {
                    GRADIENT_LINEAR_H,
                    GRADIENT_LINEAR_V,
                    GRADIENT_CIRCULAR
                } gradient_type;
            } gradient;
            rgba_t solid;
        } color;
    } background;
} UI_button_style;




typedef struct UI_animation {
    enum UI_animation_curve : u8 {
        UI_ANIMATION_CURVE_LINEAR ,
        UI_ANIMATION_CURVE_CUBIC_EASE,
        UI_ANIMATION_CURVE_EXP_EASE,
        UI_ANIMATION_CURVE_STEP,
        UI_ANIMATION_CURVE_SMOOTH_STEP
    } animation_curve;
    f32 start_value; 
    f32 end_value; 
    f32 total_time;
    f32* ptr_to_value_to_animate;
} UI_animation;

void ui_page_begin(const char* page_name);
void ui_page_end();

void ui_element_begin(UI_type element_type, const char* element_name, ...);
void ui_element_end(void);

void ui_init(void);
void ui_shutdown(void);
void ui_render(const char* page_name);
void ui_update(f32 delta_time);

/*
    // Example usage of UI layout library
    void create_ui_home_page(void)
    {
        ui_create_page(HOME_PAGE_NAME); pushes page to page array

        ui_page_begin(HOME_PAGE_NAME); // pushes page to current stack for editin the page tree - Also creating root element

            // pushes the top-bar element as the head of stack
            ui_element_begin(UI_ELEMENT_TYPE_PANEL, TOP_BAR_PANEL_STYLE, "top-bar"); 

                // Pushes search-bar as the head element and as a child of top-bar
                ui_element_begin(UI_ELEMENT_TYPE_PANEL, SEARCH_BAR_PANEL_STYLE, "search-bar");
                    // equivalent to ui_element_begin(...); ui_element_end(); pushes and pops immidatly 
                    ui_element_begin_and_end(UI_ELEMENT_TYPE_IMAGE, SEARCH_BAR_SEARCH_ICON_STYLE, "assets/images/seach-icon.png", "search-icon"); 
                    ui_element_begin_and_end(UI_ELEMENT_TYPE_TEXT_BOX, SEARCH_BAR_TEXT_BOX_STYLE, "search-box"); 
                ui_element_end(); // Pops the search-bar out of the head -> going back to top-bar as the head.

                ui_element_begin_and_end(UI_ELEMENT_TYPE_IMAGE, LOGO_ICON_STYLE, "assets/images/logo.png", "logo");
                ui_element_begin_and_end(UI_ELEMENT_TYPE_IMAGE, EXIT_ICON_STYLE, "assets/images/exit.png", "exit");

            ui_element_end(); // Pops the top-bar -> Going back to root.

            // For now there is nothing to push, after we will enter the main loop we can insert stuff between begin and end.
            ui_element_begin_and_end(UI_ELEMENT_PANEL, CONTENT_PANEL_STYLE, "content-panel");

        ui_page_end(); // Pop HomePage from current stack    
    }

    void create_movie_card(movie_name, movie_image ,movie_card_element_name)
    {
        ui_element_begin(UI_ELEMENT_TYPE_PANEL, MOVIE_CARD_PANEL_STYLE, movie_card_element_name);
            ui_element_begin_and_end(UI_ELEMENT_TYPE_IMAGE, movie_image, concat(movie_card_element_name, "-image"));
            ui_element_begin_and_end(UI_ELEMENT_TYPE_TEXT, movie_name, concat(movie_card_element_name, "-text"));
        ui_element_end();
    }

    int main(void)
    {
        // init all stuff u need.

        create_ui_home_page();
        ...

        while (running)
        {
            ...
            ui_update(); // calculate layout.

            ui_edit_page_element_begin(HOME_PAGE_NAME, "content-panel"); // Start dynamic editing - Push content panel as root
            for (movie* m = movies; m < movies + movies_count; m++)
            {
                create_movie_card(m.name, m.image, m.card_name);
            }
            ui_edit_page_element_end();


            ui_draw(context.current_page_name); // draws the page - exec draw calls
        }
    }
*/