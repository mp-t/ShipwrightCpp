#include <cstdio>

#if !defined(__linux__) && defined(ENABLE_OPENGL)

#ifdef __MINGW32__
#define FOR_WINDOWS 1
#else
#define FOR_WINDOWS 0
#endif

#include "gfx_window_manager_api.h"
#include "gfx_screen_config.h"
#include <WTypesbase.h>
#include <time.h>

#include "../../SohImGuiImpl.h"

#if FOR_WINDOWS
#include <GL/glew.h>
#include "SDL.h"
#define GL_GLEXT_PROTOTYPES 1
#include "SDL_opengl.h"
#else
#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>
#endif

#define GFX_API_NAME "SDL2 - OpenGL"

static SDL_Window *wnd;
static int inverted_scancode_table[512];
static int vsync_enabled = 0;
static unsigned int window_width = DESIRED_SCREEN_WIDTH;
static unsigned int window_height = DESIRED_SCREEN_HEIGHT;
static bool fullscreen_state;
static void (*on_fullscreen_changed_callback)(bool is_now_fullscreen);
static bool (*on_key_down_callback)(int scancode);
static bool (*on_key_up_callback)(int scancode);
static void (*on_all_keys_up_callback)(void);

const SDL_Scancode windows_scancode_table[] =
{ 
    /*	0						1							2							3							4						5							6							7 */
    /*	8						9							A							B							C						D							E							F */
    SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_ESCAPE,		SDL_SCANCODE_1,				SDL_SCANCODE_2,				SDL_SCANCODE_3,			SDL_SCANCODE_4,				SDL_SCANCODE_5,				SDL_SCANCODE_6,			/* 0 */
    SDL_SCANCODE_7,				SDL_SCANCODE_8,				SDL_SCANCODE_9,				SDL_SCANCODE_0,				SDL_SCANCODE_MINUS,		SDL_SCANCODE_EQUALS,		SDL_SCANCODE_BACKSPACE,		SDL_SCANCODE_TAB,		/* 0 */

    SDL_SCANCODE_Q,				SDL_SCANCODE_W,				SDL_SCANCODE_E,				SDL_SCANCODE_R,				SDL_SCANCODE_T,			SDL_SCANCODE_Y,				SDL_SCANCODE_U,				SDL_SCANCODE_I,			/* 1 */
    SDL_SCANCODE_O,				SDL_SCANCODE_P,				SDL_SCANCODE_LEFTBRACKET,	SDL_SCANCODE_RIGHTBRACKET,	SDL_SCANCODE_RETURN,	SDL_SCANCODE_LCTRL,			SDL_SCANCODE_A,				SDL_SCANCODE_S,			/* 1 */

    SDL_SCANCODE_D,				SDL_SCANCODE_F,				SDL_SCANCODE_G,				SDL_SCANCODE_H,				SDL_SCANCODE_J,			SDL_SCANCODE_K,				SDL_SCANCODE_L,				SDL_SCANCODE_SEMICOLON,	/* 2 */
    SDL_SCANCODE_APOSTROPHE,	SDL_SCANCODE_GRAVE,			SDL_SCANCODE_LSHIFT,		SDL_SCANCODE_BACKSLASH,		SDL_SCANCODE_Z,			SDL_SCANCODE_X,				SDL_SCANCODE_C,				SDL_SCANCODE_V,			/* 2 */

    SDL_SCANCODE_B,				SDL_SCANCODE_N,				SDL_SCANCODE_M,				SDL_SCANCODE_COMMA,			SDL_SCANCODE_PERIOD,	SDL_SCANCODE_SLASH,			SDL_SCANCODE_RSHIFT,		SDL_SCANCODE_PRINTSCREEN,/* 3 */
    SDL_SCANCODE_LALT,			SDL_SCANCODE_SPACE,			SDL_SCANCODE_CAPSLOCK,		SDL_SCANCODE_F1,			SDL_SCANCODE_F2,		SDL_SCANCODE_F3,			SDL_SCANCODE_F4,			SDL_SCANCODE_F5,		/* 3 */

    SDL_SCANCODE_F6,			SDL_SCANCODE_F7,			SDL_SCANCODE_F8,			SDL_SCANCODE_F9,			SDL_SCANCODE_F10,		SDL_SCANCODE_NUMLOCKCLEAR,	SDL_SCANCODE_SCROLLLOCK,	SDL_SCANCODE_HOME,		/* 4 */
    SDL_SCANCODE_UP,			SDL_SCANCODE_PAGEUP,		SDL_SCANCODE_KP_MINUS,		SDL_SCANCODE_LEFT,			SDL_SCANCODE_KP_5,		SDL_SCANCODE_RIGHT,			SDL_SCANCODE_KP_PLUS,		SDL_SCANCODE_END,		/* 4 */

    SDL_SCANCODE_DOWN,			SDL_SCANCODE_PAGEDOWN,		SDL_SCANCODE_INSERT,		SDL_SCANCODE_DELETE,		SDL_SCANCODE_UNKNOWN,	SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_NONUSBACKSLASH,SDL_SCANCODE_F11,		/* 5 */
    SDL_SCANCODE_F12,			SDL_SCANCODE_PAUSE,			SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_LGUI,			SDL_SCANCODE_RGUI,		SDL_SCANCODE_APPLICATION,	SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,	/* 5 */

    SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_F13,		SDL_SCANCODE_F14,			SDL_SCANCODE_F15,			SDL_SCANCODE_F16,		/* 6 */
    SDL_SCANCODE_F17,			SDL_SCANCODE_F18,			SDL_SCANCODE_F19,			SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,	SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,	/* 6 */

    SDL_SCANCODE_INTERNATIONAL2,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_INTERNATIONAL1,		SDL_SCANCODE_UNKNOWN,	SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN,	/* 7 */
    SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_INTERNATIONAL4,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_INTERNATIONAL5,		SDL_SCANCODE_UNKNOWN,	SDL_SCANCODE_INTERNATIONAL3,		SDL_SCANCODE_UNKNOWN,		SDL_SCANCODE_UNKNOWN	/* 7 */
};

