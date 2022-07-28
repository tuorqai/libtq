
#include <stdio.h>
#include <tq/tq.h>

#define NUM_COLORS  7

static tq_color colors[NUM_COLORS] = {
    {0xE0, 0xE0, 0xE0, 0xFF},
    {0xE0, 0x00, 0x00, 0xFF},
    {0x00, 0xE0, 0x00, 0xFF},
    {0x00, 0x00, 0xE0, 0xFF},
    {0xE0, 0xE0, 0x00, 0xFF},
    {0xE0, 0x00, 0xE0, 0xFF},
    {0x00, 0xE0, 0xE0, 0xFF},
};

static tq_vec2f position = {256.0f, 256.0f};
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

void on_mouse_button_pressed(tq_vec2i cursor, tq_mouse_button_t mouse_button)
{
    if (mouse_button == TQ_MOUSE_BUTTON_LEFT) {
        position.x = (float) cursor.x;
        position.y = (float) cursor.y;
    } else if (mouse_button == TQ_MOUSE_BUTTON_RIGHT) {
        position.x = 256.0f;
        position.y = 256.0f;
        size = 128.0f;
    }
}

void on_mouse_wheel_scrolled(tq_vec2i cursor, tq_vec2f wheel)
{
    size += wheel.y * 16.0f;

    if (size < 16.0f) {
        size = 16.0f;
    }

    if (size > 256.0f) {
        size = 256.0f;
    }

    printf("on_mouse_wheel_scrolled(): %f, %d, %d\n", wheel.y, cursor.x, cursor.y);
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});

    tq_initialize();

    tq_set_clear_color(tq_c24(20, 20, 64));
    tq_set_outline_color(tq_c24(224, 224, 224));

    tq_on_key_pressed(on_key_pressed);
    tq_on_mouse_button_pressed(on_mouse_button_pressed);
    tq_on_mouse_wheel_scrolled(on_mouse_wheel_scrolled);

    while (tq_process()) {
        float const t = tq_get_time_mediump();

        tq_clear();

        tq_set_outline_color(colors[current_color]);
        tq_translate_matrix(position);
        tq_rotate_matrix(t * 45.0f);

        tq_outline_rectangle((tq_rectf) {
            .x = -size / 2.0f, .y = -size / 2.0f,
            .w = size, .h = size,
        });
    }

    tq_terminate();

    return EXIT_SUCCESS;
}
