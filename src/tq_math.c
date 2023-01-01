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

#include <string.h>

#include "tq_math.h"

//------------------------------------------------------------------------------
// 3x3 matrices

void mat3_identity(float *mat)
{
    static float const identity[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    memcpy(mat, identity, 9 * sizeof(float));
}

void mat3_copy(float *dst, float const *src)
{
    memcpy(dst, src, 9 * sizeof(float));
}

void mat3_multiply(float *mat, float const *n)
{
    float const result[] = {
        mat[0] * n[0] + mat[1] * n[3] + mat[2] * n[6],
        mat[0] * n[1] + mat[1] * n[4] + mat[2] * n[7],
        mat[0] * n[2] + mat[1] * n[5] + mat[2] * n[8],
        mat[3] * n[0] + mat[4] * n[3] + mat[5] * n[6],
        mat[3] * n[1] + mat[4] * n[4] + mat[5] * n[7],
        mat[3] * n[2] + mat[4] * n[5] + mat[5] * n[8],
        mat[6] * n[0] + mat[7] * n[6] + mat[8] * n[6],
        mat[6] * n[1] + mat[7] * n[7] + mat[8] * n[7],
        mat[6] * n[2] + mat[7] * n[8] + mat[8] * n[8],
    };

    memcpy(mat, result, 9 * sizeof(float));
}

void mat3_translate(float *mat, float x, float y)
{
    float const translation[] = {
        1.0f, 0.0f, x,
        0.0f, 1.0f, y,
        0.0f, 0.0f, 1.0f,
    };

    mat3_multiply(mat, translation);
}

void mat3_scale(float *mat, float x, float y)
{
    float const scale[] = {
        x,      0.0f,   0.0f,
        0.0f,   y,      0.0f,
        0.0f,   0.0f,   1.0f,
    };

    mat3_multiply(mat, scale);
}

void mat3_rotate(float *mat, float rad)
{
    float c = cosf(rad);
    float s = sinf(rad);

    float const rotation[] = {
        c,      -s,     0.0f,
        s,      c,      0.0f,
        0.0f,   0.0f,   1.0f,
    };

    mat3_multiply(mat, rotation);
}

//------------------------------------------------------------------------------
// 4x4 matrices

void mat4_identity(float *mat)
{
    static float const identity[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    memcpy(mat, identity, 16 * sizeof(float));
}

void mat4_copy(float *dst, float const *src)
{
    memcpy(dst, src, 16 * sizeof(float));
}

void mat4_expand(float *mat, float const *mat3)
{
    mat[ 0] = mat3[0];  mat[ 1] = mat3[1];  mat[ 2] = 0.0f;     mat[ 3] = mat3[2];
    mat[ 4] = mat3[3];  mat[ 5] = mat3[4];  mat[ 6] = 0.0f;     mat[ 7] = mat3[5];
    mat[ 8] = 0.0f;     mat[ 9] = 0.0f;     mat[10] = 1.0f;     mat[11] = 0.0f;
    mat[12] = mat3[6];  mat[13] = mat3[7];  mat[14] = 0.0f;     mat[15] = mat3[8];
}

void mat4_multiply(float *mat, float const *n)
{
    float const result[] = {
        mat[ 0] * n[ 0] + mat[ 1] * n[ 4] + mat[ 2] * n[ 8] + mat[ 3] * n[12],
        mat[ 0] * n[ 1] + mat[ 1] * n[ 5] + mat[ 2] * n[ 9] + mat[ 3] * n[13],
        mat[ 0] * n[ 2] + mat[ 1] * n[ 6] + mat[ 2] * n[10] + mat[ 3] * n[14],
        mat[ 0] * n[ 3] + mat[ 1] * n[ 7] + mat[ 2] * n[11] + mat[ 3] * n[15],
        mat[ 4] * n[ 0] + mat[ 5] * n[ 4] + mat[ 6] * n[ 8] + mat[ 7] * n[12],
        mat[ 4] * n[ 1] + mat[ 5] * n[ 5] + mat[ 6] * n[ 9] + mat[ 7] * n[13],
        mat[ 4] * n[ 2] + mat[ 5] * n[ 6] + mat[ 6] * n[10] + mat[ 7] * n[14],
        mat[ 4] * n[ 3] + mat[ 5] * n[ 7] + mat[ 6] * n[11] + mat[ 7] * n[15],
        mat[ 8] * n[ 0] + mat[ 9] * n[ 4] + mat[10] * n[ 8] + mat[11] * n[12],
        mat[ 8] * n[ 1] + mat[ 9] * n[ 5] + mat[10] * n[ 9] + mat[11] * n[13],
        mat[ 8] * n[ 2] + mat[ 9] * n[ 6] + mat[10] * n[10] + mat[11] * n[14],
        mat[ 8] * n[ 3] + mat[ 9] * n[ 7] + mat[10] * n[11] + mat[11] * n[15],
        mat[12] * n[ 0] + mat[13] * n[ 4] + mat[14] * n[ 8] + mat[15] * n[12],
        mat[12] * n[ 1] + mat[13] * n[ 5] + mat[14] * n[ 9] + mat[15] * n[13],
        mat[12] * n[ 2] + mat[13] * n[ 6] + mat[14] * n[10] + mat[15] * n[14],
        mat[12] * n[ 3] + mat[13] * n[ 7] + mat[14] * n[11] + mat[15] * n[15],
    };

    memcpy(mat, result, 16 * sizeof(float));
}

void mat4_ortho(float *mat, float left, float right, float bottom, float top, float n, float f)
{
    mat[ 0] = 2.0f / (right - left);
    mat[ 1] = 0.0f;
    mat[ 2] = 0.0f;
    mat[ 3] = -(right + left) / (right - left);

    mat[ 4] = 0.0f;
    mat[ 5] = 2.0f / (top - bottom);
    mat[ 6] = 0.0f;
    mat[ 7] = -(top + bottom) / (top - bottom);

    mat[ 8] = 0.0f;
    mat[ 9] = 0.0f;
    mat[10] = -2.0f / (f - n);
    mat[11] = -(f + n) / (f - n);

    mat[12] = 0.0f;
    mat[13] = 0.0f;
    mat[14] = 0.0f;
    mat[15] = 1.0f;
}

void mat4_translate(float *mat, float x, float y, float z)
{
    float const translation[] = {
        1.0f, 0.0f, 0.0f, x,
        0.0f, 1.0f, 0.0f, y,
        0.0f, 0.0f, 0.0f, z,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    mat4_multiply(mat, translation);
}

void mat4_rotate(float *mat, float rad, float x, float y, float z)
{
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;

    float c = cosf(rad);
    float s = sinf(rad);
    float ci = 1.0f - c;

    float xs = x * s;
    float ys = y * s;
    float zs = z * s;

    float const rotation[] = {
        xx * ci + c,    xy * ci - zs,   xz * ci + ys,   0.0f,
        y  * ci + zs,   yy * ci + c,    yz * ci - xs,   0.0f,
        xz * ci - ys,   yz * ci + xs,   zz * ci + c,    0.0f,
        0.0f,           0.0f,           0.0f,           1.0f,
    };

    mat4_multiply(mat, rotation);
}

void mat4_inverse(float const *mat, float *dst)
{
    float det =
        mat[0] * (mat[15] * mat[5] - mat[7] * mat[13]) -
        mat[1] * (mat[15] * mat[4] - mat[7] * mat[12]) +
        mat[3] * (mat[13] * mat[4] - mat[5] * mat[12]);
    
    if (det == 0.0f) {
        mat4_identity(dst);
        return;
    }

    dst[ 0] = +(mat[15] * mat[5] - mat[7] * mat[13]) / det;
    dst[ 1] = -(mat[15] * mat[4] - mat[7] * mat[12]) / det;
    dst[ 2] = 0.0f;
    dst[ 3] = +(mat[13] * mat[4] - mat[5] * mat[12]) / det;

    dst[ 4] = -(mat[15] * mat[1] - mat[3] * mat[13]) / det;
    dst[ 5] = +(mat[15] * mat[0] - mat[3] * mat[12]) / det;
    dst[ 6] = 0.0f;
    dst[ 7] = -(mat[13] * mat[0] - mat[1] * mat[12]) / det;

    dst[ 8] = 0.0f;
    dst[ 9] = 0.0f;
    dst[10] = 1.0f;
    dst[11] = 0.0f;

    dst[12] = +(mat[7]  * mat[1] - mat[3] * mat[5])  / det;
    dst[13] = -(mat[7]  * mat[0] - mat[3] * mat[4])  / det;
    dst[14] = 0.0f;
    dst[15] = +(mat[5]  * mat[0] - mat[1] * mat[4])  / det;
}

void mat4_transform_point(float const *mat, float u, float v, float *x, float *y)
{
    *x = mat[0] * u + mat[4] * v + mat[12];
    *y = mat[1] * u + mat[5] * v + mat[13];
}

//------------------------------------------------------------------------------
