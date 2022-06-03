//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)

//------------------------------------------------------------------------------

#include <string.h>

#include <SFML/Graphics.h>

#include "tq_core.h"
#include "tq_display.h"
#include "tq_handle_list.h"
#include "tq_renderer.h"

//------------------------------------------------------------------------------

static sfRenderWindow *window;

static sfColor clear_color;
static sfColor point_color;
static sfColor line_color;
static sfColor outline_color;
static sfColor fill_color;

static handle_list_t *texture_list;

static sfView *view;

static sfVertexArray *point_vertex_array;
static sfVertexArray *line_vertex_array;
static sfVertexArray *outline_vertex_array;
static sfVertexArray *fill_vertex_array;
static sfVertexArray *texture_vertex_array;

static sfRenderStates primitive_renderstates;
static sfRenderStates texture_renderstates;

//------------------------------------------------------------------------------
// Utility functions

static void center_window(void)
{
    sfVector2u window_size = sfRenderWindow_getSize(window);
    sfVideoMode desktop_mode = sfVideoMode_getDesktopMode();

    sfRenderWindow_setPosition(window, (sfVector2i) {
        .x = (desktop_mode.width / 2) - (window_size.x / 2),
        .y = (desktop_mode.height / 2) - (window_size.y / 2),
    });
}

