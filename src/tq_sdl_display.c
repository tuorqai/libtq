
//------------------------------------------------------------------------------
// tq library :3
//------------------------------------------------------------------------------

#include <SDL.h>

#include "tq_core.h"
#include "tq_error.h"
#include "tq_log.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------
// Declarations

typedef struct tq_sdl_gl_version
{
    int major;
    int minor;
    int flags;
} tq_sdl_gl_version_t;

typedef struct tq_sdl_display
{
    SDL_Window      *window;
    SDL_GLContext   gl_context;
    bool            key_autorepeat;
} tq_sdl_display_t;

//------------------------------------------------------------------------------
// Definitions

static tq_sdl_display_t sdl;

//------------------------------------------------------------------------------
// Utility functions

static tq_key_t key_conv(SDL_Keysym const *sym)
{
    switch (sym->scancode) {
        case SDL_SCANCODE_0:            return TQ_KEY_0;
        case SDL_SCANCODE_1:            return TQ_KEY_1;
        case SDL_SCANCODE_2:            return TQ_KEY_2;
        case SDL_SCANCODE_3:            return TQ_KEY_3;
        case SDL_SCANCODE_4:            return TQ_KEY_4;
        case SDL_SCANCODE_5:            return TQ_KEY_5;
        case SDL_SCANCODE_6:            return TQ_KEY_6;
        case SDL_SCANCODE_7:            return TQ_KEY_7;
        case SDL_SCANCODE_8:            return TQ_KEY_8;
        case SDL_SCANCODE_9:            return TQ_KEY_9;
        case SDL_SCANCODE_A:            return TQ_KEY_A;
        case SDL_SCANCODE_B:            return TQ_KEY_B;
        case SDL_SCANCODE_C:            return TQ_KEY_C;
        case SDL_SCANCODE_D:            return TQ_KEY_D;
        case SDL_SCANCODE_E:            return TQ_KEY_E;
        case SDL_SCANCODE_F:            return TQ_KEY_F;
        case SDL_SCANCODE_G:            return TQ_KEY_G;
        case SDL_SCANCODE_H:            return TQ_KEY_H;
        case SDL_SCANCODE_I:            return TQ_KEY_I;
        case SDL_SCANCODE_J:            return TQ_KEY_J;
        case SDL_SCANCODE_K:            return TQ_KEY_K;
        case SDL_SCANCODE_L:            return TQ_KEY_L;
        case SDL_SCANCODE_M:            return TQ_KEY_M;
        case SDL_SCANCODE_N:            return TQ_KEY_N;
        case SDL_SCANCODE_O:            return TQ_KEY_O;
        case SDL_SCANCODE_P:            return TQ_KEY_P;
        case SDL_SCANCODE_Q:            return TQ_KEY_Q;
        case SDL_SCANCODE_R:            return TQ_KEY_R;
        case SDL_SCANCODE_S:            return TQ_KEY_S;
        case SDL_SCANCODE_T:            return TQ_KEY_T;
        case SDL_SCANCODE_U:            return TQ_KEY_U;
        case SDL_SCANCODE_V:            return TQ_KEY_V;
        case SDL_SCANCODE_W:            return TQ_KEY_W;
        case SDL_SCANCODE_X:            return TQ_KEY_X;
        case SDL_SCANCODE_Y:            return TQ_KEY_Y;
        case SDL_SCANCODE_Z:            return TQ_KEY_Z;
        case SDL_SCANCODE_GRAVE:        return TQ_KEY_GRAVE;
        case SDL_SCANCODE_APOSTROPHE:   return TQ_KEY_APOSTROPHE;
        case SDL_SCANCODE_MINUS:        return TQ_KEY_MINUS;
        case SDL_SCANCODE_EQUALS:       return TQ_KEY_EQUAL;
        case SDL_SCANCODE_LEFTBRACKET:  return TQ_KEY_LBRACKET;
        case SDL_SCANCODE_RIGHTBRACKET: return TQ_KEY_RBRACKET;
        case SDL_SCANCODE_COMMA:        return TQ_KEY_COMMA;
        case SDL_SCANCODE_PERIOD:       return TQ_KEY_PERIOD;
        case SDL_SCANCODE_SEMICOLON:    return TQ_KEY_SEMICOLON;
        case SDL_SCANCODE_SLASH:        return TQ_KEY_SLASH;
        case SDL_SCANCODE_BACKSLASH:    return TQ_KEY_BACKSLASH;
        case SDL_SCANCODE_SPACE:        return TQ_KEY_SPACE;
        case SDL_SCANCODE_ESCAPE:       return TQ_KEY_ESCAPE;
        case SDL_SCANCODE_BACKSPACE:    return TQ_KEY_BACKSPACE;
        case SDL_SCANCODE_TAB:          return TQ_KEY_TAB;
        case SDL_SCANCODE_RETURN:       return TQ_KEY_ENTER;
        case SDL_SCANCODE_F1:           return TQ_KEY_F1;
        case SDL_SCANCODE_F2:           return TQ_KEY_F2;
        case SDL_SCANCODE_F3:           return TQ_KEY_F3;
        case SDL_SCANCODE_F4:           return TQ_KEY_F4;
        case SDL_SCANCODE_F5:           return TQ_KEY_F5;
        case SDL_SCANCODE_F6:           return TQ_KEY_F6;
        case SDL_SCANCODE_F7:           return TQ_KEY_F7;
        case SDL_SCANCODE_F8:           return TQ_KEY_F8;
        case SDL_SCANCODE_F9:           return TQ_KEY_F9;
        case SDL_SCANCODE_F10:          return TQ_KEY_F10;
        case SDL_SCANCODE_F11:          return TQ_KEY_F11;
        case SDL_SCANCODE_F12:          return TQ_KEY_F12;
        case SDL_SCANCODE_UP:           return TQ_KEY_UP;
        case SDL_SCANCODE_DOWN:         return TQ_KEY_DOWN;
        case SDL_SCANCODE_LEFT:         return TQ_KEY_LEFT;
        case SDL_SCANCODE_RIGHT:        return TQ_KEY_RIGHT;
        case SDL_SCANCODE_LSHIFT:       return TQ_KEY_LSHIFT;
        case SDL_SCANCODE_RSHIFT:       return TQ_KEY_RSHIFT;
        case SDL_SCANCODE_LCTRL:        return TQ_KEY_LCTRL;
        case SDL_SCANCODE_RCTRL:        return TQ_KEY_RCTRL;
        case SDL_SCANCODE_LALT:         return TQ_KEY_LALT;
        case SDL_SCANCODE_RALT:         return TQ_KEY_RALT;
        case SDL_SCANCODE_LGUI:         return TQ_KEY_LSUPER;
        case SDL_SCANCODE_RGUI:         return TQ_KEY_RSUPER;
        case SDL_SCANCODE_MENU:         return TQ_KEY_MENU;
        case SDL_SCANCODE_PAGEUP:       return TQ_KEY_PGUP;
        case SDL_SCANCODE_PAGEDOWN:     return TQ_KEY_PGDN;
        case SDL_SCANCODE_HOME:         return TQ_KEY_HOME;
        case SDL_SCANCODE_END:          return TQ_KEY_END;
        case SDL_SCANCODE_INSERT:       return TQ_KEY_INSERT;
        case SDL_SCANCODE_DELETE:       return TQ_KEY_DELETE;
        case SDL_SCANCODE_PAUSE:        return TQ_KEY_PAUSE;
        case SDL_SCANCODE_KP_0:         return TQ_KEY_KP_0;
        case SDL_SCANCODE_KP_1:         return TQ_KEY_KP_1;
        case SDL_SCANCODE_KP_2:         return TQ_KEY_KP_2;
        case SDL_SCANCODE_KP_3:         return TQ_KEY_KP_3;
        case SDL_SCANCODE_KP_4:         return TQ_KEY_KP_4;
        case SDL_SCANCODE_KP_5:         return TQ_KEY_KP_5;
        case SDL_SCANCODE_KP_6:         return TQ_KEY_KP_6;
        case SDL_SCANCODE_KP_7:         return TQ_KEY_KP_7;
        case SDL_SCANCODE_KP_8:         return TQ_KEY_KP_8;
        case SDL_SCANCODE_KP_9:         return TQ_KEY_KP_9;
        case SDL_SCANCODE_KP_MULTIPLY:  return TQ_KEY_KP_MUL;
        case SDL_SCANCODE_KP_PLUS:      return TQ_KEY_KP_ADD;
        case SDL_SCANCODE_KP_MINUS:     return TQ_KEY_KP_SUB;
        case SDL_SCANCODE_KP_DIVIDE:    return TQ_KEY_KP_DIV;
        default:                        return TQ_TOTAL_KEYS;
    }
}

