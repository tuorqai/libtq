
#include <stdio.h>
#include <tq/tq.h>

#define NUM_COLORS  7

static tq_color_t colors[NUM_COLORS] = {
    0xE0E0E0FF, 0xE00000FF, 0x00E000FF, 0x0000E0FF,
    0xE0E000FF, 0xE000E0FF, 0x00E0E0FF,
};

static float position_x = 256.0f;
static float position_y = 256.0f;
static float size = 128.0f;
static int current_color = 0;

void on_key_pressed(tq_key_t key)
{
    if (key == TQ_KEY_Z) {
        current_color--;
    } else if (key == TQ_KEY_X) {
        current_color++;
    } else {
        printf("on_key_pressed(): 0x%02X\n", key);
    }

    if (current_color == -1) {
        current_color = NUM_COLORS - 1;
    }

    if (current_color == NUM_COLORS) {
        current_color = 0;
    }
}

void on_mouse_button_pressed(tq_mouse_button_t mouse_button, int32_t x, int32_t y)
{
    if (mouse_button == TQ_MOUSE_BUTTON_LEFT) {
        position_x = (float) x;
        position_y = (float) y;
    } else if (mouse_button == TQ_MOUSE_BUTTON_RIGHT) {
        position_x = 256.0f;
        position_y = 256.0f;
        size = 128.0f;
    }
}

void on_mouse_wheel_scrolled(float delta, int32_t x, int32_t y)
{
    size += delta * 16.0f;

    if (size < 16.0f) {
        size = 16.0f;
    }

    if (size > 256.0f) {
        size = 256.0f;
    }

    printf("on_mouse_wheel_scrolled(): %f, %d, %d\n", delta, x, y);
}

int main(int argc, char *argv[])
{
    tq_set_display_size(512, 512);

    tq_initialize();

    tq_set_clear_color(TQ_COLOR24(20, 20, 64));
    tq_set_outline_color(TQ_COLOR24(224, 224, 224));

    tq_on_key_pressed(on_key_pressed);
    tq_on_mouse_button_pressed(on_mouse_button_pressed);
    tq_on_mouse_wheel_scrolled(on_mouse_wheel_scrolled);

    while (tq_process()) {
        float const t = tq_get_time_mediump();

        tq_clear();

        tq_set_outline_color(colors[current_color]);
        tq_translate_matrix_f(position_x, position_y);
        tq_rotate_matrix(t * 45.0f);
        tq_outline_rectangle_f(-size / 2.0f, -size / 2.0f, size, size);
    }

    tq_terminate();

    return EXIT_SUCCESS;
}
