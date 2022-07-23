
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------

static void draw_spinning_square(float rotation, tq_vec2i surface_size)
{
    tq_vec2f center = tq_vec2f_scale(tq_vec2i_cast(surface_size), 0.5f);
    tq_rectf square = { -center.x / 2.0f, -center.y / 2.0f, center.x, center.y };

    tq_push_matrix();
    tq_translate_matrix(center);
    tq_rotate_matrix(rotation);
    tq_outline_rectangle(square);
    tq_pop_matrix();
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] graphics/surfaces");

    tq_initialize();
    atexit(tq_terminate);

    tq_vec2i surf0_size = { 16, 16 };
    tq_vec2i surf1_size = { 32, 32 };
    tq_vec2i surf2_size = { 64, 64 };

    tq_surface surf0 = tq_create_surface(surf0_size);
    tq_surface surf1 = tq_create_surface(surf1_size);
    tq_surface surf2 = tq_create_surface(surf2_size);

    tq_set_texture_smooth(tq_get_surface_texture(surf0), false);
    tq_set_texture_smooth(tq_get_surface_texture(surf1), false);
    tq_set_texture_smooth(tq_get_surface_texture(surf2), false);

    float rotation0 = 0.0f;
    float rotation1 = 30.0f;
    float rotation2 = 60.0f;

    while (tq_process()) {
        float dt = (float) tq_get_delta_time();

        rotation0 += 90.0f * dt;
        rotation1 += 90.0f * dt;
        rotation2 += 90.0f * dt;

        tq_set_surface(surf0);
        tq_set_clear_color(tq_c24(29, 43, 83));
        tq_set_outline_color(tq_c24(194, 195, 199));
        tq_clear();
        draw_spinning_square(rotation0, surf0_size);

        tq_set_surface(surf1);
        tq_set_clear_color(tq_c24(126, 37, 83));
        tq_set_outline_color(tq_c24(194, 195, 199));
        tq_clear();
        draw_spinning_square(rotation1, surf1_size);

        tq_set_surface(surf2);
        tq_set_clear_color(tq_c24(0, 135, 81));
        tq_set_outline_color(tq_c24(194, 195, 199));
        tq_clear();
        draw_spinning_square(rotation2, surf2_size);

        tq_reset_surface();
        tq_set_clear_color(tq_c24(32, 32, 32));
        tq_clear();

        tq_draw_texture(tq_get_surface_texture(surf0),
            (tq_rectf) {32, 192, 128, 128});
        tq_draw_texture(tq_get_surface_texture(surf1),
            (tq_rectf) {192, 192, 128, 128});
        tq_draw_texture(tq_get_surface_texture(surf2),
            (tq_rectf) {352, 192, 128, 128});
    }

    return 0;
}

//------------------------------------------------------------------------------
