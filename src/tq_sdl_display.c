
//------------------------------------------------------------------------------
// Copyright (c) 2021-2022 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// tq library: SDL display implementation
//------------------------------------------------------------------------------

#if defined(TQ_LINUX)

//------------------------------------------------------------------------------

#include <SDL.h>

#include "tq_core.h"
#include "tq_error.h"
#include "tq_graphics.h"
#include "tq_log.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------
// Declarations

struct sdl_display_priv
{
    SDL_Window      *window;
    SDL_GLContext   gl_context;
    bool            key_autorepeat;
};

//------------------------------------------------------------------------------
// Definitions

static struct sdl_display_priv sdl;

//------------------------------------------------------------------------------
// Utility functions

static tq_key key_conv(SDL_Keysym const *sym)
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

static tq_mouse_button mouse_button_conv(int button)
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

static void initialize(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        libtq_error("Failed to initialize SDL: %s", SDL_GetError());
    }

    int width, height;
    libtq_get_display_size(&width, &height);

#ifdef TQ_USE_GLES2
    int gl_versions[] = { 300, 200 };
#else
    int gl_versions[] = { 460, 450, 440, 430, 420, 410, 400, 330 };
#endif

    for (size_t n = 0; n < SDL_arraysize(gl_versions); n++) {
        int major = gl_versions[n] / 100;
        int minor = (gl_versions[n] % 100) / 10;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

#ifdef TQ_USE_GLES2
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

        sdl.window = SDL_CreateWindow(
            libtq_get_title(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
        );

        if (!sdl.window) {
            libtq_error("Failed to create SDL window: %s", SDL_GetError());
        }

        sdl.gl_context = SDL_GL_CreateContext(sdl.window);

        if (!sdl.gl_context) {
            SDL_DestroyWindow(sdl.window);
            continue;
        }

        libtq_log(0, "Created OpenGL context, version %d.%d\n", major, minor);
        break;
    }

    if (!sdl.gl_context) {
        libtq_error("Failed to create OpenGL context: %s", SDL_GetError());
    }

    if (SDL_GL_MakeCurrent(sdl.window, sdl.gl_context) < 0) {
        libtq_error("Failed to activate OpenGL context: %s", SDL_GetError());
    }

    libtq_on_rc_create(1);

    SDL_ShowWindow(sdl.window);
    SDL_SetWindowMinimumSize(sdl.window, 256, 256);

    SDL_GL_SetSwapInterval(1);

    sdl.key_autorepeat = libtq_is_key_autorepeat_enabled();

    libtq_log(0, "SDL window initialized.\n");
}

static void terminate(void)
{
    libtq_on_rc_destroy();

    SDL_GL_DeleteContext(sdl.gl_context);
    SDL_DestroyWindow(sdl.window);

    libtq_log(0, "SDL window terminated.\n");
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
            libtq_on_key_pressed(key_conv(&event.key.keysym));
            break;
        case SDL_KEYUP:
            libtq_on_key_released(key_conv(&event.key.keysym));
            break;
        case SDL_MOUSEBUTTONDOWN:
            libtq_on_mouse_button_pressed(mouse_button_conv(event.button.button));
            break;
        case SDL_MOUSEBUTTONUP:
            libtq_on_mouse_button_released(mouse_button_conv(event.button.button));
            break;
        case SDL_MOUSEMOTION:
            libtq_on_mouse_cursor_moved(event.button.x, event.button.y);
            break;
        case SDL_MOUSEWHEEL:
            libtq_on_mouse_wheel_scrolled(event.wheel.preciseX, event.wheel.preciseY);
            break;
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                libtq_on_display_resize(event.window.data1, event.window.data2);
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

static void set_mouse_cursor_hidden(bool hidden)
{
    SDL_ShowCursor(hidden ? SDL_DISABLE : SDL_ENABLE);
}

static void show_message_box(char const *title, char const *message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, sdl.window);
}

static void *get_gl_proc_addr(char const *name)
{
    return SDL_GL_GetProcAddress(name);
}

static bool check_gl_ext(char const *name)
{
    return SDL_GL_ExtensionSupported(name);
}

//------------------------------------------------------------------------------

void libtq_construct_sdl_display(struct libtq_display_impl *display)
{
    display->initialize             = initialize;
    display->terminate              = terminate;
    display->present                = present;
    display->process_events         = process_events;
    display->set_size               = set_size;
    display->set_title              = set_title;
    display->set_key_autorepeat_enabled = set_key_autorepeat_enabled;
    display->set_mouse_cursor_hidden = set_mouse_cursor_hidden;
    display->show_message_box       = show_message_box;
    display->get_gl_proc_addr       = get_gl_proc_addr;
    display->check_gl_ext           = check_gl_ext;
}

//------------------------------------------------------------------------------

#endif // defined(TQ_LINUX)

//------------------------------------------------------------------------------