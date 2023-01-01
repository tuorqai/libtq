//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
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

#if defined(_WIN32)

//------------------------------------------------------------------------------

#include "tchar.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "windowsx.h"

#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_log.h"
#include "tq_error.h"

//------------------------------------------------------------------------------
// WGL_ARB_multisample (#5)

#define WGL_SAMPLE_BUFFERS_ARB                      0x2041
#define WGL_SAMPLES_ARB                             0x2042

//------------------------------------------------------------------------------
// WGL_ARB_extensions_string (#8)

typedef char const *(WINAPI *WGLGETEXTENSIONSSTRINGARBPROC)(HDC);

//------------------------------------------------------------------------------
// WGL_ARB_pixel_format (#9)

#define WGL_DRAW_TO_WINDOW_ARB                      0x2001
#define WGL_ACCELERATION_ARB                        0x2003
#define WGL_SUPPORT_OPENGL_ARB                      0x2010
#define WGL_DOUBLE_BUFFER_ARB                       0x2011
#define WGL_PIXEL_TYPE_ARB                          0x2013
#define WGL_COLOR_BITS_ARB                          0x2014
#define WGL_DEPTH_BITS_ARB                          0x2022
#define WGL_STENCIL_BITS_ARB                        0x2023

#define WGL_FULL_ACCELERATION_ARB                   0x2027
#define WGL_TYPE_RGBA_ARB                           0x202B

typedef BOOL (WINAPI *WGLGETPIXELFORMATATTRIBIVARBPROC)(HDC hdc,
    int iPixelFormat, int iLayerPlane, UINT nAttributes,
    const int *piAttributes, int *piValues);

typedef BOOL (WINAPI *WGLCHOOSEPIXELFORMATARBPROC)(HDC,
    int const *, FLOAT const *,
    UINT, int *, UINT *);

//------------------------------------------------------------------------------
// WGL_ARB_create_context (#55), WGL_ARB_create_context_profile (#74),
// WGL_EXT_create_context_es2_profile (#400)

#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB                0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x0001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x0002
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT             0x0004

typedef HGLRC (WINAPI *WGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, int const *);

//------------------------------------------------------------------------------
// WGL_EXT_swap_control (#172)

typedef BOOL (WINAPI *WGLSWAPINTERVALEXTPROC)(int);

//------------------------------------------------------------------------------

static WGLGETEXTENSIONSSTRINGARBPROC        wglGetExtensionsStringARB;
static WGLGETPIXELFORMATATTRIBIVARBPROC     wglGetPixelFormatAttribivARB;
static WGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB;
static WGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB;
static WGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT;

//------------------------------------------------------------------------------

struct libtq_win32_display_priv
{
    LPCTSTR     class_name;
    LPCTSTR     window_name;
    DWORD       style;
    HWND        window;
    HDC         dc;
    HGLRC       rc;
    HCURSOR     cursor;
    BOOL        hide_cursor;
    BOOL        key_autorepeat;
    UINT        mouse_button_ordinal;
    UINT        mouse_buttons;

#if defined(UNICODE)
    WCHAR       wide_title[256];
#endif
};

static struct libtq_win32_display_priv priv;

//------------------------------------------------------------------------------

static void     initialize(void);
static void     terminate(void);
static void     present(void);
static bool     process_events(void);
static void     set_size(uint32_t, uint32_t);
static void     set_title(char const *);
static void     set_key_autorepeat_enabled(bool enabled);
static void     set_mouse_cursor_hidden(bool hidden);
static void     show_message_box(char const *title, char const *message);
static void     *get_gl_proc_addr(char const *name);
static bool     check_gl_ext(char const *name);

static LRESULT CALLBACK wndproc(HWND, UINT, WPARAM, LPARAM);
static int      init_wgl_context(HWND);
static int      init_wgl_extensions(void);
static int      choose_pixel_format(HDC);
static tq_key   key_conv(WPARAM wp, LPARAM lp);
static tq_mouse_button mb_conv(UINT message, WPARAM wp);

//------------------------------------------------------------------------------

