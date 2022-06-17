
#ifndef TQ_RENDERER_H_INC
#define TQ_RENDERER_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

struct renderer
{
    void (*initialize)(void);
    void (*terminate)(void);
    void (*clear)(void);
    void (*set_clear_color)(tq_color_t);
    void (*set_view)(float, float, float, float, float);
    void (*reset_view)(void);
    void (*transform)(float const *);
    void (*draw_points)(float const *, unsigned int);
    void (*draw_lines)(float const *, unsigned int);
    void (*draw_outline)(float const *, unsigned int);
    void (*draw_fill)(float const *, unsigned int);
    void (*set_point_color)(tq_color_t);
    void (*set_line_color)(tq_color_t);
    void (*set_outline_color)(tq_color_t);
    void (*set_fill_color)(tq_color_t);
    tq_handle_t (*load_texture)(uint8_t const *, size_t);
    void (*delete_texture)(tq_handle_t);
    void (*get_texture_size)(tq_handle_t, uint32_t *, uint32_t *);
    void (*draw_texture)(float const *, unsigned int, tq_handle_t);
    void (*flush)(void);
};

//------------------------------------------------------------------------------

#if defined(TQ_USE_SFML)
    void construct_sf_renderer(struct renderer *renderer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_RENDERER_H_INC