static tq_key_t key_conv(sfKeyCode key)
{
    switch (key) {
        case sfKeyNum0:         return TQ_KEY_0;
        case sfKeyNum1:         return TQ_KEY_1;
        case sfKeyNum2:         return TQ_KEY_2;
        case sfKeyNum3:         return TQ_KEY_3;
        case sfKeyNum4:         return TQ_KEY_4;
        case sfKeyNum5:         return TQ_KEY_5;
        case sfKeyNum6:         return TQ_KEY_6;
        case sfKeyNum7:         return TQ_KEY_7;
        case sfKeyNum8:         return TQ_KEY_8;
        case sfKeyNum9:         return TQ_KEY_9;
        case sfKeyA:            return TQ_KEY_A;
        case sfKeyB:            return TQ_KEY_B;
        case sfKeyC:            return TQ_KEY_C;
        case sfKeyD:            return TQ_KEY_D;
        case sfKeyE:            return TQ_KEY_E;
        case sfKeyF:            return TQ_KEY_F;
        case sfKeyG:            return TQ_KEY_G;
        case sfKeyH:            return TQ_KEY_H;
        case sfKeyI:            return TQ_KEY_I;
        case sfKeyJ:            return TQ_KEY_J;
        case sfKeyK:            return TQ_KEY_K;
        case sfKeyL:            return TQ_KEY_L;
        case sfKeyM:            return TQ_KEY_M;
        case sfKeyN:            return TQ_KEY_N;
        case sfKeyO:            return TQ_KEY_O;
        case sfKeyP:            return TQ_KEY_P;
        case sfKeyQ:            return TQ_KEY_Q;
        case sfKeyR:            return TQ_KEY_R;
        case sfKeyS:            return TQ_KEY_S;
        case sfKeyT:            return TQ_KEY_T;
        case sfKeyU:            return TQ_KEY_U;
        case sfKeyV:            return TQ_KEY_V;
        case sfKeyW:            return TQ_KEY_W;
        case sfKeyX:            return TQ_KEY_X;
        case sfKeyY:            return TQ_KEY_Y;
        case sfKeyZ:            return TQ_KEY_Z;
        case sfKeyTilde:        return TQ_KEY_GRAVE;
        case sfKeyQuote:        return TQ_KEY_APOSTROPHE;
        case sfKeyHyphen:       return TQ_KEY_MINUS;
        case sfKeyEqual:        return TQ_KEY_EQUAL;
        case sfKeyLBracket:     return TQ_KEY_LBRACKET;
        case sfKeyRBracket:     return TQ_KEY_RBRACKET;
        case sfKeyComma:        return TQ_KEY_COMMA;
        case sfKeyPeriod:       return TQ_KEY_PERIOD;
        case sfKeySemicolon:    return TQ_KEY_SEMICOLON;
        case sfKeySlash:        return TQ_KEY_SLASH;
        case sfKeyBackslash:    return TQ_KEY_BACKSLASH;
        case sfKeySpace:        return TQ_KEY_SPACE;
        case sfKeyEscape:       return TQ_KEY_ESCAPE;
        case sfKeyBackspace:    return TQ_KEY_BACKSPACE;
        case sfKeyTab:          return TQ_KEY_TAB;
        case sfKeyEnter:        return TQ_KEY_ENTER;
        case sfKeyF1:           return TQ_KEY_F1;
        case sfKeyF2:           return TQ_KEY_F2;
        case sfKeyF3:           return TQ_KEY_F3;
        case sfKeyF4:           return TQ_KEY_F4;
        case sfKeyF5:           return TQ_KEY_F5;
        case sfKeyF6:           return TQ_KEY_F6;
        case sfKeyF7:           return TQ_KEY_F7;
        case sfKeyF8:           return TQ_KEY_F8;
        case sfKeyF9:           return TQ_KEY_F9;
        case sfKeyF10:          return TQ_KEY_F10;
        case sfKeyF11:          return TQ_KEY_F11;
        case sfKeyF12:          return TQ_KEY_F12;
        case sfKeyUp:           return TQ_KEY_UP;
        case sfKeyDown:         return TQ_KEY_DOWN;
        case sfKeyLeft:         return TQ_KEY_LEFT;
        case sfKeyRight:        return TQ_KEY_RIGHT;
        case sfKeyLShift:       return TQ_KEY_LSHIFT;
        case sfKeyRShift:       return TQ_KEY_RSHIFT;
        case sfKeyLControl:     return TQ_KEY_LCTRL;
        case sfKeyRControl:     return TQ_KEY_RCTRL;
        case sfKeyLAlt:         return TQ_KEY_LALT;
        case sfKeyRAlt:         return TQ_KEY_RALT;
        case sfKeyLSystem:      return TQ_KEY_LSUPER;
        case sfKeyRSystem:      return TQ_KEY_RSUPER;
        case sfKeyMenu:         return TQ_KEY_MENU;
        case sfKeyPageUp:       return TQ_KEY_PGUP;
        case sfKeyPageDown:     return TQ_KEY_PGDN;
        case sfKeyHome:         return TQ_KEY_HOME;
        case sfKeyEnd:          return TQ_KEY_END;
        case sfKeyInsert:       return TQ_KEY_INSERT;
        case sfKeyDelete:       return TQ_KEY_DELETE;
        case sfKeyPause:        return TQ_KEY_PAUSE;
        case sfKeyNumpad0:      return TQ_KEY_KP_0;
        case sfKeyNumpad1:      return TQ_KEY_KP_1;
        case sfKeyNumpad2:      return TQ_KEY_KP_2;
        case sfKeyNumpad3:      return TQ_KEY_KP_3;
        case sfKeyNumpad4:      return TQ_KEY_KP_4;
        case sfKeyNumpad5:      return TQ_KEY_KP_5;
        case sfKeyNumpad6:      return TQ_KEY_KP_6;
        case sfKeyNumpad7:      return TQ_KEY_KP_7;
        case sfKeyNumpad8:      return TQ_KEY_KP_8;
        case sfKeyNumpad9:      return TQ_KEY_KP_9;
        case sfKeyMultiply:     return TQ_KEY_KP_MUL;
        case sfKeyAdd:          return TQ_KEY_KP_ADD;
        case sfKeySubtract:     return TQ_KEY_KP_SUB;
        case sfKeyDivide:       return TQ_KEY_KP_DIV;
        default:                return TQ_TOTAL_KEYS;
    }
}