static tq_mouse_button_t mouse_button_conv(int button)
{
    if (button == SDL_BUTTON_LEFT) {
        return TQ_MOUSE_BUTTON_LEFT;
    } else if (button == SDL_BUTTON_RIGHT) {
        return TQ_MOUSE_BUTTON_RIGHT;
    } else if (button == SDL_BUTTON_MIDDLE) {
        return TQ_MOUSE_BUTTON_MIDDLE;
    }

    return TQ_TOTAL_MOUSE_BUTTONS;
}

//------------------------------------------------------------------------------
// Implementation

static void initialize(uint32_t a0, uint32_t a1, char const *a2)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        tq_error("Failed to initialize SDL: %s", SDL_GetError());
    }

    int width, height;
    tq_core_get_display_size(&width, &height);

    tq_sdl_gl_version_t versions[] = {
        { 4, 6, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 4, 5, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 4, 4, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 4, 3, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 4, 2, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 4, 1, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 4, 0, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 3, 3, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 3, 2, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
        { 3, 1, 0 },
        { 3, 0, 0 },
        { 2, 1, 0 },
    };

    for (size_t n = 0; n < sizeof(versions) / sizeof(versions[0]); n++) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, versions[n].major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, versions[n].minor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, versions[n].flags);

        sdl.window = SDL_CreateWindow(
            tq_core_get_title(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        if (sdl.window != NULL) {
            tq_log_info("Created SDL window, requested OpenGL %d.%d\n",
                versions[n].major, versions[n].minor);
            break;
        }
    }

    if (sdl.window == NULL) {
        tq_error("Failed to create SDL window: %s", SDL_GetError());
    }

    sdl.gl_context = SDL_GL_CreateContext(sdl.window);

    if (sdl.gl_context == NULL) {
        tq_error("Failed to create OpenGL context: %s", SDL_GetError());
    }

    if (SDL_GL_MakeCurrent(sdl.window, sdl.gl_context) < 0) {
        tq_error("Failed to activate OpenGL context: %s", SDL_GetError());
    }

    SDL_GL_SetSwapInterval(1);

    SDL_SetWindowMinimumSize(sdl.window, 256, 256);

    sdl.key_autorepeat = tq_core_is_key_autorepeat_enabled();

    tq_log_info("SDL-based display module initialized.\n");
}

