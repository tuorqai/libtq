
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [core/keyboard]
// This example shows how to use mouse routines.
//------------------------------------------------------------------------------

// Default position of a circle: center of the screen.
tq_vec2f circle_pos = {256, 256};

void on_key_pressed(tq_key key)
{
    // Reset circle position if Space key is pressed.

    if (key == TQ_KEY_SPACE) {
        circle_pos = (tq_vec2f) {256, 256};
    }
}

int main(int argc, char *argv[])
{
    tq_set_display_size((tq_vec2i) {512, 512});
    tq_set_title("[tq library] core/keyboard");

    tq_initialize();
    atexit(tq_terminate);

    // Set key press callback.
    tq_on_key_pressed(on_key_pressed);

    // Set dark green background.
    tq_set_clear_color((tq_color) {10, 22, 15, 255});

    // Set white outline.
    tq_set_outline_color((tq_color) {255, 255, 255, 255});

    while (tq_process()) {
        // Use tq_get_delta_time() function to get delta time.
        float speed = 400.0f * tq_get_delta_time();

        // Move circle up if Up key is pressed.
        if (tq_is_key_pressed(TQ_KEY_UP)) {
            circle_pos.y -= speed;
        }

        // Move circle down if Down key is pressed.
        if (tq_is_key_pressed(TQ_KEY_DOWN)) {
            circle_pos.y += speed;
        }

        // Move circle left if Left key is pressed.
        if (tq_is_key_pressed(TQ_KEY_LEFT)) {
            circle_pos.x -= speed;
        }

        // Move circle right if Right key is pressed.
        if (tq_is_key_pressed(TQ_KEY_RIGHT)) {
            circle_pos.x += speed;
        }

        // Keep circle inside of screen.
        if (circle_pos.x < 0.0f) {
            circle_pos.x = 0.0f;
        }

        if (circle_pos.y < 0.0f) {
            circle_pos.y = 0.0f;
        }

        if (circle_pos.x > 512.0f) {
            circle_pos.x = 512.0f;
        }

        if (circle_pos.y > 512.0f) {
            circle_pos.y = 512.0f;
        }

        // Clear the screen before drawing.
        tq_clear();

        // Draw our circle.
        tq_outline_circle(circle_pos, 32.0f);
    }

    return 0;
}

//------------------------------------------------------------------------------
