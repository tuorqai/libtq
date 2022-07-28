
//------------------------------------------------------------------------------

#include <stdio.h>
#include <tq/tq.h>

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    tq_set_display_size(TQ_VEC2I(512, 512));
    tq_set_title("[tq library] text.c");

    tq_initialize();
    atexit(tq_terminate);

    tq_font regular = tq_load_font_from_file("assets/fonts/sansation.ttf",
        36.0f, TQ_FONT_NORMAL);

    tq_font italic = tq_load_font_from_file("assets/fonts/sansation-italic.ttf",
        24.0f, TQ_FONT_NORMAL);

    if (regular.id == -1 || italic.id == -1) {
        return EXIT_FAILURE;
    }

    tq_set_clear_color(tq_c24(32, 32, 32));

    while (tq_process()) {
        tq_clear();

        tq_set_fill_color(tq_c24(255, 255, 255));
        tq_draw_text(regular, (tq_vec2f) {32, 36}, "Op brute wĳze ving de");
        tq_draw_text(regular, (tq_vec2f) {32, 72}, "schooljuf de quasi-kalme");
        tq_draw_text(regular, (tq_vec2f) {32, 108}, "lynx");

        tq_vec2i cursor = tq_get_mouse_cursor_position();

        tq_set_fill_color(tq_c24(0, 160, 210));
        tq_print_text(italic, TQ_VEC2F(32, 410),
            "cursor pos: [%d, %d]",
            cursor.x, cursor.y);

        tq_set_fill_color(tq_c24(224, 192, 224));
        tq_print_text(italic, TQ_VEC2F(32, 450),
            "time: %.2f", tq_get_time_mediump());
    }

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
