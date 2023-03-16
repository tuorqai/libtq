
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------

static tq_texture field; 
static tq_texture glow;
static tq_surface mask;

static void loop(void)
{
    tq_vec2i cursor = tq_get_mouse_cursor_position();
    tq_rectf glow_rect = { cursor.x - 128, cursor.y - 128, 256, 256 };

    tq_set_blend_mode(TQ_BLEND_MODE_NONE);

    tq_set_surface(mask);
    tq_clear();
    tq_draw_texture(glow, glow_rect);

    tq_reset_surface();
    tq_clear();
    tq_draw_texture(field, (tq_rectf) { 0, 0, 512, 512 });

    tq_set_blend_mode(TQ_BLEND_MODE_MUL);
    tq_draw_texture(tq_get_surface_texture(mask),
        (tq_rectf) { 0, 0, 512, 512 });
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] graphics/surfaces");

    tq_initialize();

    field = tq_load_texture_from_file("assets/textures/field.png");
    glow = tq_load_texture_from_file("assets/textures/glow.png");
    mask = tq_create_surface((tq_vec2i) {512, 512});

    tq_run(loop);

    return 0;
}

//------------------------------------------------------------------------------
