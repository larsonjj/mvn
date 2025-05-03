/**
 * \file            mvn-core.h
 * \brief           MVN core framework header
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

#ifndef MVN_CORE_H
#define MVN_CORE_H

#include "mvn/mvn-file.h"   // IWYU pragma: keep
#include "mvn/mvn-logger.h" // IWYU pragma: keep
#include "mvn/mvn-string.h"
#include "mvn/mvn-text.h"    // IWYU pragma: keep
#include "mvn/mvn-texture.h" // IWYU pragma: keep
#include "mvn/mvn-types.h"
#include "mvn/mvn-utils.h"  // IWYU pragma: keep
#include "mvn/mvn-window.h" // IWYU pragma: keep

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Core functions */
mvn_string_t      *mvn_get_engine_version(void);
bool               mvn_init(int width, int height, const char *title, mvn_window_flags_t flags);
void               mvn_quit(void);
mvn_renderer_t    *mvn_get_renderer(void);
mvn_text_engine_t *mvn_get_text_engine(void);
bool               mvn_window_should_close(void);
bool               mvn_begin_drawing(void);
bool               mvn_clear_background(mvn_color_t color);
bool               mvn_end_drawing(void);

/* Timing functions */
void   mvn_set_target_fps(int fps);
float  mvn_get_frame_time(void);
double mvn_get_time(void);
int    mvn_get_fps(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_CORE_H */