void libtq_construct_win32_display(struct libtq_display_impl *display)
{
    *display = (struct libtq_display_impl) {
        .initialize = initialize,
        .terminate = terminate,
        .present = present,
        .process_events = process_events,
        .set_size = set_size,
        .set_title = set_title,
        .set_key_autorepeat_enabled = set_key_autorepeat_enabled,
        .set_mouse_cursor_hidden = set_mouse_cursor_hidden,
        .show_message_box = show_message_box,
        .get_gl_proc_addr = get_gl_proc_addr,
        .check_gl_ext = check_gl_ext,
    };
}

//------------------------------------------------------------------------------

void initialize(void)
{
    char const *title = libtq_get_title();

#if defined(UNICODE)
    MultiByteToWideChar(CP_UTF8, 0, title, -1, priv.wide_title, ARRAYSIZE(priv.wide_title));

    priv.class_name = priv.wide_title;
    priv.window_name = priv.wide_title;
#else
    priv.class_name = title;
    priv.window_name = title;
#endif

    priv.style = WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

    // -- Get current instance handle

    HINSTANCE instance = GetModuleHandle(NULL);

    if (!instance) {
        libtq_error("WinAPI: no module instance.");
    }

    // -- Cursor?

    priv.cursor = LoadCursor(NULL, IDC_CROSS);
    priv.hide_cursor = FALSE;

    priv.key_autorepeat = tq_is_key_autorepeat_enabled();

    // -- Register window class and create window

    WNDCLASSEX wcex = {
        .cbSize         = sizeof(WNDCLASSEX),
        .style          = CS_VREDRAW | CS_HREDRAW | CS_OWNDC,
        .lpfnWndProc    = wndproc,
        .cbClsExtra     = 0,
        .cbWndExtra     = 0,
        .hInstance      = instance,
        .hIcon          = LoadIcon(NULL, IDI_WINLOGO),
        .hCursor        = NULL,
        .hbrBackground  = NULL,
        .lpszMenuName   = NULL,
        .lpszClassName  = priv.class_name,
        .hIconSm        = NULL,
    };

    if (!RegisterClassEx(&wcex)) {
        libtq_error("WinAPI: failed to register class.");
    }

    priv.window = CreateWindow(priv.class_name, priv.window_name, priv.style,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, instance, NULL);
    
    if (!priv.window) {
        libtq_error("WinAPI: failed to create window.");
    }

    // -- Set initial window size, place it in the center of
    // the default monitor and show it

    tq_vec2i display_size = tq_get_display_size();
    set_size(display_size.x, display_size.y);

    // -- Done.

    libtq_log(LIBTQ_INFO, "WinAPI: display initialized.\n");
}

void terminate(void)
{
    if (priv.window) {
        if (priv.dc) {
            if (priv.rc) {
                wglMakeCurrent(priv.dc, NULL);
                wglDeleteContext(priv.rc);
            }

            ReleaseDC(priv.window, priv.dc);
        }

        DestroyWindow(priv.window);
    }

    libtq_log(LIBTQ_INFO, "WinAPI: display terminated.\n");
}

void present(void)
{
    SwapBuffers(priv.dc);
}

bool process_events(void)
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

