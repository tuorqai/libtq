
//------------------------------------------------------------------------------

#include <stdio.h>
#include <tq/tq.h>

//------------------------------------------------------------------------------

void loop(void)
{
    float s = sin(tq_get_time_mediump());
    float t = cos(tq_get_time_mediump());

    tq_rectf rect = {
        .x = 360 - 16 + s * 200,
        .y = 240 - 16 - t * 200,
        .w = 32,
        .h = 32,
    };

    tq_set_clear_color((tq_color) { 32, 64, 128, 255 });
    tq_clear();

    tq_set_draw_color((tq_color) { 255, 255, 0, 255 });
    tq_fill_rectangle(rect);
}

void key_(tq_key key)
{
    printf("Key pressed: %d\n", key);
}

int main(int argc, char *argv[])
{
    printf("Something.\n");

    tq_set_display_size((tq_vec2i) { 720, 480 });

    tq_initialize();
    tq_on_key_pressed(key_);
    tq_run(loop);
    tq_terminate();

    return 0;
}

//------------------------------------------------------------------------------