static tq_mouse_button_t mouse_button_conv(sfMouseButton mouse_button)
{
    switch (mouse_button) {
        case sfMouseLeft:       return TQ_MOUSE_BUTTON_LEFT;
        case sfMouseRight:      return TQ_MOUSE_BUTTON_RIGHT;
        case sfMouseMiddle:     return TQ_MOUSE_BUTTON_MIDDLE;
        default:                return TQ_TOTAL_MOUSE_BUTTONS;
    }
}

static void on_key_pressed(sfKeyEvent const *event)
{
    tq_key_t key = key_conv(event->code);

    if (key != TQ_TOTAL_KEYS) {
        core_on_key_pressed(key);
    }
}

static void on_key_released(sfKeyEvent const *event)
{
    tq_key_t key = key_conv(event->code);

    if (key != TQ_TOTAL_KEYS) {
        core_on_key_released(key);
    }
}

static void on_mouse_button_pressed(sfMouseButtonEvent const *event)
{
    tq_mouse_button_t mouse_button = mouse_button_conv(event->button);

    if (mouse_button != TQ_TOTAL_MOUSE_BUTTONS) {
        core_on_mouse_button_pressed(mouse_button);
    }
}

static void on_mouse_button_released(sfMouseButtonEvent const *event)
{
    tq_mouse_button_t mouse_button = mouse_button_conv(event->button);

    if (mouse_button != TQ_TOTAL_MOUSE_BUTTONS) {
        core_on_mouse_button_released(mouse_button);
    }
}

static void texture_dtor(void *item)
{
    sfTexture *texture = *(sfTexture **) item;
    sfTexture_destroy(texture);
}

//------------------------------------------------------------------------------
// SFML-based display implementation

static void sf2_display_initialize(uint32_t width, uint32_t height, char const *title)
{
    window = sfRenderWindow_create(
        (sfVideoMode) {
            .width = width,
            .height = height,
            .bitsPerPixel = 32,
        },
        title,
        sfTitlebar | sfClose,
        &(sfContextSettings) {
            .antialiasingLevel = 4,
            .majorVersion = 2,
            .minorVersion = 1,
        }
    );

    sfRenderWindow_setKeyRepeatEnabled(window, sfFalse);
    sfRenderWindow_setVerticalSyncEnabled(window, sfTrue);

    center_window();
}

static void sf2_display_terminate(void)
{
    sfRenderWindow_destroy(window);
}

static void sf2_display_present(void)
{
    sfRenderWindow_display(window);
}

static bool sf2_display_process_events(void)
{
    sfEvent event;

    while (sfRenderWindow_pollEvent(window, &event)) {
        switch (event.type) {
        case sfEvtKeyPressed:
            on_key_pressed(&event.key);
            break;
        case sfEvtKeyReleased:
            on_key_released(&event.key);
            break;
        case sfEvtMouseButtonPressed:
            on_mouse_button_pressed(&event.mouseButton);
            break;
        case sfEvtMouseButtonReleased:
            on_mouse_button_released(&event.mouseButton);
            break;
        case sfEvtMouseMoved:
            core_on_mouse_cursor_moved(event.mouseMove.x, event.mouseMove.y);
            break;
        case sfEvtClosed:
            sfRenderWindow_close(window);
            break;
        default:
            break;
        }
    }

    return sfRenderWindow_isOpen(window);
}

static void sf2_display_set_size(uint32_t width, uint32_t height)
{
    sfRenderWindow_setSize(window, (sfVector2u) { width, height });
}

static void sf2_display_set_title(char const *title)
{
    sfRenderWindow_setTitle(window, title);
}

//------------------------------------------------------------------------------
// SFML-based renderer implementation

