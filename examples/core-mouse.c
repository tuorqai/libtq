
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [core/mouse]
// This example shows how to use mouse-related functions.
//
// What happens here:
// Spawn circles (bubbles) under the mouse cursor if a user presses left mouse
// button. While they hold the button, make the bubble larger.
// When the button is released, the bubble starts to shrink until it disappears.
//------------------------------------------------------------------------------

// Maximum number of bubbles is 64.
#define NUM_BUBBLES 64

// Define a struct to hold bubble information.
typedef struct Bubble
{
    tq_vec2f pos;   // Center
    float rad;      // Radius
} Bubble;

// Static array of bubbles.
Bubble bubbles[NUM_BUBBLES];

int current_bubble = 0;     // Index that should be used next.
int active_bubble = -1;     // Index of active bubble (-1: none)
float crosshair_size = 8;   // Size of crosshair, in pixels

// Mouse button callback function.
// This function will be called by tq library when mouse button is pressed.
// 1st parameter holds current mouse cursor position, meanwhile
// the 2nd one tells you which button is pressed.
void on_button_down(tq_vec2i pos, tq_mouse_button button)
{
    if (button == TQ_MOUSE_BUTTON_LEFT) {
        bubbles[current_bubble].pos = tq_vec2i_cast(pos);
        bubbles[current_bubble].rad = 16.0f;

        active_bubble = current_bubble;
        current_bubble = (current_bubble + 1) % NUM_BUBBLES;
    }
}

// Same as above, but called when a button is released.
void on_button_up(tq_vec2i pos, tq_mouse_button button)
{
    if (button == TQ_MOUSE_BUTTON_LEFT) {
        active_bubble = -1;
    }
}

// TODO: comments.
void on_wheel_scroll(tq_vec2i cursor, tq_vec2f wheel)
{
    crosshair_size += wheel.y * 2.0f;

    if (crosshair_size < 2) {
        crosshair_size = 2;
    }

    if (crosshair_size > 64) {
        crosshair_size = 64;
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] core/mouse");
    tq_set_antialiasing_level(4);

    tq_initialize();
    atexit(tq_terminate);

    // Set light gray background.
    tq_set_clear_color((tq_color) {224, 224, 224, 255});

    // Set mouse callback functions.
    // To remove the callback, you can pass NULL pointer.
    tq_on_mouse_button_pressed(on_button_down);
    tq_on_mouse_button_released(on_button_up);
    tq_on_mouse_wheel_scrolled(on_wheel_scroll);

    // Other than this, tq library also supports setting callbacks for
    // these events:
    // * mouse cursor is moved (tq_on_mouse_cursor_moved())

    // You can also hide mouse cursor.
    tq_set_mouse_cursor_hidden(true);

    while (tq_process()) {
        // Get delta time. It's an amount of time passed between current
        // and previous frames, in seconds.
        float dt = (float) tq_get_delta_time();

        // Get mouse cursor position (integer vector).
        // Since in most API functions we need floating-point vectors,
        // an integer vector can be easily cast to floating-point one
        // with tq_vec2i_cast().
        tq_vec2f cursor_pos = tq_vec2i_cast(tq_get_mouse_cursor_position());

        // Logic of bubbles goes here. It's pretty simple.
        // If bubble has radius more than 0, it should slowly shrink.
        // But if it's last spawned bubble (the user is holding a button)
        // it should grow and follow cursor.
        for (int i = 0; i < NUM_BUBBLES; i++) {
            if (bubbles[i].rad > 0.0f) {
                if (active_bubble == i) {
                    bubbles[i].pos = cursor_pos;
                    bubbles[i].rad += 24.0f * dt;
                } else {
                    bubbles[i].rad -= 16.0f * dt;
                }
            }
        }

        // Clear screen.
        tq_clear();

        // Draw all bubbles with positive radius.
        tq_set_draw_color((tq_color) {32, 32, 32, 255});
        tq_set_outline_color((tq_color) {224, 224, 224, 255});
        
        for (int i = 0; i < NUM_BUBBLES; i++) {
            if (bubbles[i].rad > 0.0f) {
                // Fill and outline circle area.
                tq_draw_circle(bubbles[i].pos, bubbles[i].rad);
            }
        }

        // Draw crosshair.
        tq_set_draw_color((tq_color) {255, 0, 0, 255});
        tq_push_matrix();
        tq_translate_matrix(cursor_pos);
        tq_draw_line((tq_vec2f) {-crosshair_size, 0.0f}, (tq_vec2f) {crosshair_size, 0.0f});
        tq_draw_line((tq_vec2f) {0.0f, -crosshair_size}, (tq_vec2f) {0.0f, crosshair_size});
        tq_pop_matrix();
    }

    return 0;
}

//------------------------------------------------------------------------------
