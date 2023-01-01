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

#ifndef TQ_TEXT_H_INC
#define TQ_TEXT_H_INC

//------------------------------------------------------------------------------

#include "tq_graphics.h"

//------------------------------------------------------------------------------

void tq_initialize_text(tq_renderer_impl *renderer);
void tq_terminate_text(void);

void tq_set_text_color(tq_color text_color);
void tq_set_text_outline_color(tq_color outline_color);

//------------------------------------------------------------------------------

#endif // TQ_TEXT_H_INC

//------------------------------------------------------------------------------
