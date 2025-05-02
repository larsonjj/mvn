/**
 * \file            mvn-types.h
 * \brief           MVN type definitions and aliases
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

#ifndef MVN_TYPES_H
#define MVN_TYPES_H

#include "mvn/mvn-utils.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Type aliases for SDL types
 */

/**
 * \brief           Type definition for MVN image (SDL_Surface wrapper)
 */
typedef SDL_Surface mvn_image_t;

/**
 * \brief           Type definition for MVN texture (SDL_Texture wrapper)
 */
typedef SDL_Texture mvn_texture_t;

/**
 * \brief           Type definition for MVN renderer (SDL_Renderer wrapper)
 */
typedef SDL_Renderer mvn_renderer_t;

/**
 * \brief           Type definition for MVN text engine (SDL_ttf text engine wrapper)
 */
typedef TTF_TextEngine mvn_text_engine_t;

/**
 * \brief           Type definition for MVN window (SDL_Window wrapper)
 */
typedef SDL_Window mvn_window_t;

/**
 * \brief           Type definition for MVN color (SDL_Color wrapper)
 */
typedef SDL_Color mvn_color_t;

/**
 * \brief           Type definition for MVN float rectangle (SDL_FRect wrapper)
 */
typedef SDL_FRect mvn_frect_t;

/**
 * \brief           Type definition for MVN int rectangle (SDL_Rect wrapper)
 */
typedef SDL_Rect mvn_rect_t;

/**
 * \brief           Type definition for MVN float point (SDL_FPoint wrapper)
 */
typedef SDL_FPoint mvn_fpoint_t;

/**
 * \brief           Type definition for MVN int point (SDL_Point wrapper)
 */
typedef SDL_Point mvn_point_t;

/**
 * \brief           Type definition for MVN monitor display ID (SDL_DisplayID wrapper)
 */
typedef SDL_DisplayID mvn_display_id_t;

/**
 * \brief           Predefined color definitions
 */
#define MVN_BLANK MVN_STRUCT(mvn_color_t, {.r = 0, .g = 0, .b = 0, .a = 0})
#define MVN_WHITE MVN_STRUCT(mvn_color_t, {.r = 255, .g = 255, .b = 255, .a = 255})
#define MVN_BLACK MVN_STRUCT(mvn_color_t, {.r = 0, .g = 0, .b = 0, .a = 255})
#define MVN_LIGHTGRAY                                                                              \
    MVN_STRUCT(mvn_color_t, {.r = 200, .g = 200, .b = 200, .a = 255}) /* Light Gray */
#define MVN_GRAY     MVN_STRUCT(mvn_color_t, {.r = 130, .g = 130, .b = 130, .a = 255}) /* Gray */
#define MVN_DARKGRAY MVN_STRUCT(mvn_color_t, {.r = 80, .g = 80, .b = 80, .a = 255}) /* Dark Gray   \
                                                                                     */
#define MVN_YELLOW MVN_STRUCT(mvn_color_t, {.r = 253, .g = 249, .b = 0, .a = 255}) /* Yellow */
#define MVN_GOLD   MVN_STRUCT(mvn_color_t, {.r = 255, .g = 203, .b = 0, .a = 255}) /* Gold */
#define MVN_ORANGE MVN_STRUCT(mvn_color_t, {.r = 255, .g = 161, .b = 0, .a = 255}) /* Orange */
#define MVN_PINK   MVN_STRUCT(mvn_color_t, {.r = 255, .g = 109, .b = 194, .a = 255}) /* Pink */
#define MVN_RED    MVN_STRUCT(mvn_color_t, {.r = 230, .g = 41, .b = 55, .a = 255}) /* Red */
#define MVN_MAROON MVN_STRUCT(mvn_color_t, {.r = 190, .g = 33, .b = 55, .a = 255}) /* Maroon */
#define MVN_GREEN  MVN_STRUCT(mvn_color_t, {.r = 0, .g = 228, .b = 48, .a = 255}) /* Green */
#define MVN_LIME   MVN_STRUCT(mvn_color_t, {.r = 0, .g = 158, .b = 47, .a = 255}) /* Lime */
#define MVN_DARKGREEN                                                                              \
    MVN_STRUCT(mvn_color_t, {.r = 0, .g = 117, .b = 44, .a = 255}) /* Dark Green */
#define MVN_SKYBLUE MVN_STRUCT(mvn_color_t, {.r = 102, .g = 191, .b = 255, .a = 255}) /* Sky Blue  \
                                                                                       */
#define MVN_BLUE     MVN_STRUCT(mvn_color_t, {.r = 0, .g = 121, .b = 241, .a = 255}) /* Blue */
#define MVN_DARKBLUE MVN_STRUCT(mvn_color_t, {.r = 0, .g = 82, .b = 172, .a = 255}) /* Dark Blue   \
                                                                                     */
#define MVN_PURPLE MVN_STRUCT(mvn_color_t, {.r = 200, .g = 122, .b = 255, .a = 255}) /* Purple */
#define MVN_VIOLET MVN_STRUCT(mvn_color_t, {.r = 135, .g = 60, .b = 190, .a = 255}) /* Violet */
#define MVN_DARKPURPLE                                                                             \
    MVN_STRUCT(mvn_color_t, {.r = 112, .g = 31, .b = 126, .a = 255}) /* Dark Purple */
#define MVN_BEIGE MVN_STRUCT(mvn_color_t, {.r = 211, .g = 176, .b = 131, .a = 255}) /* Beige */
#define MVN_BROWN MVN_STRUCT(mvn_color_t, {.r = 127, .g = 106, .b = 79, .a = 255}) /* Brown */
#define MVN_DARKBROWN                                                                              \
    MVN_STRUCT(mvn_color_t, {.r = 76, .g = 63, .b = 47, .a = 255}) /* Dark Brown */
#define MVN_MAGENTA MVN_STRUCT(mvn_color_t, {.r = 255, .g = 0, .b = 255, .a = 255}) /* Magenta */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_TYPES_H */
