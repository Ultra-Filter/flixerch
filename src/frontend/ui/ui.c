#include "ui_interface.h"
#include "core/utils/darena.h"
#include "core/utils/containers/dstring.h"
#include "frontend/renderer/window.h"


#define UI_MALLOC allocate

static struct UI_CONTEXT 
{
    ui_element* curr_elem;
    darena_t arena;
} CTX;

void * allocate(u64 size)
{
    return darena_malloc(CTX.arena, size);
}

void* reallocate(void* block, u64 size)
{
    return darena_realloc(CTX.arena, block, size);
}

void dfree(void* block)
{
    darena_free(CTX.arena, block);
}


dstr_memory_allocator allocator  = {
    .allocate = allocate,
    .reallocate = reallocate,
    .free = dfree,
};

dstr_memory_allocator* ui_allocator(void)
{
    return &allocator;
}

int compare_elements_w(const void* e1, const void * e2)
{
    const ui_element* e11 = (const ui_element*)e1;
    const ui_element* e22 = (const ui_element*)e2;
    return (int)(e11->transform.scale.x - e22->transform.scale.x);  
}

int compare_elements_h(const void* e1, const void * e2)
{
    const ui_element* e11 = (const ui_element*)e1;
    const ui_element* e22 = (const ui_element*)e2;
    return (int)(e11->transform.scale.y - e22->transform.scale.y);  
}


void grow_sizing(ui_element* parent)
{
    if (!parent || !parent->childs_count) return;

    bool horizontal = parent->create_data.childs.direction == HORIZONTAL;

    f32 remain = (
            horizontal ? 
            parent->transform.scale.x - parent->create_data.childs.paddings.left - parent->create_data.childs.paddings.right : 
            parent->transform.scale.y - parent->create_data.childs.paddings.bottom - parent->create_data.childs.paddings.top
        ) - parent->sum_sizes - parent->create_data.childs.gap_main * (parent->childs_count - 1);

    u32 number_of_growable = 0;
    
    ui_element* c = parent->first_child;
    
    while (c)
    {
        if (horizontal && c->create_data.self.width.type == SIZING_GROW)
        {
            number_of_growable++;
        }
        else if (!horizontal && c->create_data.self.height.type == SIZING_GROW)
        {
            number_of_growable++;
        }
        
        if (horizontal && c->create_data.self.height.type == SIZING_GROW)
        {
            c->transform.scale.y = fminf(
                c->create_data.self.height.max,
                fmaxf(
                    parent->max_element_size, 
                    parent->transform.scale.y - parent->create_data.childs.paddings.top - parent->create_data.childs.paddings.bottom
                )
            );
        }
        else if (!horizontal && c->create_data.self.width.type == SIZING_GROW)
        {
            c->transform.scale.x = fminf(
                c->create_data.self.width.max,
                fmaxf(
                    parent->max_element_size, 
                    parent->transform.scale.x - parent->create_data.childs.paddings.left - parent->create_data.childs.paddings.right
                )
            );
        }
        
        c = c->right_sibling;
    }

    c =  parent->first_child;
    if (remain > 0.0F && c)
    {
        if (number_of_growable)
        {
            c = parent->first_child;
            
            ui_element** growables = UI_MALLOC(sizeof(ui_element*) * number_of_growable);
            assert(growables);
            
            u32 idx = 0;
            
            while (c)
            {
                if (horizontal && c->create_data.self.width.type == SIZING_GROW)
                {
                    growables[idx++] = c;
                }
                else if (!horizontal && c->create_data.self.height.type == SIZING_GROW)
                {
                    growables[idx++] = c;
                }
                c = c->right_sibling;
            }
            
            qsort(growables, number_of_growable, sizeof(ui_element*), horizontal ? compare_elements_w : compare_elements_h);
            
            while (remain >= 0.0001)
            {
                u32 count_of_min_val = 0;
                f32 min_val = horizontal ? growables[ 0 ]->transform.scale.x : growables[ 0 ]->transform.scale.y; // TODO: this is not the min val it might !can_grow
                f32 next_val = min_val;
                
                for (u32 i = 0; i < number_of_growable; i++)    
                {
                    bool can_grow = (horizontal && (growables[ i ]->create_data.self.width.max > growables[ i ]->transform.scale.x)) ||
                        (!horizontal && (growables[ i ]->create_data.self.height.max > growables[ i ]->transform.scale.y));

                    if (
                        (
                            (horizontal && growables[ i ]->transform.scale.x <= min_val) || 
                            (!horizontal && growables[ i ]->transform.scale.y <= min_val)
                        ) && can_grow
                    ) 
                        count_of_min_val++;

                }

                if (!count_of_min_val) break;
                
                f32 amount = fminf(remain, (next_val - min_val) * count_of_min_val);

                if (next_val == min_val) amount = remain;
                bool anyone_grew = false;
                
                for (u32 i = 0; i < number_of_growable; i++)    
                {
                    if (
                        (horizontal && growables[ i ]->transform.scale.x == min_val) ||
                        (!horizontal && growables[ i ]->transform.scale.y == min_val)
                    ) 
                    {
                        bool can_grow = (horizontal && (growables[ i ]->create_data.self.width.max > growables[ i ]->transform.scale.x)) ||
                            (!horizontal && (growables[ i ]->create_data.self.height.max > growables[ i ]->transform.scale.y));
                        if (horizontal && can_grow) 
                        {
                            anyone_grew = true;
                            if (horizontal)
                                growables[ i ]->transform.scale.x += amount / count_of_min_val;
                            else
                                growables[ i ]->transform.scale.y += amount / count_of_min_val;

                        }
                        else if(can_grow) growables[ i ]->transform.scale.y += amount / count_of_min_val;
                    }
                }

                if (!anyone_grew) break;

                parent->sum_sizes += amount;
                
                remain -= amount;
            }
        } 
    }
    
    c = parent->first_child;
    
    while (c)
    {
        grow_sizing(c);
        c = c->right_sibling;
    }
}
  

  
void ui_element_begin(dstr_t name, ui_element_type type, ui_create_data create_data)
{
    ui_element* new_elem = (ui_element*)UI_MALLOC(sizeof(ui_element));
    assert(new_elem);
    
    new_elem->name             = name;
    new_elem->first_child      = NULL;
    new_elem->right_sibling    = NULL;
    new_elem->type             = type;
    new_elem->create_data      = create_data;
    new_elem->parent           = CTX.curr_elem;
    new_elem->childs_count     = 0;
    new_elem->transform        = (transform2D_s){ 0 };
    new_elem->max_element_size = 0.0F;
    new_elem->sum_sizes        = 0.0F;
    // TODO: continue

    if (!CTX.curr_elem) 
    {
        CTX.curr_elem = new_elem;
        return;  
    }

    ui_element* p = CTX.curr_elem->first_child;
    CTX.curr_elem->childs_count++;

    if (!p) {
        CTX.curr_elem->first_child = new_elem;
        CTX.curr_elem = new_elem;
        return;
    }

    while(p->right_sibling) p = p->right_sibling;

    p->right_sibling = new_elem;
    CTX.curr_elem = new_elem;
}