const SDL_Scancode scancode_rmapping_extended[][2] = {
    {SDL_SCANCODE_KP_ENTER, SDL_SCANCODE_RETURN},
    {SDL_SCANCODE_RALT, SDL_SCANCODE_LALT},
    {SDL_SCANCODE_RCTRL, SDL_SCANCODE_LCTRL},
    {SDL_SCANCODE_KP_DIVIDE, SDL_SCANCODE_SLASH},
    //{SDL_SCANCODE_KP_PLUS, SDL_SCANCODE_CAPSLOCK}
};

const SDL_Scancode scancode_rmapping_nonextended[][2] = {
    {SDL_SCANCODE_KP_7, SDL_SCANCODE_HOME},
    {SDL_SCANCODE_KP_8, SDL_SCANCODE_UP},
    {SDL_SCANCODE_KP_9, SDL_SCANCODE_PAGEUP},
    {SDL_SCANCODE_KP_4, SDL_SCANCODE_LEFT},
    {SDL_SCANCODE_KP_6, SDL_SCANCODE_RIGHT},
    {SDL_SCANCODE_KP_1, SDL_SCANCODE_END},
    {SDL_SCANCODE_KP_2, SDL_SCANCODE_DOWN},
    {SDL_SCANCODE_KP_3, SDL_SCANCODE_PAGEDOWN},
    {SDL_SCANCODE_KP_0, SDL_SCANCODE_INSERT},
    {SDL_SCANCODE_KP_PERIOD, SDL_SCANCODE_DELETE},
    {SDL_SCANCODE_KP_MULTIPLY, SDL_SCANCODE_PRINTSCREEN}
};

static void set_fullscreen(bool on, bool call_callback) {
    if (fullscreen_state == on) {
        return;
    }
    fullscreen_state = on;

    if (on) {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        window_width = mode.w;
        window_height = mode.h;
        SDL_ShowCursor(false);
    } else {
        window_width = DESIRED_SCREEN_WIDTH;
        window_height = DESIRED_SCREEN_HEIGHT;
    }
    SDL_SetWindowSize(wnd, window_width, window_height);
    SDL_SetWindowFullscreen(wnd, on ? SDL_WINDOW_FULLSCREEN : 0);
    SDL_SetCursor(SDL_DISABLE);

    if (on_fullscreen_changed_callback != NULL && call_callback) {
        on_fullscreen_changed_callback(on);
    }
}

static std::uint64_t previous_time;
static HANDLE timer;

static int frameDivisor = 1;

#define FRAME_INTERVAL_US_NUMERATOR_ 50000
#define FRAME_INTERVAL_US_DENOMINATOR 3
#define FRAME_INTERVAL_US_NUMERATOR (FRAME_INTERVAL_US_NUMERATOR_ * frameDivisor)

static void gfx_sdl_init(const char *game_name, bool start_in_fullscreen) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    timer = CreateWaitableTimer(nullptr, false, nullptr);

    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    char title[512];
    sprintf(title, "%s (%s)", game_name, GFX_API_NAME);

    wnd = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (start_in_fullscreen) {
        set_fullscreen(true, false);
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(wnd);

    SDL_GL_SetSwapInterval(1);

    SohImGui::WindowImpl window_impl;
    window_impl.backend = SohImGui::Backend::SDL;
    window_impl.sdl = { wnd, ctx };
    SohImGui::Init(window_impl);

    for (size_t i = 0; i < sizeof(windows_scancode_table) / sizeof(SDL_Scancode); i++) {
        inverted_scancode_table[windows_scancode_table[i]] = i;
    }

    for (size_t i = 0; i < sizeof(scancode_rmapping_extended) / sizeof(scancode_rmapping_extended[0]); i++) {
        inverted_scancode_table[scancode_rmapping_extended[i][0]] = inverted_scancode_table[scancode_rmapping_extended[i][1]] + 0x100;
    }

    for (size_t i = 0; i < sizeof(scancode_rmapping_nonextended) / sizeof(scancode_rmapping_nonextended[0]); i++) {
        inverted_scancode_table[scancode_rmapping_nonextended[i][0]] = inverted_scancode_table[scancode_rmapping_nonextended[i][1]];
        inverted_scancode_table[scancode_rmapping_nonextended[i][1]] += 0x100;
    }
}

