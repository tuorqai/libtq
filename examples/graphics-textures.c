
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [graphics/textures]
// This example shows how to use textures.
//------------------------------------------------------------------------------

static tq_vec2f mushie_position;
static float mushie_direction;
static tq_texture field;
static tq_texture moon;

//------------------------------------------------------------------------------

static void loop(void)
{
    // Move mushroom back and forth.

    // 64 pixels per second
    mushie_position.x += mushie_direction * 64.0f * (float) tq_get_delta_time();

    if (mushie_position.x < 0.0f) {
        mushie_position.x = 0.0f;
        mushie_direction = 1.0f;
    } else if (mushie_position.x > 320.0f) {
        mushie_position.x = 320.0f;
        mushie_direction = -1.0f;
    }

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
        tq_rectf dst = {mushie_position.x, mushie_position.y, 32, 32};

        tq_draw_subtexture(field, sub, dst);
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] graphics/textures");

    tq_initialize();

    // Night sky color.
    tq_set_clear_color((tq_color) {19, 41, 66, 255});

    // Sometimes you want to load opaque texture as transparent one,
    // pretending like one of its colors is fully transparent.
    // This function can help you to do that.
    tq_set_color_key((tq_color) {38, 53, 68, 255});

    // Load two texture files.
    field = tq_load_texture_from_file("assets/textures/field.png");
    moon = tq_load_texture_from_file("assets/textures/moon.png");

    // Check if the textures are successfully loaded.
    if (field.id == -1 || moon.id == -1) {
        return -1;
    }

    // In order to add more life to this, let's move some object.
    // The mushroom in the bottom of the screen will be moved
    // in the way that mushrooms in Mario games move.

    mushie_position.x = 32;
    mushie_position.y = 416;
    mushie_direction = -1.0f; // positive value is right, negative is left

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

    // Enter main loop.
    tq_run(loop);

    return 0;
}

//------------------------------------------------------------------------------
