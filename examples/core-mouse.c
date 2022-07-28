
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [core/mouse]
// This example shows how to use mouse routines.
//------------------------------------------------------------------------------

// Global variable that holds circle radius.
float circle_radius = 16.0f;

// Mouse wheel callback function.
// This function will be called by tq library when mouse wheel
// is scrolled.
// `cursor` parameter holds current mouse cursor position, meanwhile
// `wheel` parameter holds wheel scroll delta.
// X coordinate is, obviously, a horizontal scroll delta and Y one
// is vertical.
void on_mouse_wheel_scrolled(tq_vec2i cursor, tq_vec2f wheel)
{
    if (wheel.y < 0.0f) {
        circle_radius -= 4.0f; // Make circle smaller
    } else if (wheel.y > 0.0f) {
        circle_radius += 4.0f; // Make it bigger
    }

    // But keep it larger than 8px and smaller than 64px.

    if (circle_radius < 8.0f) {
        circle_radius = 8.0f;
    }

    if (circle_radius > 64.0f) {
        circle_radius = 64.0f;
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] core/mouse");

    tq_initialize();
    atexit(tq_terminate);

    // Set dark gray background.
    tq_set_clear_color((tq_color) {20, 20, 20, 255});

    // Set mouse wheel callback function.
    // To remove the callback, you can pass NULL pointer.
    tq_on_mouse_wheel_scrolled(on_mouse_wheel_scrolled);

    // Other than this, tq library also supports setting callbacks for
    // these events:
    // * mouse button is clicked (tq_on_mouse_button_pressed())
    // * mouse button is released (tq_on_mouse_button_released())
    // * mouse cursor is moved (tq_on_mouse_cursor_moved())

    while (tq_process()) {
        tq_clear();

        // If left mouse button is pressed, then make our circle red.
        // Or white, if the button isn't pressed.
        if (tq_is_mouse_button_pressed(TQ_MOUSE_BUTTON_LEFT)) {
            tq_set_outline_color((tq_color) {255, 0, 0, 255});
        } else {
            tq_set_outline_color((tq_color) {255, 255, 255, 255});
        }

        // Get mouse cursor position (integer vector).
        // Since in most API functions we need floating-point vectors,
        // an integer vector can be easily cast to floating-point one
        // with tq_vec2i_cast().
        tq_vec2f cursor = tq_vec2i_cast(tq_get_mouse_cursor_position());

        tq_outline_circle(cursor, circle_radius);
    }

    return 0;
}

//------------------------------------------------------------------------------
