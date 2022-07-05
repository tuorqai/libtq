//------------------------------------------------------------------------------
// Copyright (c) 2021-2022 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------
// tq library: simple example
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    tq_set_display_size(512, 512);
    tq_set_title("Hello, tq!");

    tq_initialize();

    tq_handle_t moon = tq_load_texture_from_file("assets/moon.png");
    tq_handle_t ostrich = tq_open_music_from_file("assets/ostrich.ogg");

    tq_set_clear_color(TQ_COLOR24(237, 199, 229));
    tq_set_outline_color(TQ_COLOR24(40, 112, 106));
    tq_set_fill_color(TQ_COLOR24(213, 237, 199));

    tq_handle_t ostrich_wave = tq_play_music(ostrich, -1);

    while (tq_process()) {
        if (tq_is_key_pressed(TQ_KEY_ESCAPE)) {
            break;
        }

        tq_clear();
        tq_draw_rectangle_f(128.0f, 128.0f, 256.0f, 256.0f);
        tq_draw_texture_f(moon, 128.0f, 128.0f, 256.0f, 256.0f);
    }

    tq_terminate();

    return 0;
}

//------------------------------------------------------------------------------