void ui_element_end(void)
{
    ui_element* parent = CTX.curr_elem->parent;
    if (!parent) return;

    bool horizontal = CTX.curr_elem->create_data.childs.direction == HORIZONTAL;

    const f32 horizontal_paddings = 
        CTX.curr_elem->create_data.childs.paddings.left + 
        CTX.curr_elem->create_data.childs.paddings.right;
    const f32 vertical_paddings = 
        CTX.curr_elem->create_data.childs.paddings.bottom + 
        CTX.curr_elem->create_data.childs.paddings.top;
    const f32 main_extra =  (horizontal && CTX.curr_elem->childs_count) ? 
            (CTX.curr_elem->childs_count - 1) * CTX.curr_elem->create_data.childs.gap_main : 0.0F;

    if (horizontal)
    {
        if (CTX.curr_elem->create_data.self.width.type != SIZING_FIXED)
        {
            CTX.curr_elem->create_data.self.width.min = fmaxf(
                CTX.curr_elem->create_data.self.width.min, 
                CTX.curr_elem->sum_sizes + (CTX.curr_elem->childs_count ? horizontal_paddings : 0.0F) + main_extra 
            );
        }
        else
        {
            CTX.curr_elem->create_data.self.width.min = CTX.curr_elem->create_data.self.width.value;
        }

        if (CTX.curr_elem->create_data.self.height.type != SIZING_FIXED)
        {
            CTX.curr_elem->create_data.self.height.min = fmaxf(
                CTX.curr_elem->create_data.self.height.min, 
                CTX.curr_elem->max_element_size + (CTX.curr_elem->childs_count ? vertical_paddings : 0.0F)
            );    
        }
        else
        {
            CTX.curr_elem->create_data.self.height.min = CTX.curr_elem->create_data.self.height.value;
        }
    }
    else
    {
        if (CTX.curr_elem->create_data.self.width.type != SIZING_FIXED)
        {
            CTX.curr_elem->create_data.self.width.min = fmaxf(
                CTX.curr_elem->create_data.self.width.min, 
                CTX.curr_elem->max_element_size + (CTX.curr_elem->childs_count ? horizontal_paddings : 0.0F)
            );    
        }
        else
        {
            CTX.curr_elem->create_data.self.width.min = CTX.curr_elem->create_data.self.width.value;
        }
        
        if (CTX.curr_elem->create_data.self.height.type != SIZING_FIXED)
        {
            CTX.curr_elem->transform.scale.x = fmaxf(
                CTX.curr_elem->create_data.self.height.min, 
                CTX.curr_elem->sum_sizes +  (CTX.curr_elem->childs_count ? vertical_paddings : 0.0F) + main_extra
            );
        }
        else
        {
            CTX.curr_elem->create_data.self.height.min = CTX.curr_elem->create_data.self.height.value;
        }
    }

    CTX.curr_elem->transform.scale.x = fminf(
        fmaxf(
            CTX.curr_elem->create_data.self.width.value,
            CTX.curr_elem->create_data.self.width.min
        ),
        CTX.curr_elem->create_data.self.width.max
    );
    CTX.curr_elem->transform.scale.y = fminf(
        fmaxf(
            CTX.curr_elem->create_data.self.height.value, 
            CTX.curr_elem->create_data.self.height.min
        ),
        CTX.curr_elem->create_data.self.height.max
    );

    bool p_horizontal = parent->create_data.childs.direction == HORIZONTAL;

    parent->sum_sizes +=  p_horizontal ? CTX.curr_elem->transform.scale.x : CTX.curr_elem->transform.scale.y;

    parent->max_element_size = fmaxf(
        parent->max_element_size, 
        (p_horizontal ? CTX.curr_elem->transform.scale.y : CTX.curr_elem->transform.scale.x) 
    );

    if (CTX.curr_elem && CTX.curr_elem->parent)
        CTX.curr_elem = CTX.curr_elem->parent;
}

