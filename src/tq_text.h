
//------------------------------------------------------------------------------
// tq library: tq_text.h
//------------------------------------------------------------------------------

#ifndef TQ_TEXT_H_INC
#define TQ_TEXT_H_INC

//------------------------------------------------------------------------------

#include "tq_graphics.h"

//------------------------------------------------------------------------------

void text_initialize(struct libtq_renderer_impl const *renderer);
void text_terminate(void);

int text_load_font(libtq_stream *stream, float pt, int weight);
int text_load_font_from_file(char const *path, float pt, int weight);
int text_load_font_from_memory(void const *buffer, size_t size, float pt, int weight);

void text_delete_font(int font_id);

int text_get_font_texture(int font_id);

void text_draw_text(int font_id, float x, float y, char const *text);

void text_set_outline_color(tq_color outline_color);
void text_set_fill_color(tq_color fill_color);

//------------------------------------------------------------------------------

#endif // TQ_TEXT_H_INC

//------------------------------------------------------------------------------