void set_size(uint32_t width, uint32_t height)
{
    HMONITOR monitor = MonitorFromWindow(priv.window, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfo(monitor, &mi);

    int dx = (mi.rcMonitor.right - mi.rcMonitor.left - width) / 2;
    int dy = (mi.rcMonitor.bottom - mi.rcMonitor.top - height) / 2;

    RECT rect = { dx, dy, dx + width, dy + height };
    AdjustWindowRect(&rect, priv.style, FALSE);

    int x = rect.left;
    int y = rect.top;
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    SetWindowPos(priv.window, NULL, x, y, w, h, SWP_SHOWWINDOW);
    UpdateWindow(priv.window);
}

void set_title(char const *title)
{
#if defined(UNICODE)
    MultiByteToWideChar(CP_UTF8, 0, title, -1, priv.wide_title, ARRAYSIZE(priv.wide_title));
    SetWindowText(priv.window, priv.wide_title);
#else
    SetWindowText(priv.window, title);
#endif
}

void set_key_autorepeat_enabled(bool enabled)
{
    priv.key_autorepeat = enabled;
}

void set_mouse_cursor_hidden(bool hidden)
{
    priv.hide_cursor = hidden;
}

void show_message_box(char const *title, char const *message)
{
    MessageBox(NULL, message, title, MB_OK | MB_ICONSTOP);
}

void *get_gl_proc_addr(char const *name)
{
    return (void *) wglGetProcAddress(name);
}

bool check_gl_ext(char const *name)
{
    if (!wglGetExtensionsStringARB) {
        return false;
    }

    char const *str = wglGetExtensionsStringARB(priv.dc);

    if (!str) {
        return false;
    }

    char *token = strtok(str, " ");

    while (token) {
        if (strcmp(token, name) == 0) {
            return true;
        }

        token = strtok(NULL, " ");
    }

    return false;
}

//------------------------------------------------------------------------------

LRESULT wndproc(HWND window, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE:
        return init_wgl_context(window);
    case WM_DESTROY:
        tq_on_rc_destroy();
        return 0;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        libtq_on_display_resize(LOWORD(lp), HIWORD(lp));
        return 0;
    case WM_ACTIVATE:
        if (LOWORD(wp) == WA_ACTIVE && HIWORD(wp) == 0) {
            // Send release message for all pressed buttons when
            // switching from another window
            for (int i = 0; i < TQ_TOTAL_MOUSE_BUTTONS; i++) {
                if (priv.mouse_buttons & (1 << i)) {
                    libtq_on_mouse_button_released(i);
                }
            }

            priv.mouse_buttons = 0;
            libtq_on_focus_gain();
        } else if (LOWORD(wp) == WA_INACTIVE) {
            if (priv.mouse_button_ordinal > 0) {
                priv.mouse_button_ordinal = 0;
                ReleaseCapture();
            }

            libtq_on_focus_loss();
        }
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if ((lp & 0x40000000) && !priv.key_autorepeat) {
            break;
        }
        libtq_on_key_pressed(key_conv(wp, lp));
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        libtq_on_key_released(key_conv(wp, lp));
        return 0;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
        if (priv.mouse_button_ordinal == 0) {
            SetCapture(priv.window);
        }
        priv.mouse_button_ordinal++;
        priv.mouse_buttons |= (1 << mb_conv(msg, wp));
        libtq_on_mouse_button_pressed(mb_conv(msg, wp));
        return 0;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
        priv.mouse_button_ordinal--;
        if (priv.mouse_button_ordinal == 0) {
            ReleaseCapture();
        }
        priv.mouse_buttons &= ~(1 << mb_conv(msg, wp));
        libtq_on_mouse_button_released(mb_conv(msg, wp));
        return 0;
    case WM_MOUSEMOVE:
        libtq_on_mouse_cursor_moved(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        return 0;
    case WM_MOUSEWHEEL:
        libtq_on_mouse_wheel_scrolled(0.0f, GET_WHEEL_DELTA_WPARAM(wp) / (float) WHEEL_DELTA);
        return 0;
    case WM_MOUSEHWHEEL:
        libtq_on_mouse_wheel_scrolled(GET_WHEEL_DELTA_WPARAM(wp) / (float) WHEEL_DELTA, 0.0f);
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT) {
            SetCursor(priv.hide_cursor ? NULL : priv.cursor);
            return 0;
        }
        break;
    }

    return DefWindowProc(window, msg, wp, lp);
}