static void calc_positions(ui_element* root)
{
    const bool horizontal = root->create_data.childs.direction == HORIZONTAL; 
    const bool vertical = !horizontal;
    
    const f32 total_main_size = root->childs_count ? root->sum_sizes + (root->childs_count - 1) * root->create_data.childs.gap_main : 0.0F;  
    const f32 remain_main = horizontal ? 
        root->transform.scale.x - root->create_data.childs.paddings.left - root->create_data.childs.paddings.right - total_main_size :
        root->transform.scale.y - root->create_data.childs.paddings.top - root->create_data.childs.paddings.bottom - total_main_size;
    // const f32 remain_cross = vertical ? 
    //     root->transform.scale.x - root->create_data.childs.paddings.left - root->create_data.childs.paddings.right - root->max_element_size :
    //     root->transform.scale.y - root->create_data.childs.paddings.top - root->create_data.childs.paddings.bottom - root->max_element_size;

    // TODO: consider layouts - End, Start, Center etc
    const f32 start_main = horizontal 
        ? 
        root->transform.position.x +
        (
            root->create_data.childs.align_main == CENTER ? (root->transform.scale.x - total_main_size) * 0.5F :
            root->create_data.childs.align_main == START ? root->create_data.childs.paddings.left : 
            root->create_data.childs.align_main == END ? root->transform.scale.x - root->create_data.childs.paddings.right - total_main_size : 
            root->create_data.childs.align_main == SPACE_AROUND ? root->create_data.childs.paddings.left + remain_main / (root->childs_count + 1) : 
            root->create_data.childs.paddings.left
        ) 
        : 
        root->transform.position.y +
        (
            root->create_data.childs.align_main == CENTER ? (root->transform.scale.y - total_main_size) * 0.5F :
            root->create_data.childs.align_main == START ? root->create_data.childs.paddings.top : 
            root->create_data.childs.align_main == END ? root->transform.scale.y - root->create_data.childs.paddings.bottom - total_main_size : 
            root->create_data.childs.align_main == SPACE_AROUND ? root->create_data.childs.paddings.top + remain_main / (root->childs_count + 1) : 
            root->create_data.childs.paddings.top
        );

    const f32 start_cross = horizontal 
        ? 
        root->transform.position.y +
        (
            root->create_data.childs.align_cross == CENTER ? (root->transform.scale.y * 0.5F) :
            root->create_data.childs.align_cross == START ? (root->create_data.childs.paddings.top) :
            root->create_data.childs.align_cross == END ? (root->transform.scale.y - root->create_data.childs.paddings.bottom) :
            root->create_data.childs.paddings.top
        )
        :
        root->transform.position.x +
        (
            root->create_data.childs.align_cross == CENTER ? (root->transform.scale.x * 0.5F) :
            root->create_data.childs.align_cross == START ? (root->create_data.childs.paddings.left) :
            root->create_data.childs.align_cross == END ? (root->transform.scale.x - root->create_data.childs.paddings.right) : 
            root->create_data.childs.paddings.left
        );

    // const f32 root_main_scale = horizontal ? root->transform.scale.x : root->transform.scale.y;

    f32 offset_main = start_main;
    f32 offset_cross = start_cross;
    f32 max_cross = 0.0F;

    ui_element* c = root->first_child;
    while (c)
    {

        f32 advance_main = (horizontal ? c->transform.scale.x : c->transform.scale.y); 
        
        // if (offset_main + advance_main> root_main_scale + start_main) // TODO: add padding end
        // {
        //     offset_cross += max_cross + root->create_data.childs.gap_cross.value;
        //     offset_main = start_main;
        // }
        
        max_cross = fmaxf(max_cross, (vertical ? c->transform.scale.x : c->transform.scale.y));

        c->transform.position.x = (horizontal) ? offset_main : 
            root->create_data.childs.align_cross == CENTER ? offset_cross - c->transform.scale.x * 0.5F :
            root->create_data.childs.align_cross == START ? offset_cross :
            root->create_data.childs.align_cross == END ? offset_cross - c->transform.scale.x : 
            offset_cross;
        
        c->transform.position.y = (vertical)   ? offset_main : 
            root->create_data.childs.align_cross == CENTER ? offset_cross - c->transform.scale.y * 0.5F :
            root->create_data.childs.align_cross == START ? offset_cross :
            root->create_data.childs.align_cross == END ? offset_cross - c->transform.scale.y : 
            offset_cross;
        
        offset_main += root->create_data.childs.gap_main + advance_main + 
            (
                root->create_data.childs.align_main == SPACE_BETWEEN  && root->childs_count ? remain_main / (root->childs_count - 1) : 
                root->create_data.childs.align_main == SPACE_AROUND ? remain_main / (root->childs_count + 1) : 
                0.0F 
            ); // TODO:

        calc_positions(c);

        c = c->right_sibling;
    }
}

