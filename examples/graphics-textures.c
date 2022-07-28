
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [graphics/textures]
// This example shows how to use textures.
//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] graphics/textures");

    tq_initialize();
    atexit(tq_terminate);

    tq_set_clear_color((tq_color) {0, 0, 0, 255});

    // Load two texture files.
    tq_texture field = tq_load_texture_from_file("assets/textures/field.png");
    tq_texture moon = tq_load_texture_from_file("assets/textures/moon.png");

    // Check if the textures are successfully loaded.
    if (field.id == -1 || moon.id == -1) {
        return -1;
    }

    // In tq library, all resources are wrapped in structures which contain
    // only one integer number member `id`, such as `tq_texture`, `tq_sound`
    // and others.

    // This is done to enforce stricter type checking, so that you don't
    // accidentally write something like this:
    //      tq_sound sound = tq_load_sound_from_file("...");
    //      tq_play_music(sound, 0);

    // Simple integer values were used as resource handles in very early
    // versions of tq library (and its predecessor dclib), but it was proven
    // quite error-prone.

    while (tq_process()) {
        tq_clear();

        // Draw first texture to the whole screen.
        // Note that if you change the screen size (see above)
        // to something other 512x512, you should care about this values too.
        tq_draw_texture(field, (tq_rectf) {0, 0, 512, 512});

        // Draw second texture inside a smaller rectangle on the top-left
        // side of the screen.
        tq_draw_texture(moon, (tq_rectf) {40, 40, 64, 64});

        // If you want to draw some part of the texture, you can
        // use tq_draw_subtexture().
        {
            tq_rectf sub = {48, 144, 16, 16};
            tq_rectf dst = {32, 416, 32, 32};

            tq_draw_subtexture(field, sub, dst);
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