int init_wgl_context(HWND window)
{
    if (init_wgl_extensions() == -1) {
        libtq_log(LIBTQ_ERROR, "WGL: Failed to fetch extension list.\n");
        return -1;
    }

    HDC dc = GetDC(window);

    if (!dc) {
        libtq_log(LIBTQ_ERROR, "WGL: Failed to get Device Context for main window.\n");
        return -1;
    }

    libtq_log(LIBTQ_INFO, "WGL: available extensions:\n");
    libtq_log(LIBTQ_INFO, ":: %s\n", wglGetExtensionsStringARB(dc));

    int format = choose_pixel_format(dc);

    if (format == -1) {
        libtq_log(LIBTQ_ERROR, "WGL: Failed to choose appropriate Pixel Format.\n");

        ReleaseDC(window, dc);
        return -1;
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(dc, format, sizeof(pfd), &pfd);

    if (!SetPixelFormat(dc, format, &pfd)) {
        libtq_log(LIBTQ_ERROR, "WGL: Failed to set Pixel Format.\n");
        libtq_log(LIBTQ_ERROR, ":: GetLastError -> 0x%08x\n", GetLastError());

        ReleaseDC(window, dc);
        return -1;
    }

    HGLRC rc = NULL;
    int gl_versions[] = { 460, 450, 440, 430, 420, 410, 400, 330 };

    for (int i = 0; i < ARRAYSIZE(gl_versions); i++) {
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,  (gl_versions[i] / 100),
            WGL_CONTEXT_MINOR_VERSION_ARB,  (gl_versions[i] % 100) / 10,
            WGL_CONTEXT_PROFILE_MASK_ARB,   (WGL_CONTEXT_CORE_PROFILE_BIT_ARB),
            0,
        };

        rc = wglCreateContextAttribsARB(dc, NULL, attribs);

        if (rc && wglMakeCurrent(dc, rc)) {
            libtq_log(LIBTQ_INFO, "WGL: OpenGL version %d seems to be supported.\n", gl_versions[i]);
            break;
        }

        libtq_log(LIBTQ_ERROR, "WGL: Unable to create OpenGL version %d context.\n", gl_versions[i]);
    }

    if (!rc) {
        libtq_log(LIBTQ_ERROR, "WGL: Neither of listed OpenGL versions is supported.\n");

        ReleaseDC(window, dc);
        return -1;
    }

    wglSwapIntervalEXT = (WGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");

    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(1);
    }

    priv.dc = dc;
    priv.rc = rc;

    libtq_log(LIBTQ_INFO, "WGL: OpenGL context is successfully created.\n");

    tq_on_rc_create(1);

    return 0;
}

