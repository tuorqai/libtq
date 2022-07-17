
//------------------------------------------------------------------------------
// tq library: tq_text.h
//------------------------------------------------------------------------------

#ifndef TQ_TEXT_H_INC
#define TQ_TEXT_H_INC

//------------------------------------------------------------------------------

#include "tq_graphics.h"

//------------------------------------------------------------------------------

void text_initialize(struct renderer_impl const *renderer);
void text_terminate(void);

int text_load_font(int stream_id, float pt, int weight);
int text_load_font_from_file(char const *path, float pt, int weight);
int text_load_font_from_memory(void const *buffer, size_t size, float pt, int weight);

void text_delete_font(int font_id);

void text_draw_text(int font_id, float x, float y, char const *text);

//------------------------------------------------------------------------------

#endif // TQ_TEXT_H_INC

//------------------------------------------------------------------------------
