//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