static void sf2_renderer_initialize(void)
{
    texture_list = handle_list_create(sizeof(sfTexture *), texture_dtor);
    view = sfView_create();

    point_vertex_array = sfVertexArray_create();
    line_vertex_array = sfVertexArray_create();
    outline_vertex_array = sfVertexArray_create();
    fill_vertex_array = sfVertexArray_create();
    texture_vertex_array = sfVertexArray_create();

    sfVertexArray_setPrimitiveType(point_vertex_array, sfPoints);
    sfVertexArray_setPrimitiveType(line_vertex_array, sfLines);
    sfVertexArray_setPrimitiveType(outline_vertex_array, sfLineStrip);
    sfVertexArray_setPrimitiveType(fill_vertex_array, sfTriangleFan);
    sfVertexArray_setPrimitiveType(texture_vertex_array, sfTriangleFan);

    primitive_renderstates.blendMode = sfBlendAlpha;
    texture_renderstates.blendMode = sfBlendAlpha;
}

static void sf2_renderer_terminate(void)
{
    sfVertexArray_destroy(point_vertex_array);
    sfVertexArray_destroy(line_vertex_array);
    sfVertexArray_destroy(outline_vertex_array);
    sfVertexArray_destroy(fill_vertex_array);
    sfVertexArray_destroy(texture_vertex_array);

    sfView_destroy(view);
    handle_list_destroy(texture_list);
}

static void sf2_renderer_clear(void)
{
    sfRenderWindow_clear(window, clear_color);
}

static void sf2_renderer_set_clear_color(tq_color_t new_clear_color)
{
    clear_color = sfColor_fromInteger(new_clear_color);
}

static void sf2_renderer_set_view(float x, float y, float width, float height, float angle)
{
    view = sfView_createFromRect((sfFloatRect) { x, y, width, height });
    sfRenderWindow_setView(window, view);
}

static void sf2_renderer_reset_view(void)
{
    sfRenderWindow_setView(window, sfRenderWindow_getDefaultView(window));
}

static void sf2_renderer_transform(float const *matrix)
{
    memcpy(primitive_renderstates.transform.matrix, matrix, sizeof(float) * 9);
    memcpy(texture_renderstates.transform.matrix, matrix, sizeof(float) * 9);
}

static void sf2_renderer_draw_points(float const *data, unsigned int length)
{
    for (int i = 0; i < length; i++) {
        sfVertexArray_append(point_vertex_array, (sfVertex) {
            .position = { data[2 * i + 0], data[2 * i + 1] },
            .color = point_color,
        });
    }

    sfRenderWindow_drawVertexArray(window, point_vertex_array, &primitive_renderstates);
    sfVertexArray_clear(point_vertex_array);
}

static void sf2_renderer_draw_lines(float const *data, unsigned int length)
{
    for (int i = 0; i < length; i++) {
        sfVertexArray_append(line_vertex_array, (sfVertex) {
            .position = { data[2 * i + 0], data[2 * i + 1] },
            .color = line_color,
        });
    }

    sfRenderWindow_drawVertexArray(window, line_vertex_array, &primitive_renderstates);
    sfVertexArray_clear(line_vertex_array);
}

static void sf2_renderer_draw_outline(float const *data, unsigned int length)
{
    for (int i = 0; i < length; i++) {
        sfVertexArray_append(outline_vertex_array, (sfVertex) {
            .position = { data[2 * i + 0], data[2 * i + 1] },
            .color = outline_color,
        });
    }

    sfVertexArray_append(outline_vertex_array, *sfVertexArray_getVertex(outline_vertex_array, 0));

    sfRenderWindow_drawVertexArray(window, outline_vertex_array, &primitive_renderstates);
    sfVertexArray_clear(outline_vertex_array);
}

static void sf2_renderer_draw_fill(float const *data, unsigned int length)
{
    for (int i = 0; i < length; i++) {
        sfVertexArray_append(fill_vertex_array, (sfVertex) {
            .position = { data[2 * i + 0], data[2 * i + 1] },
            .color = fill_color,
        });
    }

    sfRenderWindow_drawVertexArray(window, fill_vertex_array, &primitive_renderstates);
    sfVertexArray_clear(fill_vertex_array);
}

