
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------

static int selected_index = -1;

static int32_t texture;
static int32_t sounds[4];

//------------------------------------------------------------------------------

void draw_button(int index, float x, float y)
{
    tq_set_outline_color((index == selected_index) ? 0xFFFFFFFF : 0x808080FF);

    tq_push_matrix();
        tq_translate_matrix_f(x, y);
        tq_draw_texture_fragment_f(texture,
            -16.0f, -16.0f, 32.0f, 32.0f,
            index * 16.0f, 0.0f, 16.0f, 16.0f);
        tq_outline_rectangle_f(-32.0f, -32.0f, 64.0f, 64.0f);
    tq_pop_matrix();
}

void on_key_pressed(tq_key_t key)
{
    switch (key) {
    case TQ_KEY_1:
        tq_play_sound(sounds[0], 0);
        break;
    case TQ_KEY_2:
        tq_play_sound(sounds[1], 0);
        break;
    case TQ_KEY_3:
        tq_play_sound(sounds[2], 0);
        break;
    case TQ_KEY_4:
        tq_play_sound(sounds[3], 0);
        break;
    }
}

void on_mouse_button_pressed(tq_mouse_button_t button, int32_t x, int32_t y)
{
    if (button == TQ_MOUSE_BUTTON_LEFT) {
        if (selected_index != -1) {
            tq_play_sound(sounds[selected_index], 0);
        }
    }
}

void on_mouse_cursor_moved(int32_t x, int32_t y)
{
    selected_index = -1;

    if (y >= 224 && y <= 288) {
        if (x >= 32 && x <= 96) {
            selected_index = 0;
        } else if (x >= 160 && x <= 224) {
            selected_index = 1;
        } else if (x >= 288 && x <= 352) {
            selected_index = 2;
        } else if (x >= 416 && x <= 480) {
            selected_index = 3;
        }
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size(512, 512);
    tq_set_title("[tq library] sounds.c");

    tq_initialize();
    atexit(tq_terminate);

    texture = tq_load_texture_from_file("assets/sounds.png");
    sounds[0] = tq_load_sound_from_file("assets/smb_coin.wav");
    sounds[1] = tq_load_sound_from_file("assets/smb_breakblock.wav");
    sounds[2] = tq_load_sound_from_file("assets/smb_powerup_appears.wav");
    sounds[3] = tq_load_sound_from_file("assets/smb_kick.wav");

    tq_on_key_pressed(on_key_pressed);
    tq_on_mouse_button_pressed(on_mouse_button_pressed);
    tq_on_mouse_cursor_moved(on_mouse_cursor_moved);

    tq_set_clear_color(0x00082eff);

    while (tq_process()) {
        tq_clear();

        draw_button(0, 64.0f, 256.0f);
        draw_button(1, 192.0f, 256.0f);
        draw_button(2, 320.0f, 256.0f);
        draw_button(3, 448.0f, 256.0f);
    }

    return 0;
}
