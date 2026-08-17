#pragma once
#include "core/utils/defines.h"

#define KEY_SPACE              32U
#define KEY_APOSTROPHE         39U  /* ' */
#define KEY_COMMA              44U  /* , */
#define KEY_MINUS              45U  /* - */
#define KEY_PERIOD             46U  /* . */
#define KEY_SLASH              47U  /* / */
#define KEY_0                  48U
#define KEY_1                  49U
#define KEY_2                  50U
#define KEY_3                  51U
#define KEY_4                  52U
#define KEY_5                  53U
#define KEY_6                  54U
#define KEY_7                  55U
#define KEY_8                  56U
#define KEY_9                  57U
#define KEY_SEMICOLON          59U  /* ; */
#define KEY_EQUAL              61U  /* = */
#define KEY_A                  65U
#define KEY_B                  66U
#define KEY_C                  67U
#define KEY_D                  68U
#define KEY_E                  69U
#define KEY_F                  70U
#define KEY_G                  71U
#define KEY_H                  72U
#define KEY_I                  73U
#define KEY_J                  74U
#define KEY_K                  75U
#define KEY_L                  76U
#define KEY_M                  77U
#define KEY_N                  78U
#define KEY_O                  79U
#define KEY_P                  80U
#define KEY_Q                  81U
#define KEY_R                  82U
#define KEY_S                  83U
#define KEY_T                  84U
#define KEY_U                  85U
#define KEY_V                  86U
#define KEY_W                  87U
#define KEY_X                  88U
#define KEY_Y                  89U
#define KEY_Z                  90U
#define KEY_LEFT_BRACKET       91U  /* [ */
#define KEY_BACKSLASH          92U  /* \ */
#define KEY_RIGHT_BRACKET      93U  /* ] */
#define KEY_GRAVE_ACCENT       96U  /* ` */
#define KEY_WORLD_1            161U /* non-US #1 */
#define KEY_WORLD_2            162U /* non-US #2 */
#define KEY_ESCAPE             256U
#define KEY_ENTER              257U
#define KEY_TAB                258U
#define KEY_BACKSPACE          259U
#define KEY_INSERT             260U
#define KEY_DELETE             261U
#define KEY_RIGHT              262U
#define KEY_LEFT               263U
#define KEY_DOWN               264U
#define KEY_UP                 265U
#define KEY_PAGE_UP            266U
#define KEY_PAGE_DOWN          267U
#define KEY_HOME               268U
#define KEY_END                269U
#define KEY_CAPS_LOCK          280U
#define KEY_SCROLL_LOCK        281U
#define KEY_NUM_LOCK           282U
#define KEY_PRINT_SCREEN       283U
#define KEY_PAUSE              284U
#define KEY_F1                 290U
#define KEY_F2                 291U
#define KEY_F3                 292U
#define KEY_F4                 293U
#define KEY_F5                 294U
#define KEY_F6                 295U
#define KEY_F7                 296U
#define KEY_F8                 297U
#define KEY_F9                 298U
#define KEY_F10                299U
#define KEY_F11                300U
#define KEY_F12                301U
#define KEY_F13                302U
#define KEY_F14                303U
#define KEY_F15                304U
#define KEY_F16                305U
#define KEY_F17                306U
#define KEY_F18                307U
#define KEY_F19                308U
#define KEY_F20                309U
#define KEY_F21                310U
#define KEY_F22                311U
#define KEY_F23                312U
#define KEY_F24                313U
#define KEY_F25                314U
#define KEY_KP_0               320U
#define KEY_KP_1               321U
#define KEY_KP_2               322U
#define KEY_KP_3               323U
#define KEY_KP_4               324U
#define KEY_KP_5               325U
#define KEY_KP_6               326U
#define KEY_KP_7               327U
#define KEY_KP_8               328U
#define KEY_KP_9               329U
#define KEY_KP_DECIMAL         330U
#define KEY_KP_DIVIDE          331U
#define KEY_KP_MULTIPLY        332U
#define KEY_KP_SUBTRACT        333U
#define KEY_KP_ADD             334U
#define KEY_KP_ENTER           335U
#define KEY_KP_EQUAL           336U
#define KEY_LEFT_SHIFT         340U
#define KEY_LEFT_CONTROL       341U
#define KEY_LEFT_ALT           342U
#define KEY_LEFT_SUPER         343U
#define KEY_RIGHT_SHIFT        344U
#define KEY_RIGHT_CONTROL      345U
#define KEY_RIGHT_ALT          346U
#define KEY_RIGHT_SUPER        347U
#define KEY_MENU               348U
#define KEY_LAST               KEY_MENU
#define KEY_COUNT              121U

#define MOUSE_BUTTON_LEFT      0
#define MOUSE_BUTTON_RIGHT     1
#define MOUSE_BUTTON_MIDDLE    2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
#define MOUSE_BUTTON_COUNT     8


bool window_init              (u32 width, u32 height, const char * title);
bool window_should_close      (void);
bool is_key_pressed           (u32 key_code);
bool is_key_released          (u32 key_code);
bool is_key_down              (u32 key_code);
bool is_key_up                (u32 key_code);
bool is_mouse_button_pressed  (u8 button_code);
bool is_mouse_button_released (u8 button_code);
bool is_mouse_button_down     (u8 button_code);
bool is_mouse_button_up       (u8 button_code);
void window_prepare           (void);
void window_swap_buffers      (void); 
void window_destroy           (void);
void window_clear_screen      (void);
void time_update              (void);
void input_update             (void);
f64  get_mouse_x              (void);
f64  get_mouse_y              (void);
u32  get_window_width         (void);
u32  get_window_height        (void);
f64  get_mouse_scroll_offset  (void);
f64  get_time                 (void);
f64  get_delta_time           (void);