void calc_ui(void /* page */)
{
    ui_element* root = CTX.curr_elem; // TODO: change - page->root
    if (!root) return;
    
    root->transform.scale.x = get_window_width();
    root->transform.scale.y = get_window_height();
    root->create_data.self.width.value = root->transform.scale.x; 
    root->create_data.self.height.value = root->transform.scale.y; 
    root->create_data.self.width.min = root->transform.scale.x; 
    root->create_data.self.width.max = root->transform.scale.x; 
    root->create_data.self.height.min = root->transform.scale.y; 
    root->create_data.self.height.max = root->transform.scale.y; 
    
    grow_sizing(root);
    calc_positions(root);
}

void ui_draw(ui_element* root)
{
    if(root->create_data.self.visible)
    {
        render_system_draw(
            DRAWABLE_TYPE_QUAD, 
            root->transform,
            Z_INDEX_0,
            root->create_data.style
        );
    }
    if (root->right_sibling) ui_draw(root->right_sibling);
    if (root && root->first_child) ui_draw(root->first_child);
    
    
}

void ui_end_frame(void)
{
    calc_ui();
    ui_draw(CTX.curr_elem);
    darena_clear(CTX.arena);    
    CTX.curr_elem = NULL;
    render_system_render();
}

void ui_init(void)
{
    render_system_init();
    CTX.arena = darena_create(DARENA_TYPE_LINEAR, DKB(10));
    CTX.curr_elem = NULL;
}

void ui_shutdown(void)
{
    darena_destroy(CTX.arena);
    render_system_shutdown();
}