static void terminate(void)
{
    SDL_GL_DeleteContext(sdl.gl_context);
    SDL_DestroyWindow(sdl.window);

    tq_log_info("SDL-based display module terminated.\n");
}

static void present(void)
{
    SDL_GL_SwapWindow(sdl.window);
}

static bool process_events(void)
{
    SDL_Event event;
    bool running = true;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.repeat && !sdl.key_autorepeat) {
                break;
            }
            tq_core_on_key_pressed(key_conv(&event.key.keysym));
            break;
        case SDL_KEYUP:
            tq_core_on_key_released(key_conv(&event.key.keysym));
            break;
        case SDL_MOUSEBUTTONDOWN:
            tq_core_on_mouse_button_pressed(mouse_button_conv(event.button.button));
            break;
        case SDL_MOUSEBUTTONUP:
            tq_core_on_mouse_button_released(mouse_button_conv(event.button.button));
            break;
        case SDL_MOUSEMOTION:
            tq_core_on_mouse_cursor_moved(event.button.x, event.button.y);
            break;
        case SDL_MOUSEWHEEL:
            if (event.wheel.y) {
                tq_core_on_mouse_wheel_scrolled((float) event.wheel.x, (float) event.wheel.y);
            }
            break;
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                tq_core_on_display_resized(event.window.data1, event.window.data2);
                break;
            }
            break;
        case SDL_QUIT:
            running = false;
            break;
        }
    }

    return running;
}

static void set_size(uint32_t width, uint32_t height)
{
    SDL_SetWindowSize(sdl.window, width, height);
}

static void set_title(char const *title)
{
    SDL_SetWindowTitle(sdl.window, title);
}

static void set_key_autorepeat_enabled(bool enabled)
{
    sdl.key_autorepeat = enabled;
}

static void show_message_box(char const *title, char const *message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, sdl.window);
}

//------------------------------------------------------------------------------

void tq_construct_sdl_display(tq_display_t *display)
{
    display->initialize             = initialize;
    display->terminate              = terminate;
    display->present                = present;
    display->process_events         = process_events;
    display->set_size               = set_size;
    display->set_title              = set_title;
    display->set_key_autorepeat_enabled = set_key_autorepeat_enabled;
    display->show_message_box       = show_message_box;
}

//------------------------------------------------------------------------------
