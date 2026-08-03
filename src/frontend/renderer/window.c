
#include "window.h"
#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

static struct { 
    GLFWwindow* handle;
    // Screen size always non-negative, 
    //  and it is OK to assume that the screen size is smaller then 2^32 - 1.
    u32 width; 
    u32 height;
} _window;

// Transitions between keys
/*
    up -> pressed -> down -> release -> up ...
             ^                  ^                            
            once               once
*/
typedef enum key_state {
    KEY_STATE_UP, // key is not pressed and had not being pressed for a while
    KEY_STATE_DOWN, // key is currently pressed
    KEY_STATE_PRESSED, // key just pressed 
    KEY_STATE_RELEASE, // key just released
} _key;

static struct {
    // mouse data
    struct {
        u32 x, y; 
        f64 scroll_offset;
        _key btns [ MOUSE_BUTTON_COUNT ];
    } mouse;

    _key keys [ KEY_COUNT ];
} _input;

static struct {
    f64 current_time;
    f64 last_time;
    f64 delta_time;
} _time;

static u8 key_to_index(u32 key_code)
{
    switch (key_code)
    {
        case KEY_SPACE: return 0;  
        case KEY_APOSTROPHE: return 1;  
        case KEY_COMMA: return 2;  
        case KEY_MINUS: return 3;  
        case KEY_PERIOD: return 4;  
        case KEY_SLASH: return 5;  
        case KEY_0: return 6;  
        case KEY_1: return 7;  
        case KEY_2: return 8;  
        case KEY_3: return 9;  
        case KEY_4: return 10;  
        case KEY_5: return 11;  
        case KEY_6: return 12;  
        case KEY_7: return 13;  
        case KEY_8: return 14;  
        case KEY_9: return 15;  
        case KEY_SEMICOLON: return 16;  
        case KEY_EQUAL: return 17;  
        case KEY_A: return 18;  
        case KEY_B: return 19;  
        case KEY_C: return 20;  
        case KEY_D: return 21;  
        case KEY_E: return 22;  
        case KEY_F: return 23;  
        case KEY_G: return 24;  
        case KEY_H: return 25;  
        case KEY_I: return 26;  
        case KEY_J: return 27;  
        case KEY_K: return 28;  
        case KEY_L: return 29;  
        case KEY_M: return 30;  
        case KEY_N: return 31;  
        case KEY_O: return 32;  
        case KEY_P: return 33;  
        case KEY_Q: return 34;  
        case KEY_R: return 35;  
        case KEY_S: return 36;  
        case KEY_T: return 37;  
        case KEY_U: return 38;  
        case KEY_V: return 39;  
        case KEY_W: return 40;  
        case KEY_X: return 41;  
        case KEY_Y: return 42;  
        case KEY_Z: return 43;  
        case KEY_LEFT_BRACKET: return 44;  
        case KEY_BACKSLASH: return 45;  
        case KEY_RIGHT_BRACKET: return 46;  
        case KEY_GRAVE_ACCENT: return 47;  
        case KEY_WORLD_1: return 48;  
        case KEY_WORLD_2: return 49;  
        case KEY_ESCAPE: return 50;  
        case KEY_ENTER: return 51;  
        case KEY_TAB: return 52;  
        case KEY_BACKSPACE: return 53;  
        case KEY_INSERT: return 54;  
        case KEY_DELETE: return 55;  
        case KEY_RIGHT: return 56;  
        case KEY_LEFT: return 57;  
        case KEY_DOWN: return 58;  
        case KEY_UP: return 59;  
        case KEY_PAGE_UP: return 60;  
        case KEY_PAGE_DOWN: return 61;  
        case KEY_HOME: return 62;  
        case KEY_END: return 63;  
        case KEY_CAPS_LOCK: return 64;  
        case KEY_SCROLL_LOCK: return 65;  
        case KEY_NUM_LOCK: return 66;  
        case KEY_PRINT_SCREEN: return 67;  
        case KEY_PAUSE: return 68;  
        case KEY_F1: return 69;  
        case KEY_F2: return 70;  
        case KEY_F3: return 71;  
        case KEY_F4: return 72;  
        case KEY_F5: return 73;  
        case KEY_F6: return 74;  
        case KEY_F7: return 75;  
        case KEY_F8: return 76;  
        case KEY_F9: return 77;  
        case KEY_F10: return 78;  
        case KEY_F11: return 79;  
        case KEY_F12: return 80;  
        case KEY_F13: return 81;  
        case KEY_F14: return 82;  
        case KEY_F15: return 83;  
        case KEY_F16: return 84;  
        case KEY_F17: return 85;  
        case KEY_F18: return 86;  
        case KEY_F19: return 87;  
        case KEY_F20: return 88;  
        case KEY_F21: return 89;  
        case KEY_F22: return 90;  
        case KEY_F23: return 91;  
        case KEY_F24: return 92;  
        case KEY_F25: return 93;  
        case KEY_KP_0: return 94;  
        case KEY_KP_1: return 95;  
        case KEY_KP_2: return 96;  
        case KEY_KP_3: return 97;  
        case KEY_KP_4: return 98;  
        case KEY_KP_5: return 99;  
        case KEY_KP_6: return 100;  
        case KEY_KP_7: return 101;  
        case KEY_KP_8: return 102;  
        case KEY_KP_9: return 103;  
        case KEY_KP_DECIMAL: return 104;  
        case KEY_KP_DIVIDE: return 105;  
        case KEY_KP_MULTIPLY: return 106;  
        case KEY_KP_SUBTRACT: return 107;  
        case KEY_KP_ADD: return 108;  
        case KEY_KP_ENTER: return 109;  
        case KEY_KP_EQUAL: return 110;  
        case KEY_LEFT_SHIFT: return 111;  
        case KEY_LEFT_CONTROL: return 112;  
        case KEY_LEFT_ALT: return 113;  
        case KEY_LEFT_SUPER: return 114;  
        case KEY_RIGHT_SHIFT: return 115;  
        case KEY_RIGHT_CONTROL: return 116;  
        case KEY_RIGHT_ALT: return 117;  
        case KEY_RIGHT_SUPER: return 118;  
        case KEY_MENU: return 119;  
        default:
            UNREACHABLE("Key code does not exist");
            return U8_MAX;
    }
}

