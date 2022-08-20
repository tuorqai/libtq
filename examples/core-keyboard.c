
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [core/keyboard]
// This example shows how to use keyboard-related functions.
//------------------------------------------------------------------------------

bool alphabet[26]; // array of pressed letter keys

void on_key_pressed(tq_key key)
{
    if (key >= TQ_KEY_A && key <= TQ_KEY_Z) {
        alphabet[key - TQ_KEY_A] = true;
    }
}

void on_key_released(tq_key key)
{
    if (key >= TQ_KEY_A && key <= TQ_KEY_Z) {
        alphabet[key - TQ_KEY_A] = false;
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] core/keyboard");

    tq_initialize();
    atexit(tq_terminate);

    // Set key press callback.
    tq_on_key_pressed(on_key_pressed);
    tq_on_key_released(on_key_released);

    // Set dark gray background.
    tq_set_clear_color((tq_color) {24, 24, 24, 255});

    tq_font unispace = tq_load_font_from_file("assets/fonts/unispace.ttf", 36.0f, 400);

    while (tq_process()) {
        // Clear the screen before drawing.
        tq_clear();

        // Draw alphabet.
        tq_set_fill_color((tq_color) {255, 255, 255, 255});
        tq_draw_text(unispace, (tq_vec2f) {60, 120}, "Alphabet:");

        tq_vec2f char_pos = { 60, 200 };

        for (int i = 0; i < 26; i++) {
            if (alphabet[i]) {
                tq_set_fill_color((tq_color) {0, 255, 0, 255});
            } else {
                tq_set_fill_color((tq_color) {255, 255, 255, 255});
            }

            tq_print_text(unispace, char_pos, "%c", 'A' + i);

            char_pos.x += 40.0f;

            if ((i + 1) % 10 == 0) {
                char_pos.x = 60.0f;
                char_pos.y += 50.0f;
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
