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
#include "mvn/mvn-utils.h" // IWYU pragma: keep

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Window flags for use with mvn_init
 * \hideinitializer
 */
typedef uint64_t mvn_window_flags_t;
#define MVN_WINDOW_FULLSCREEN          SDL_WINDOW_FULLSCREEN
#define MVN_WINDOW_HIDDEN              SDL_WINDOW_HIDDEN
#define MVN_WINDOW_BORDERLESS          SDL_WINDOW_BORDERLESS
#define MVN_WINDOW_RESIZABLE           SDL_WINDOW_RESIZABLE
#define MVN_WINDOW_MINIMIZED           SDL_WINDOW_MINIMIZED
#define MVN_WINDOW_MAXIMIZED           SDL_WINDOW_MAXIMIZED
#define MVN_WINDOW_MOUSE_GRABBED       SDL_WINDOW_MOUSE_GRABBED
#define MVN_WINDOW_INPUT_FOCUS         SDL_WINDOW_INPUT_FOCUS
#define MVN_WINDOW_MOUSE_FOCUS         SDL_WINDOW_MOUSE_FOCUS
#define MVN_WINDOW_HIGH_PIXEL_DENSITY  SDL_WINDOW_HIGH_PIXEL_DENSITY
#define MVN_WINDOW_MOUSE_CAPTURE       SDL_WINDOW_MOUSE_CAPTURE
#define MVN_WINDOW_MOUSE_RELATIVE_MODE SDL_WINDOW_MOUSE_RELATIVE_MODE
#define MVN_WINDOW_ALWAYS_ON_TOP       SDL_WINDOW_ALWAYS_ON_TOP
#define MVN_WINDOW_KEYBOARD_GRABBED    SDL_WINDOW_KEYBOARD_GRABBED
#define MVN_WINDOW_TRANSPARENT         SDL_WINDOW_TRANSPARENT

/* Core functions */
mvn_string_t *     mvn_get_engine_version(void);
bool               mvn_init(int width, int height, const char *title, mvn_window_flags_t flags);
void               mvn_quit(void);
mvn_window_t *     mvn_get_window(void);
mvn_renderer_t *   mvn_get_renderer(void);
mvn_text_engine_t *mvn_get_text_engine(void);
bool               mvn_window_should_close(void);
bool               mvn_begin_drawing(void);
bool               mvn_clear_background(mvn_color_t color);
bool               mvn_end_drawing(void);

/* Window management functions */
bool             mvn_toggle_fullscreen(void);
bool             mvn_toggle_borderless_windowed(void);
bool             mvn_maximize_window(void);
bool             mvn_minimize_window(void);
bool             mvn_restore_window(void);
mvn_display_id_t mvn_get_current_monitor(void);
mvn_fpoint_t     mvn_get_monitor_position(mvn_display_id_t monitor);
int32_t          mvn_get_monitor_width(mvn_display_id_t monitor);
int32_t          mvn_get_monitor_height(mvn_display_id_t monitor);
int32_t          mvn_get_monitor_refresh_rate(mvn_display_id_t monitor);
mvn_fpoint_t     mvn_get_window_position(void);
mvn_fpoint_t     mvn_get_window_scale_dpi(void);
mvn_string_t *   mvn_get_monitor_name(mvn_display_id_t monitor);
void             mvn_set_window_icon(mvn_image_t *image);
void             mvn_set_window_title(const char *title);
void             mvn_set_window_position(int32_t x, int32_t y);
void             mvn_set_window_monitor(mvn_display_id_t monitor);
void             mvn_set_window_min_size(int32_t width, int32_t height);
void             mvn_set_window_max_size(int32_t width, int32_t height);
void             mvn_set_window_size(int32_t width, int32_t height);
bool             mvn_set_window_opacity(float opacity);
void             mvn_set_window_focused(void);
int32_t          mvn_get_screen_width(void);
int32_t          mvn_get_screen_height(void);
int32_t          mvn_get_render_width(void);
int32_t          mvn_get_render_height(void);
int32_t          mvn_get_monitor_count(void);

/* Cursor functions */
void mvn_show_cursor(void);
void mvn_hide_cursor(void);
bool mvn_is_cursor_hidden(void);
void mvn_enable_cursor(void);
void mvn_disable_cursor(void);
bool mvn_is_cursor_on_screen(void);

/* Timing functions */
void   mvn_set_target_fps(int fps);
float  mvn_get_frame_time(void);
double mvn_get_time(void);
int    mvn_get_fps(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_CORE_H */