static u8 mouse_btn_to_index(u8 btn_code)
{
    if (btn_code > MOUSE_BUTTON_COUNT) 
    {
        UNREACHABLE("Mouse button code not exists!");
        return 0U;
    }
    
    return btn_code;
}

static void  window_size_changed_callback(GLFWwindow* window, int width, int height)
{
    UNUSED(window);

    _window.width  = (u32)width;
    _window.height = (u32)height;

    glViewport(0, 0, _window.width, _window.width);
}

static void window_key_pressed_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    UNUSED(window); UNUSED(scancode); UNUSED(mods);

    u8 index = key_to_index(key);
    _key state = action == GLFW_PRESS   ? KEY_STATE_PRESSED : 
                 action == GLFW_REPEAT  ? KEY_STATE_DOWN    :
                 action == GLFW_RELEASE ? KEY_STATE_RELEASE :
                 KEY_STATE_UP;
    if (index < KEY_COUNT) _input.keys[ index ] = state;
} 

static void window_callback_mouse_pressed(GLFWwindow* window, int button, int action, int mods)
{
    UNUSED(window); UNUSED(action); UNUSED(mods);

    u8 index = mouse_btn_to_index(button);
    _key state = action == GLFW_PRESS   ? KEY_STATE_PRESSED : 
                 action == GLFW_REPEAT  ? KEY_STATE_DOWN    :
                 action == GLFW_RELEASE ? KEY_STATE_RELEASE :
                 KEY_STATE_UP;
    if (index < MOUSE_BUTTON_COUNT) _input.mouse.btns[ index ] = state;
}

static void window_scroll_callback(GLFWwindow * win, f64 offsetx, f64 offsety)
{
    UNUSED(offsetx); UNUSED(win);
    _input.mouse.scroll_offset = offsety;
}

void init_openGL(void)
{
    glViewport(0,0,_window.width, _window.height);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    // TODO: glEnable(stuff)...
}

