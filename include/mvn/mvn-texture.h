/**
 * \file            mvn-texture.h
 * \brief           MVN texture loading and management functionality
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

#ifndef MVN_TEXTURES_H
#define MVN_TEXTURES_H

#include "mvn/mvn-types.h"

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           N-patch layout types
 */
typedef enum {
    NPATCH_NINE_PATCH = 0,        /*!< Npatch layout: 3x3 tiles */
    NPATCH_THREE_PATCH_VERTICAL,  /*!< Npatch layout: 1x3 tiles */
    NPATCH_THREE_PATCH_HORIZONTAL /*!< Npatch layout: 3x1 tiles */
} mvn_npatch_layout_t;

/**
 * \brief           N-patch information structure
 */
typedef struct mvn_npatch_info_t {
    mvn_rect_t          source; /*!< Texture source rectangle */
    int32_t             left;   /*!< Left border offset */
    int32_t             top;    /*!< Top border offset */
    int32_t             right;  /*!< Right border offset */
    int32_t             bottom; /*!< Bottom border offset */
    mvn_npatch_layout_t layout; /*!< Layout of the n-patch: 3x3, 1x3 or 3x1 */
} mvn_npatch_info_t;

mvn_image_t *  mvn_load_image(const char *filename);
void           mvn_unload_image(mvn_image_t *surface);
mvn_texture_t *mvn_image_to_texture(mvn_renderer_t *renderer, mvn_image_t *surface);
mvn_texture_t *mvn_load_texture(mvn_renderer_t *renderer, const char *filename);
void           mvn_unload_texture(mvn_texture_t *texture);
void mvn_draw_texture(mvn_texture_t *texture, int32_t posX, int32_t posY, mvn_color_t tint);
void mvn_draw_texture_v(mvn_texture_t *texture, mvn_fpoint_t position, mvn_color_t tint);
void mvn_draw_texture_ex(mvn_texture_t *texture,
                         mvn_fpoint_t   position,
                         float          rotation,
                         float          scale,
                         mvn_color_t    tint);
void mvn_draw_texture_rec(mvn_texture_t *texture,
                          mvn_frect_t    source,
                          mvn_fpoint_t   position,
                          mvn_color_t    tint);
void mvn_draw_texture_pro(mvn_texture_t *texture,
                          mvn_frect_t    source,
                          mvn_frect_t    dest,
                          mvn_fpoint_t   origin,
                          float          rotation,
                          mvn_color_t    tint);
void mvn_draw_texture_npatch(mvn_texture_t *   texture,
                             mvn_npatch_info_t nPatchInfo,
                             mvn_frect_t       dest,
                             mvn_fpoint_t      origin,
                             float             rotation,
                             mvn_color_t       tint);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_TEXTURES_H */
