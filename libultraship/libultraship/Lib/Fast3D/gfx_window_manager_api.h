#ifndef GFX_WINDOW_MANAGER_API_H
#define GFX_WINDOW_MANAGER_API_H

#include <cstdint>

struct GfxWindowManagerAPI {
    void (*init)(const char *game_name, bool start_in_fullscreen);
    void (*set_keyboard_callbacks)(bool (*on_key_down)(int scancode), bool (*on_key_up)(int scancode), void (*on_all_keys_up)(void));
    void (*set_fullscreen_changed_callback)(void (*on_fullscreen_changed)(bool is_now_fullscreen));
    void (*set_fullscreen)(bool enable);
    void (*show_cursor)(bool hide);
    void (*main_loop)(void (*run_one_game_iter)(void));
    void (*get_dimensions)(std::uint32_t *width, std::uint32_t *height);
    void (*handle_events)(void);
    bool (*start_frame)(void);
    void (*swap_buffers_begin)(void);
    void (*swap_buffers_end)(void);
    double (*get_time)(void); // For debug
    void (*set_frame_divisor)(int);
};

#endif