void window_update(void)
{
    _time.current_time = glfwGetTime();
    _time.delta_time   = _time.current_time - _time.last_time;
    _time.last_time    = _time.current_time;
    
    for (u8 i = 0; i < KEY_COUNT; i++)
    {
        _input.keys[i] = _input.keys[i] == KEY_STATE_PRESSED ? KEY_STATE_DOWN :
        _input.keys[i] == KEY_STATE_RELEASE ? KEY_STATE_UP   :
        _input.keys[i];
    }
    
    for (u8 i = 0; i < MOUSE_BUTTON_COUNT; i++)
    {
        _input.mouse.btns[i] = _input.mouse.btns[i] == KEY_STATE_PRESSED ? KEY_STATE_DOWN :
        _input.mouse.btns[i] == KEY_STATE_RELEASE ? KEY_STATE_UP   :
        _input.mouse.btns[i]; 
    }

    glfwPollEvents();
    glfwSwapBuffers(_window.handle); 
}

bool window_init(u32 width, u32 height, const char* title)
{
    // Init GLFW and glfw-window
    if (!glfwInit())
    {
        LOG_ERROR("EROR: initializing GLFW.\n");
        return false;
    }

    _window.handle = glfwCreateWindow((int)width, (int)height, title, NULL, NULL);
    
    if (!_window.handle)
    {
        LOG_ERROR("EROR: initializing glfw-window.\n");
        return false;
    }

    _window.width  = width;
    _window.height = height;
    
    // TODO: add required hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwMaximizeWindow(_window.handle); // ??
    
    // TODO: set callbacks
    glfwSetFramebufferSizeCallback(_window.handle, window_size_changed_callback);
    glfwSetKeyCallback(_window.handle, window_key_pressed_callback);
    glfwSetMouseButtonCallback(_window.handle, window_callback_mouse_pressed);
    glfwSetScrollCallback(_window.handle, window_scroll_callback);

    // Init openGL
    glfwMakeContextCurrent(_window.handle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "ERROR: init glad");
        return false;
    }
    
    init_openGL();
   
    return true;
}

bool window_should_close(void)
{
    return glfwWindowShouldClose(_window.handle);
}

bool is_key_pressed(u32 key_code)
{
    u8 index = key_to_index(key_code);
    if (index <= KEY_COUNT)
        return _input.keys[ index ] == KEY_STATE_PRESSED;
    return false;
}

bool is_key_released(u32 key_code)
{
    u8 index = key_to_index(key_code);
    if (index <= KEY_COUNT)
        return _input.keys[ index ] == KEY_STATE_RELEASE;
    return false;
}

bool is_key_down(u32 key_code)
{
    u8 index = key_to_index(key_code);
    if (index <= KEY_COUNT)
        return _input.keys[ index ] == KEY_STATE_DOWN;
    return false;
}

bool is_key_up(u32 key_code)
{
    u8 index = key_to_index(key_code);
    if (index <= KEY_COUNT)
        return _input.keys[ index ] == KEY_STATE_UP;
    return false;
}

bool is_mouse_button_pressed(u8 button_code)
{
    u8 index = mouse_btn_to_index(button_code);
    if (index <= KEY_COUNT)
        return _input.mouse.btns[ index ] == KEY_STATE_PRESSED;
    return false;
}

bool is_mouse_button_released(u8 button_code)
{
    u8 index = mouse_btn_to_index(button_code);
    if (index <= KEY_COUNT)
        return _input.mouse.btns[ index ] == KEY_STATE_RELEASE;
    return false;
}

bool is_mouse_button_down(u8 button_code)
{
    u8 index = mouse_btn_to_index(button_code);
    if (index <= KEY_COUNT)
        return _input.mouse.btns[ index ] == KEY_STATE_DOWN;
    return false;
}

bool is_mouse_button_up(u8 button_code)
{
    u8 index = mouse_btn_to_index(button_code);
    if (index <= KEY_COUNT)
        return _input.mouse.btns[ index ] == KEY_STATE_UP;
    return false;
}

void window_destroy(void)
{
    glfwDestroyWindow(_window.handle);
    glfwTerminate();
}

void window_clear_screen(void)
{
    glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

u32 get_mouse_x(void)
{
    return _input.mouse.x;
}

u32 get_mouse_y(void)
{
    return _input.mouse.x;
}

u32 get_window_width(void)
{
    return _window.width;
}

u32 get_window_height(void)
{
    return _window.height;
}

f64 get_mouse_scroll_offset(void)
{
    return _input.mouse.scroll_offset;
}

f64 get_time(void)
{
    return _time.current_time;
}

f64 get_delta_time(void)
{
    return _time.delta_time;
}