static void gfx_sdl_set_fullscreen_changed_callback(void (*on_fullscreen_changed)(bool is_now_fullscreen)) {
    on_fullscreen_changed_callback = on_fullscreen_changed;
}

static void gfx_sdl_set_fullscreen(bool enable) {
    set_fullscreen(enable, true);
}

static void gfx_sdl_show_cursor(bool hide) {
    SDL_ShowCursor(hide);
}

static void gfx_sdl_set_keyboard_callbacks(bool (*on_key_down)(int scancode), bool (*on_key_up)(int scancode), void (*on_all_keys_up)(void)) {
    on_key_down_callback = on_key_down;
    on_key_up_callback = on_key_up;
    on_all_keys_up_callback = on_all_keys_up;
}

static void gfx_sdl_main_loop(void (*run_one_game_iter)(void)) {
    while (1) 
    {
        run_one_game_iter();
    }
}

static void gfx_sdl_get_dimensions(std::uint32_t *width, std::uint32_t *height) {
    *width = window_width;
    *height = window_height;
}

static int translate_scancode(int scancode) {
    if (scancode < 512) {
        return inverted_scancode_table[scancode];
    } else {
        return 0;
    }
}

static void gfx_sdl_onkeydown(int scancode) {
    int key = translate_scancode(scancode);
    if (on_key_down_callback != NULL) {
        on_key_down_callback(key);
    }
}

static void gfx_sdl_onkeyup(int scancode) {
    int key = translate_scancode(scancode);
    if (on_key_up_callback != NULL) {
        on_key_up_callback(key);
    }
}

static void gfx_sdl_handle_events(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        SohImGui::EventImpl event_impl;
        event_impl.sdl = { &event };
        SohImGui::Update(event_impl);
        switch (event.type) {
#ifndef TARGET_WEB
            // Scancodes are broken in Emscripten SDL2: https://bugzilla.libsdl.org/show_bug.cgi?id=3259
            case SDL_KEYDOWN:
                gfx_sdl_onkeydown(event.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                gfx_sdl_onkeyup(event.key.keysym.scancode);
                break;
#endif
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                }
                break;
            case SDL_QUIT:
                exit(0);
        }
    }
}

static bool gfx_sdl_start_frame(void) {
    return true;
}

static std::uint64_t qpc_to_100ns(std::uint64_t qpc) {
    const std::uint64_t qpc_freq = SDL_GetPerformanceFrequency();
    return qpc / qpc_freq * 10000000 + qpc % qpc_freq * 10000000 / qpc_freq;
}

static inline void sync_framerate_with_timer(void) {
    std::uint64_t t;
    t = SDL_GetPerformanceCounter();

    const std::int64_t next = qpc_to_100ns(previous_time) + 10 * FRAME_INTERVAL_US_NUMERATOR / FRAME_INTERVAL_US_DENOMINATOR;
    const std::int64_t left = next - qpc_to_100ns(t);
    if (left > 0) {
#ifdef __linux__
        const timespec spec = { 0, left * 100 };
        nanosleep(&spec, nullptr);
#else
        LARGE_INTEGER li;
        li.QuadPart = -left;
        SetWaitableTimer(timer, &li, 0, nullptr, nullptr, false);
        WaitForSingleObject(timer, INFINITE);
#endif
    }

    t = SDL_GetPerformanceCounter();
    previous_time = t;
}

static void gfx_sdl_swap_buffers_begin(void) {
    sync_framerate_with_timer();
    SDL_GL_SwapWindow(wnd);
}

static void gfx_sdl_swap_buffers_end(void) {

}

static double gfx_sdl_get_time(void) {
    return 0.0;
}

static void gfx_sdl_set_framedivisor(int divisor)
{
    frameDivisor = divisor;
}

struct GfxWindowManagerAPI gfx_sdl = {
    gfx_sdl_init,
    gfx_sdl_set_keyboard_callbacks,
    gfx_sdl_set_fullscreen_changed_callback,
    gfx_sdl_set_fullscreen,
    gfx_sdl_show_cursor,
    gfx_sdl_main_loop,
    gfx_sdl_get_dimensions,
    gfx_sdl_handle_events,
    gfx_sdl_start_frame,
    gfx_sdl_swap_buffers_begin,
    gfx_sdl_swap_buffers_end,
    gfx_sdl_get_time,
    gfx_sdl_set_framedivisor
};

#endif