int init_wgl_extensions(void)
{
    // Create dummy window and OpenGL context and check for
    // available WGL extensions

    libtq_log(LIBTQ_INFO, "WinAPI: Creating dummy invisible window to check supported WGL extensions.\n");

    WNDCLASS wc = {
        .style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc    = DefWindowProc,
        .hInstance      = GetModuleHandle(0),
        .lpszClassName  = TEXT("Trampoline"),
    };

    if (!RegisterClass(&wc)) {
        libtq_log(LIBTQ_ERROR, "WinAPI: Unable to register dummy window class.\n");
        return -1;
    }

    HWND window = CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, 0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, wc.hInstance, NULL);

    if (!window) {
        libtq_log(LIBTQ_ERROR, "WinAPI: Unable to create dummy window.\n");
        return -1;
    }

    HDC dc = GetDC(window);

    if (!dc) {
        libtq_log(LIBTQ_ERROR, "WinAPI: Dummy window has invalid Device Context.\n");

        DestroyWindow(window);
        return -1;
    }

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize          = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion       = 1,
        .dwFlags        = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
        .iPixelType     = PFD_TYPE_RGBA,
        .cColorBits     = 32,
        .cAlphaBits     = 8,
        .iLayerType     = PFD_MAIN_PLANE,
    };

    int format = ChoosePixelFormat(dc, &pfd);

    if (!format || !SetPixelFormat(dc, format, &pfd)) {
        libtq_log(LIBTQ_ERROR, "WinAPI: Invalid pixel format.\n");

        ReleaseDC(window, dc);
        DestroyWindow(window);
        return -1;
    }

    HGLRC rc = wglCreateContext(dc);

    if (!rc || !wglMakeCurrent(dc, rc)) {
        libtq_log(LIBTQ_ERROR, "WinAPI: Failed to create dummy OpenGL context.\n");

        if (rc) {
            wglDeleteContext(rc);
        }

        ReleaseDC(window, dc);
        DestroyWindow(window);
        return -1;
    }

    wglGetExtensionsStringARB = (WGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");
    wglGetPixelFormatAttribivARB = (WGLGETPIXELFORMATATTRIBIVARBPROC) wglGetProcAddress("wglGetPixelFormatAttribivARB");
    wglChoosePixelFormatARB = (WGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

    wglMakeCurrent(dc, NULL);
    wglDeleteContext(rc);
    ReleaseDC(window, dc);
    DestroyWindow(window);

    libtq_log(LIBTQ_INFO, "WGL: Pointers to required functions acquired:\n");
    libtq_log(LIBTQ_INFO, ":: wglGetExtensionsStringARB -> %p\n", wglGetExtensionsStringARB);
    libtq_log(LIBTQ_INFO, ":: wglGetPixelFormatAttribivARB -> %p\n", wglGetPixelFormatAttribivARB);
    libtq_log(LIBTQ_INFO, ":: wglChoosePixelFormatARB -> %p\n", wglChoosePixelFormatARB);
    libtq_log(LIBTQ_INFO, ":: wglCreateContextAttribsARB -> %p\n", wglCreateContextAttribsARB);

    // These functions are mandatory to create OpenGL Core Profile Context.

    if (!wglGetExtensionsStringARB) {
        libtq_log(LIBTQ_ERROR, "WGL: required function wglGetExtensionsStringARB() is unavailable.\n");
        return -1;
    }

    if (!wglGetPixelFormatAttribivARB) {
        libtq_log(LIBTQ_ERROR, "WGL: required function wglGetPixelFormatAttribivARB() is unavailable.\n");
        return -1;
    }

    if (!wglChoosePixelFormatARB) {
        libtq_log(LIBTQ_ERROR, "WGL: required function wglChoosePixelFormatARB() is unavailable.\n");
        return -1;
    }

    if (!wglCreateContextAttribsARB) {
        libtq_log(LIBTQ_ERROR, "WGL: required function wglCreateContextAttribsARB() is unavailable.\n");
        return -1;
    }

    return 0;
}

int choose_pixel_format(HDC dc)
{
    int format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     (TRUE),
        WGL_ACCELERATION_ARB,       (WGL_FULL_ACCELERATION_ARB),
        WGL_SUPPORT_OPENGL_ARB,     (TRUE),
        WGL_DOUBLE_BUFFER_ARB,      (TRUE),
        WGL_PIXEL_TYPE_ARB,         (WGL_TYPE_RGBA_ARB),
        WGL_COLOR_BITS_ARB,         (32),
        WGL_DEPTH_BITS_ARB,         (24),
        WGL_STENCIL_BITS_ARB,       (8),
        0,
    };

    int formats[256];
    UINT total_formats = 0;
    wglChoosePixelFormatARB(dc, format_attribs, NULL, 256, formats, &total_formats);

    if (!total_formats) {
        libtq_log(LIBTQ_ERROR, "WGL: No suitable Pixel Format found.\n");
        return -1;
    }

    int best_format = -1;
    int best_samples = 0;

    for (unsigned int i = 0; i < total_formats; i++) {
        int attribs[] = {
            WGL_SAMPLE_BUFFERS_ARB,
            WGL_SAMPLES_ARB,
        };

        int values[2];

        wglGetPixelFormatAttribivARB(dc, formats[i], 0, 2, attribs, values);

        if (!values[0]) {
            continue;
        }

        if (values[1] > best_samples) {
            best_format = i;
            best_samples = values[1];
        }
    }

    return formats[(best_format < 0) ? 0 : best_format];
}

