
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [graphics/text]
// This example shows how to use textures.
//------------------------------------------------------------------------------

static tq_font regular;
static tq_font italic;

//------------------------------------------------------------------------------

static void loop(void)
{
    float t = tq_get_time_mediump();

    tq_color bg = {
        .r = 80 + (int) (sinf(t + 0.0f) * 16.0f) + 16,
        .g = 80 + (int) (sinf(t + 3.0f) * 16.0f) + 16,
        .b = 80 + (int) (sinf(t + 6.0f) * 16.0f) + 16,
        .a = 255,
    };

    tq_set_clear_color(bg);
    tq_clear();

    // Use tq_draw_text() to print static text.
    tq_set_draw_color((tq_color) {255, 255, 255, 255});
    tq_draw_text(regular, (tq_vec2f) {40, 40}, "Hello, world!");

    // Use tq_print_text() for printf-style formatted output.
    tq_set_draw_color((tq_color) {255, 255, 0, 255});
    tq_print_text(italic, (tq_vec2f) {40, 100}, "Mouse position: [%d, %d]",
        tq_get_mouse_cursor_position().x,
        tq_get_mouse_cursor_position().y);
    
    tq_print_text(italic, (tq_vec2f) {40, 130}, "Background: 0x%08x",
        (bg.r << 24) | (bg.g << 16) | (bg.b << 8) | 255);
    
    // Special symbols
    tq_print_text(italic, (tq_vec2f) { 40, 160 }, "Line 1\nLine 2");
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] graphics/text");

    tq_initialize();

    // Load fonts.
    regular = tq_load_font_from_file("assets/fonts/sansation.ttf", 36, 400);
    italic = tq_load_font_from_file("assets/fonts/sansation-italic.ttf", 22, 400);

    // Check if the fonts are successfully loaded.
    if (regular.id == -1 || italic.id == -1) {
        return -1;
    }

    tq_run(loop);

    return 0;
}

//------------------------------------------------------------------------------
