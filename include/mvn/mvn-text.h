/**
 * \file            mvn-text.h
 * \brief           MVN text and font management functionality
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */

#ifndef MVN_TEXT_H
#define MVN_TEXT_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include "mvn/mvn-types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Function declarations */
TTF_Font* mvn_load_font(const char* fileName, float size);
TTF_Font* mvn_load_font_ex(const char* fileName, float size, const int* codePoints,
                           int codePointCount);
void mvn_unload_font(TTF_Font* font);
void mvn_set_text_line_spacing(int32_t spacing);
int32_t mvn_measure_text(TTF_Font* font, const char* text, float spacing);
void mvn_draw_text(TTF_Font* font, const char* text, mvn_fpoint_t position, mvn_color_t tint);
void mvn_draw_text_pro(TTF_Font* font, const char* text, mvn_fpoint_t position, mvn_fpoint_t origin,
                       float rotation, mvn_color_t tint);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_TEXT_H */
