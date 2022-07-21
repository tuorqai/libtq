
#ifndef TQ_MATH_H
#define TQ_MATH_H

//------------------------------------------------------------------------------

#include <math.h>

#if !defined(M_PI)
    #define M_PI 3.14159265358979323846
#endif // !defined(M_PI)

#define RADIANS(deg) ((deg) * (M_PI / 180.0))
#define DEGREES(rad) ((rad) * (180.0 / M_PI))

//------------------------------------------------------------------------------

void mat3_identity(float *mat);
void mat3_copy(float *dst, float const *src);
void mat3_multiply(float *mat, float const *n);
void mat3_translate(float *mat, float x, float y);
void mat3_scale(float *mat, float x, float y);
void mat3_rotate(float *mat, float rad);

void mat4_identity(float *mat);
void mat4_copy(float *dst, float const *src);
void mat4_expand(float *mat, float const *mat3);
void mat4_multiply(float *mat, float const *n);
void mat4_ortho(float *mat, float left, float right, float bottom, float top, float n, float f);
void mat4_translate(float *mat, float x, float y, float z);
void mat4_rotate(float *mat, float rad, float x, float y, float z);
void mat4_inverse(float const *mat, float *dst);
void mat4_transform_point(float const *mat, float u, float v, float *x, float *y);

//------------------------------------------------------------------------------

#endif // TQ_MATH_H