static void sf2_renderer_set_point_color(tq_color_t new_point_color)
{
    point_color = sfColor_fromInteger(new_point_color);
}

static void sf2_renderer_set_line_color(tq_color_t new_line_color)
{
    line_color = sfColor_fromInteger(new_line_color);
}

static void sf2_renderer_set_outline_color(tq_color_t new_outline_color)
{
    outline_color = sfColor_fromInteger(new_outline_color);
}

static void sf2_renderer_set_fill_color(tq_color_t new_fill_color)
{
    fill_color = sfColor_fromInteger(new_fill_color);
}

static tq_handle_t sf2_renderer_load_texture(uint8_t const *buffer, size_t length)
{
    sfTexture *texture = sfTexture_createFromMemory(buffer, length, NULL);

    if (!texture) {
        return TQ_INVALID_HANDLE;
    }

    return handle_list_append(texture_list, &texture);
}

static void sf2_renderer_delete_texture(tq_handle_t texture_handle)
{
    handle_list_erase(texture_list, texture_handle);
}

static void sf2_renderer_get_texture_size(tq_handle_t texture_handle, uint32_t *width, uint32_t *height)
{
    sfTexture **texture = (sfTexture **) handle_list_get(texture_list, texture_handle);

    if (!texture) {
        return;
    }

    *width = sfTexture_getSize(*texture).x;
    *height = sfTexture_getSize(*texture).y;
}

static void sf2_renderer_draw_texture(float const *data, unsigned int length, tq_handle_t texture_handle)
{
    sfTexture **texture = (sfTexture **) handle_list_get(texture_list, texture_handle);
    
    if (!texture) {
        return;
    }

    sfVertexArray_clear(texture_vertex_array);

    for (unsigned int i = 0; i < length; i++) {
        sfVertexArray_append(texture_vertex_array, (sfVertex) {
            .position = { data[4 * i + 0], data[4 * i + 1] },
            .texCoords = { data[4 * i + 2], data[4 * i + 3] },
            .color = sfWhite,
        });
    }

    texture_renderstates.texture = *texture;
    sfRenderWindow_drawVertexArray(window, texture_vertex_array, &texture_renderstates);
}

static void sf2_renderer_flush(void)
{
}

//------------------------------------------------------------------------------

void construct_sf2_display(struct display *display)
{
    *display = (struct display) {
        .initialize         = sf2_display_initialize,
        .terminate          = sf2_display_terminate,
        .present            = sf2_display_present,
        .process_events     = sf2_display_process_events,
        .set_size           = sf2_display_set_size,
        .set_title          = sf2_display_set_title,
    };
}

void construct_sf2_renderer(struct renderer *renderer)
{
    *renderer = (struct renderer) {
        .initialize         = sf2_renderer_initialize,
        .terminate          = sf2_renderer_terminate,
        .clear              = sf2_renderer_clear,
        .set_clear_color    = sf2_renderer_set_clear_color,
        .set_view           = sf2_renderer_set_view,
        .reset_view         = sf2_renderer_reset_view,
        .transform          = sf2_renderer_transform,
        .draw_points        = sf2_renderer_draw_points,
        .draw_lines         = sf2_renderer_draw_lines,
        .draw_outline       = sf2_renderer_draw_outline,
        .draw_fill          = sf2_renderer_draw_fill,
        .set_point_color    = sf2_renderer_set_point_color,
        .set_line_color     = sf2_renderer_set_line_color,
        .set_outline_color  = sf2_renderer_set_outline_color,
        .set_fill_color     = sf2_renderer_set_fill_color,
        .load_texture       = sf2_renderer_load_texture,
        .delete_texture     = sf2_renderer_delete_texture,
        .get_texture_size   = sf2_renderer_get_texture_size,
        .draw_texture       = sf2_renderer_draw_texture,
        .flush              = sf2_renderer_flush,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_USE_SFML)

//------------------------------------------------------------------------------
