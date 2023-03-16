
//------------------------------------------------------------------------------

#include <tq/tq.h>

//------------------------------------------------------------------------------
// [core/hello]
// This example shows some basic usage of tq library.
//------------------------------------------------------------------------------

static void loop(void)
{
    // Clear the screen using background color we set earlier.
    tq_clear();

    // Draw yellowish circle.
    tq_outline_circle((tq_vec2f) {256, 256}, 100);
}

int main(int argc, char *argv[])
{
    // Set initial display size.
    // It's safe to call this function before tq library is initialized.
    tq_set_display_size((tq_vec2i) {512, 512});

    // Set title of the window (has no effect on Android).
    // It's safe to call this function before tq library is initialized.
    tq_set_title("[tq library] core/hello");

    // Initialize tq library.
    // If it fails to initialize, abort() will be called and
    // your application will be terminated.
    tq_initialize();

    // Set background color (used by tq_clear()).
    tq_set_clear_color((tq_color) {0, 0, 30, 255});

    // Set outline color (used by some graphics primitives,
    // such as rectangles and circles).
    tq_set_outline_color((tq_color) {180, 180, 0, 255});

    // Give control to the library. This function never returns.
    // loop() will be called every frame until application exits.
    tq_run(loop);

    // As a result, you should see a square window with a yellow circle
    // on the center.

    return 0;
}

//------------------------------------------------------------------------------
