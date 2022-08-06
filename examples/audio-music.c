
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [audio/music]
// In this example we demonstrate how tq library handles music.
//------------------------------------------------------------------------------

tq_channel music_channel = { -1 };

// Key press callback.
// Use this to handle key presses. Here we use it to pause the music if
// SPACE key is pressed.
void on_key_pressed(tq_key key)
{
    if (key == TQ_KEY_SPACE) {
        // Use this function to find out if the channel is playing or not.
        tq_channel_state state = tq_get_channel_state(music_channel);

        // Pause or unpause the channel according to its state.
        if (state == TQ_CHANNEL_PLAYING) {
            tq_pause_channel(music_channel);
        } else if (state == TQ_CHANNEL_PAUSED) {
            tq_unpause_channel(music_channel);
        }
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] audio/music");

    tq_initialize();
    atexit(tq_terminate);

    // Set key press callback.
    tq_on_key_pressed(on_key_pressed);

    tq_set_clear_color((tq_color) {0, 0, 0, 255});
    tq_set_outline_color((tq_color) {255, 255, 255, 255});

    tq_music music = tq_open_music_from_file("assets/music/ostrich.ogg");
    music_channel = tq_play_music(music, -1);

    // In the main loop, we will draw rotating rectangle.
    // It will rotate only if the music plays.

    float rotation = 0.0f;

    while (tq_process()) {
        // We also will scale the rectangle back and forth.
        float scale = 1.0f + (0.25f * sinf(2.0f * tq_get_time_mediump()));
    
        tq_channel_state state = tq_get_channel_state(music_channel);

        if (state == TQ_CHANNEL_PLAYING) {
            // 45 degrees per second
            rotation += 45.0f * (float) tq_get_delta_time();
        }

        tq_clear();

        tq_push_matrix();
            tq_translate_matrix((tq_vec2f) {256, 256});
            tq_scale_matrix((tq_vec2f) {scale, scale});
            tq_rotate_matrix(rotation);
            tq_outline_rectangle((tq_rectf) {-64, -64, 128, 128});
        tq_pop_matrix();
    }

    return 0;
}

//------------------------------------------------------------------------------
