
//------------------------------------------------------------------------------

#ifndef TQ_CORE_H_INC
#define TQ_CORE_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"
#include "tq_clock.h"
#include "tq_display.h"

//------------------------------------------------------------------------------

void core_initialize(struct clock const *clock, struct display const *display);
void core_terminate(void);

void core_present(void);
void core_keep_up(void);
bool core_process_events(void);

uint32_t core_get_display_width(void);
uint32_t core_get_display_height(void);
void core_get_display_size(uint32_t *width, uint32_t *height);
void core_set_display_size(uint32_t width, uint32_t height);

char const *core_get_title(void);
void core_set_title(char const *title);

bool core_is_key_pressed(tq_key_t key);
bool core_is_mouse_button_pressed(tq_mouse_button_t mouse_button);

int32_t core_get_mouse_cursor_x(void);
int32_t core_get_mouse_cursor_y(void);

float core_get_time_mediump(void);
double core_get_time_highp(void);
double core_get_delta_time(void);

unsigned int core_get_framerate(void);

void core_on_key_pressed(tq_key_t key);
void core_on_key_released(tq_key_t key);

void core_on_mouse_button_pressed(tq_mouse_button_t mouse_button);
void core_on_mouse_button_released(tq_mouse_button_t mouse_button);
void core_on_mouse_cursor_moved(int32_t x, int32_t y);

//------------------------------------------------------------------------------

#endif // TQ_CORE_H_INC
