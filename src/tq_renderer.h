
#ifndef TQ_RENDERER_H_INC
#define TQ_RENDERER_H_INC

//------------------------------------------------------------------------------

#include "tq_stream.h"

//------------------------------------------------------------------------------

typedef struct tq_renderer
{
    void        (*initialize)(void);
    void        (*terminate)(void);

    void        (*clear)(void);
    void        (*set_clear_color)(tq_color_t);

    void        (*update_viewport)(int x, int y, int w, int h);
    void        (*update_projection)(float const *mat4);
    void        (*update_model_view)(float const *mat3);

    void        (*draw_points)(float const *, unsigned int);
    void        (*draw_lines)(float const *, unsigned int);
    void        (*draw_outline)(float const *, unsigned int);
    void        (*draw_fill)(float const *, unsigned int);

    void        (*set_point_color)(tq_color_t);
    void        (*set_line_color)(tq_color_t);
    void        (*set_outline_color)(tq_color_t);
    void        (*set_fill_color)(tq_color_t);

    int32_t     (*load_texture)(stream_t const *stream);
    void        (*delete_texture)(int32_t texture_id);
    void        (*get_texture_size)(int32_t texture_id, uint32_t *width, uint32_t *height);
    void        (*draw_texture)(int32_t texture_id, float const *data, uint32_t num_vertices);

    void        (*flush)(void);
} tq_renderer_t;

//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENGL)
    void tq_construct_gl_renderer(tq_renderer_t *renderer);
#endif

//------------------------------------------------------------------------------

#endif // TQ_RENDERER_H_INC