tq_key key_conv(WPARAM wp, LPARAM lp)
{
    UINT vk = (UINT) wp;
    UINT scancode = (lp & (0xff << 16)) >> 16;
    BOOL extended = (lp & (0x01 << 24));

    if (vk >= 'A' && vk <= 'Z') {
        return TQ_KEY_A + (vk - 'A');
    }

    if (vk >= '0' && vk <= '9') {
        return TQ_KEY_0 + (vk - '0');
    }

    if (vk >= VK_F1 && vk <= VK_F12) {
        return TQ_KEY_F1 + (vk - VK_F1);
    }

    if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
        return TQ_KEY_KP_0 + (vk - VK_NUMPAD0);
    }

    if (vk == VK_RETURN) {
        if (extended) {
            return TQ_KEY_KP_ENTER;
        }
        return TQ_KEY_ENTER;
    }

    if (vk == VK_SHIFT) {
        UINT nVk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
        if (nVk == VK_LSHIFT) {
            return TQ_KEY_LSHIFT;
        } else if (nVk == VK_RSHIFT) {
            return TQ_KEY_RSHIFT;
        }
    }

    if (vk == VK_CONTROL) {
        if (extended) {
            return TQ_KEY_RCTRL;
        }
        return TQ_KEY_LCTRL;
    }

    if (vk == VK_MENU) {
        if (extended) {
            return TQ_KEY_RALT;
        }
        return TQ_KEY_LALT;
    }

    switch (vk) {
    case VK_BACK:               return TQ_KEY_BACKSPACE;
    case VK_TAB:                return TQ_KEY_TAB;
    case VK_PAUSE:              return TQ_KEY_PAUSE;
    case VK_CAPITAL:            return TQ_KEY_CAPSLOCK;
    case VK_ESCAPE:             return TQ_KEY_ESCAPE;
    case VK_SPACE:              return TQ_KEY_SPACE;
    case VK_PRIOR:              return TQ_KEY_PGUP;
    case VK_NEXT:               return TQ_KEY_PGDN;
    case VK_END:                return TQ_KEY_END;
    case VK_HOME:               return TQ_KEY_HOME;
    case VK_LEFT:               return TQ_KEY_LEFT;
    case VK_UP:                 return TQ_KEY_UP;
    case VK_RIGHT:              return TQ_KEY_RIGHT;
    case VK_DOWN:               return TQ_KEY_DOWN;
    case VK_PRINT:              return TQ_KEY_PRINTSCREEN;
    case VK_INSERT:             return TQ_KEY_INSERT;
    case VK_DELETE:             return TQ_KEY_DELETE;
    case VK_LWIN:               return TQ_KEY_LSUPER;
    case VK_RWIN:               return TQ_KEY_RSUPER;
    case VK_MULTIPLY:           return TQ_KEY_KP_MUL;
    case VK_ADD:                return TQ_KEY_KP_ADD;
    case VK_SUBTRACT:           return TQ_KEY_KP_SUB;
    case VK_DECIMAL:            return TQ_KEY_KP_POINT;
    case VK_DIVIDE:             return TQ_KEY_KP_DIV;
    case VK_NUMLOCK:            return TQ_KEY_NUMLOCK;
    case VK_SCROLL:             return TQ_KEY_SCROLLLOCK;
    case VK_LMENU:              return TQ_KEY_MENU;
    case VK_RMENU:              return TQ_KEY_MENU;
    case VK_OEM_1:              return TQ_KEY_SEMICOLON;
    case VK_OEM_PLUS:           return TQ_KEY_EQUAL;
    case VK_OEM_COMMA:          return TQ_KEY_COMMA;
    case VK_OEM_MINUS:          return TQ_KEY_MINUS;
    case VK_OEM_PERIOD:         return TQ_KEY_PERIOD;
    case VK_OEM_2:              return TQ_KEY_SLASH;
    case VK_OEM_3:              return TQ_KEY_GRAVE;
    case VK_OEM_4:              return TQ_KEY_LBRACKET;
    case VK_OEM_6:              return TQ_KEY_RBRACKET;
    case VK_OEM_5:              return TQ_KEY_BACKSLASH;
    case VK_OEM_7:              return TQ_KEY_APOSTROPHE;
    }

    return TQ_TOTAL_KEYS;
}

tq_mouse_button mb_conv(UINT msg, WPARAM wp)
{
    switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        return TQ_MOUSE_BUTTON_LEFT;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        return TQ_MOUSE_BUTTON_RIGHT;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        return TQ_MOUSE_BUTTON_MIDDLE;
    }

    if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP) {
        WORD button = GET_XBUTTON_WPARAM(wp);

        if (button == 0x0001) {
            return TQ_MOUSE_BUTTON_X4;
        } else if (button == 0x0002) {
            return TQ_MOUSE_BUTTON_X5;
        }
    }

    return TQ_TOTAL_MOUSE_BUTTONS;
}

//------------------------------------------------------------------------------

#endif // defined(_WIN32)

//------------------------------------------------------------------------------
