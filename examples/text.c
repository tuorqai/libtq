
//------------------------------------------------------------------------------

#include <stdio.h>
#include <tq/tq.h>

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    tq_set_display_size(512, 512);
    tq_set_title("[tq library] text.c");

    tq_initialize();
    atexit(tq_terminate);

    tq_font font = tq_load_font_from_file("assets/fonts/sansation.ttf",
        72.0f, TQ_FONT_NORMAL);

    if (font.id == -1) {
        fprintf(stderr, "Failed to load TTF font.\n");
        return EXIT_FAILURE;
    }

    tq_set_clear_color(TQ_COLOR24(32, 32, 32));

    while (tq_process()) {
        tq_clear();

        tq_draw_text(font, TQ_VEC2F(32, 32), "Hello world!");
    }

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
