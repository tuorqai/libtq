
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------

static int selected_index = -1;

static tq_texture   texture;
static tq_sound     sounds[4];
static tq_music     music;
static tq_channel   music_channel = { -1 };

//------------------------------------------------------------------------------

void draw_button(int index, float x, float y)
{
    tq_set_outline_color((index == selected_index)
        ? tq_c24(0xFF, 0xFF, 0xFF)
        : tq_c24(0x80, 0x80, 0x80));

    tq_push_matrix();
        tq_translate_matrix((tq_vec2f) {x, y});
        tq_draw_subtexture(texture,
            (tq_rectf) {index * 16.0f, 0.0f, 16.0f, 16.0f},
            (tq_rectf) {-16.0f, -16.0f, 32.0f, 32.0f});
        tq_outline_rectangle((tq_rectf) {-32.0f, -32.0f, 64.0f, 64.0f});
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
    case TQ_KEY_SPACE:
        if (music_channel.id == -1) {
            music_channel = tq_play_music(music, -1);
        } else {
            if (tq_get_channel_state(music_channel) == TQ_CHANNEL_PLAYING) {
                tq_pause_channel(music_channel);
            } else if (tq_get_channel_state(music_channel) == TQ_CHANNEL_PAUSED) {
                tq_unpause_channel(music_channel);
            }
        }
        break;
    }
}

void on_mouse_button_pressed(tq_vec2i cursor, tq_mouse_button_t button)
{
    if (button == TQ_MOUSE_BUTTON_LEFT) {
        if (selected_index != -1) {
            tq_play_sound(sounds[selected_index], 0);
        }
    }
}

void on_mouse_cursor_moved(tq_vec2i cursor)
{
    selected_index = -1;

    if (cursor.y >= 224 && cursor.y <= 288) {
        if (cursor.x >= 32 && cursor.x <= 96) {
            selected_index = 0;
        } else if (cursor.x >= 160 && cursor.x <= 224) {
            selected_index = 1;
        } else if (cursor.x >= 288 && cursor.x <= 352) {
            selected_index = 2;
        } else if (cursor.x >= 416 && cursor.x <= 480) {
            selected_index = 3;
        }
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] sounds.c");

    tq_initialize();
    atexit(tq_terminate);

    texture = tq_load_texture_from_file("assets/sounds.png");
    sounds[0] = tq_load_sound_from_file("assets/smb_coin.wav");
    sounds[1] = tq_load_sound_from_file("assets/smb_breakblock.wav");
    sounds[2] = tq_load_sound_from_file("assets/smb_powerup_appears.wav");
    sounds[3] = tq_load_sound_from_file("assets/smb_kick.wav");
    music = tq_open_music_from_file("assets/smas_smb_bonus.ogg");

    tq_on_key_pressed(on_key_pressed);
    tq_on_mouse_button_pressed(on_mouse_button_pressed);
    tq_on_mouse_cursor_moved(on_mouse_cursor_moved);

    tq_set_clear_color(tq_c24(0x00, 0x08, 0x2E));

    while (tq_process()) {
        tq_clear();

        draw_button(0, 64.0f, 256.0f);
        draw_button(1, 192.0f, 256.0f);
        draw_button(2, 320.0f, 256.0f);
        draw_button(3, 448.0f, 256.0f);
    }

    return 0;
}
