/**
 * \file            mvn-window.c
 * \brief           MVN window management functionality implementation
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

#include "mvn/mvn-window.h"

#include "mvn/mvn-core.h"
#include "mvn/mvn-error.h"
#include "mvn/mvn-logger.h"
#include "mvn/mvn-utils.h"

#include <SDL3/SDL.h>

/* External reference to the window created in mvn-core.c */
extern mvn_window_t *g_window;

/**
 * \brief           Get the SDL window
 * \return          Pointer to the SDL window, NULL if not initialized
 */
mvn_window_t *mvn_get_window(void)
{
    return g_window;
}

/**
 * \brief           Toggle window state: fullscreen/windowed
 * \note            Resizes monitor to match window resolution
 * \return          true on success, false on failure
 */
bool mvn_toggle_fullscreen(void)
{
    mvn_window_t *window = mvn_get_window();
    MVN_CHECK_NULL(window, "Cannot toggle fullscreen: No window available");

    // Check current fullscreen state
    bool is_fullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;

    // Toggle fullscreen state
    if (SDL_SetWindowFullscreen(window, !is_fullscreen)) {
        return mvn_set_error("Failed to toggle fullscreen mode: %s", SDL_GetError());
    }

    return true;
}

/**
 * \brief           Toggle window state: borderless windowed
 * \note            Resizes window to match monitor resolution
 * \return          true on success, false on failure
 */
bool mvn_toggle_borderless_windowed(void)
{
    mvn_window_t *window = mvn_get_window();
    MVN_CHECK_NULL(window, "Cannot toggle borderless windowed mode: No window available");

    SDL_WindowFlags flags         = SDL_GetWindowFlags(window);
    bool            is_borderless = flags & SDL_WINDOW_BORDERLESS;
    bool            is_fullscreen = flags & SDL_WINDOW_FULLSCREEN;

    // If in fullscreen, exit fullscreen first
    if (is_fullscreen) {
        if (SDL_SetWindowFullscreen(window, false)) {
            return mvn_set_error("Failed to exit fullscreen mode: %s", SDL_GetError());
        }
    }

    // Toggle borderless state
    if (SDL_SetWindowBordered(window, is_borderless)) {
        return mvn_set_error("Failed to toggle window border: %s", SDL_GetError());
    }

    // If switching to borderless, resize to match monitor
    if (!is_borderless) {
        mvn_display_id_t monitor = mvn_get_current_monitor();
        int32_t          width   = mvn_get_monitor_width(monitor);
        int32_t          height  = mvn_get_monitor_height(monitor);
        if (width > 0 && height > 0) {
            SDL_SetWindowSize(window, width, height);
            SDL_SetWindowPosition(window, 0, 0);
        }
    }

    return true;
}

/**
 * \brief           Set window state: maximized, if resizable
 * \return          true on success, false on failure
 */
bool mvn_maximize_window(void)
{
    mvn_window_t *window = mvn_get_window();
    MVN_CHECK_NULL(window, "Cannot maximize window: No window available");

    // Check if window is resizable
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_RESIZABLE)) {
        return mvn_set_error("Cannot maximize window: Window is not resizable");
    }

    if (SDL_MaximizeWindow(window)) {
        return mvn_set_error("Failed to maximize window: %s", SDL_GetError());
    }

    return true;
}

/**
 * \brief           Set window state: minimized, if resizable
 * \return          true on success, false on failure
 */
bool mvn_minimize_window(void)
{
    mvn_window_t *window = mvn_get_window();
    MVN_CHECK_NULL(window, "Cannot minimize window: No window available");

    if (SDL_MinimizeWindow(window)) {
        return mvn_set_error("Failed to minimize window: %s", SDL_GetError());
    }

    return true;
}

/**
 * \brief           Set window state: not minimized/maximized
 * \return          true on success, false on failure
 */
bool mvn_restore_window(void)
{
    mvn_window_t *window = mvn_get_window();
    MVN_CHECK_NULL(window, "Cannot restore window: No window available");

    if (SDL_RestoreWindow(window)) {
        return mvn_set_error("Failed to restore window: %s", SDL_GetError());
    }

    return true;
}

/**
 * \brief           Get current monitor where window is placed
 * \return          ID of the monitor where window is placed, 0 if not found
 */
mvn_display_id_t mvn_get_current_monitor(void)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot get current monitor: No window available");
        return 0;
    }

    mvn_display_id_t displayID = SDL_GetDisplayForWindow(window);
    return displayID;
}

/**
 * \brief           Get specified monitor position
 * \param[in]       monitor: Monitor ID
 * \return          Monitor position as mvn_fpoint_t
 */
mvn_fpoint_t mvn_get_monitor_position(mvn_display_id_t monitor)
{
    mvn_fpoint_t position = {0, 0};

    if (monitor <= 0) {
        mvn_set_error("Invalid monitor ID: %d", monitor);
        return position;
    }

    SDL_Rect bounds;
    if (!SDL_GetDisplayBounds(monitor, &bounds)) {
        mvn_set_error("Failed to get monitor position: %s", SDL_GetError());
        return position;
    }

    position.x = (float)bounds.x;
    position.y = (float)bounds.y;

    return position;
}

/**
 * \brief           Get specified monitor width
 * \param[in]       monitor: Monitor ID
 * \return          Width of the monitor in pixels, 0 on failure
 */
int32_t mvn_get_monitor_width(mvn_display_id_t monitor)
{
    if (monitor <= 0) {
        mvn_set_error("Invalid monitor ID: %d", monitor);
        return 0;
    }

    SDL_Rect bounds;
    if (!SDL_GetDisplayBounds(monitor, &bounds)) {
        mvn_set_error("Failed to get monitor width: %s", SDL_GetError());
        return 0;
    }
    return bounds.w;
}

/**
 * \brief           Get specified monitor height
 * \param[in]       monitor: Monitor ID
 * \return          Height of the monitor in pixels, 0 on failure
 */
int32_t mvn_get_monitor_height(mvn_display_id_t monitor)
{
    if (monitor <= 0) {
        mvn_set_error("Invalid monitor ID: %d", monitor);
        return 0;
    }

    SDL_Rect bounds;
    if (!SDL_GetDisplayBounds(monitor, &bounds)) {
        mvn_set_error("Failed to get monitor height: %s", SDL_GetError());
        return 0;
    }

    return bounds.h;
}

/**
 * \brief           Set icon for window (single image, RGBA 32bit)
 * \param[in]       image: Surface with icon image
 */
void mvn_set_window_icon(mvn_image_t *image)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window icon: No window available");
        return;
    }

    if (image == NULL) {
        mvn_set_error("Cannot set window icon: Invalid image");
        return;
    }

    SDL_SetWindowIcon(window, image);
}

/**
 * \brief           Set title for window
 * \param[in]       title: New title for window
 */
void mvn_set_window_title(const char *title)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window title: No window available");
        return;
    }

    if (title == NULL) {
        mvn_set_error("Cannot set window title: Invalid title");
        return;
    }

    SDL_SetWindowTitle(window, title);
}

/**
 * \brief           Set window position on screen
 * \param[in]       x: New x position
 * \param[in]       y: New y position
 */
void mvn_set_window_position(int32_t x, int32_t y)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window position: No window available");
        return;
    }

    // In SDL3, SDL_SetWindowPosition is asynchronous and may not take effect immediately
    // The actual positioning depends on the window manager and may be constrained
    if (!SDL_SetWindowPosition(window, x, y)) {
        mvn_set_error("Failed to set window position: %s", SDL_GetError());
    }
}

/**
 * \brief           Set monitor for the current window
 * \param[in]       monitor: Monitor ID to move window to
 */
void mvn_set_window_monitor(mvn_display_id_t monitor)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window monitor: No window available");
        return;
    }

    if (monitor == 0) {
        mvn_set_error("Cannot set window monitor: Invalid monitor ID");
        return;
    }

    // Get the monitor's bounds to position the window on it
    SDL_Rect bounds;
    if (!SDL_GetDisplayBounds(monitor, &bounds)) {
        mvn_set_error("Failed to get monitor bounds: %s", SDL_GetError());
        return;
    }

    // Get current window size to maintain it
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);

    // Center the window on the selected monitor
    int x = bounds.x + ((bounds.w - width) / 2);
    int y = bounds.y + ((bounds.h - height) / 2);

    // Position the window on the monitor
    if (!SDL_SetWindowPosition(window, x, y)) {
        mvn_set_error("Failed to set window position: %s", SDL_GetError());
    }
}

/**
 * \brief           Set window minimum dimensions
 * \param[in]       width: Minimum width of the window
 * \param[in]       height: Minimum height of the window
 * \note            Only works with resizable windows (MVN_WINDOW_RESIZABLE)
 */
void mvn_set_window_min_size(int32_t width, int32_t height)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window minimum size: No window available");
        return;
    }

    // Check if window is resizable
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_RESIZABLE)) {
        mvn_log_warn("Setting minimum size on non-resizable window may have no effect");
    }

    SDL_SetWindowMinimumSize(window, width, height);
}

/**
 * \brief           Set window maximum dimensions
 * \param[in]       width: Maximum width of the window
 * \param[in]       height: Maximum height of the window
 * \note            Only works with resizable windows (MVN_WINDOW_RESIZABLE)
 */
void mvn_set_window_max_size(int32_t width, int32_t height)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window maximum size: No window available");
        return;
    }

    // Check if window is resizable
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_RESIZABLE)) {
        mvn_log_warn("Setting maximum size on non-resizable window may have no effect");
    }

    SDL_SetWindowMaximumSize(window, width, height);
}

/**
 * \brief           Set window dimensions
 * \param[in]       width: New width of the window
 * \param[in]       height: New height of the window
 * \note            This is an asynchronous request and may not take effect immediately
 */
void mvn_set_window_size(int32_t width, int32_t height)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window size: No window available");
        return;
    }

    // In SDL3, window size changes are asynchronous
    if (!SDL_SetWindowSize(window, width, height)) {
        mvn_set_error("Failed to set window size: %s", SDL_GetError());
    }
}

/**
 * \brief           Set window opacity
 * \param[in]       opacity: Opacity value (0.0f = transparent, 1.0f = opaque)
 * \return          true on success, false on failure
 * \note            Requires a window with SDL_WINDOW_TRANSPARENT flag
 */
bool mvn_set_window_opacity(float opacity)
{
    mvn_window_t *window = mvn_get_window();
    MVN_CHECK_NULL(window, "Cannot set window opacity: No window available");

    // Clamp opacity value between 0.0 and 1.0
    if (opacity < 0.0f) {
        opacity = 0.0f;
    } else if (opacity > 1.0f) {
        opacity = 1.0f;
    }

    // Check if window has transparency support
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_TRANSPARENT)) {
        mvn_log_warn("Window does not have TRANSPARENT flag, opacity change may not work");
    }

    if (!SDL_SetWindowOpacity(window, opacity)) {
        return mvn_set_error("Failed to set window opacity: %s", SDL_GetError());
    }

    return true;
}

/**
 * \brief           Set window as focused
 */
void mvn_set_window_focused(void)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot set window focus: No window available");
        return;
    }

    SDL_RaiseWindow(window);
}

/**
 * \brief           Get current screen width
 * \return          Width of the current screen in pixels
 */
int32_t mvn_get_screen_width(void)
{
    mvn_display_id_t display = mvn_get_current_monitor();
    if (display == 0) {
        mvn_set_error("Failed to get primary display: %s", SDL_GetError());
        return 0;
    }

    SDL_Rect bounds;
    if (!SDL_GetDisplayBounds(display, &bounds)) {
        mvn_set_error("Failed to get display bounds: %s", SDL_GetError());
        return 0;
    }

    return bounds.w;
}

/**
 * \brief           Get current screen height
 * \return          Height of the current screen in pixels
 */
int32_t mvn_get_screen_height(void)
{
    mvn_display_id_t display = mvn_get_current_monitor();
    if (display == 0) {
        mvn_set_error("Failed to get primary display: %s", SDL_GetError());
        return 0;
    }

    SDL_Rect bounds;
    if (!SDL_GetDisplayBounds(display, &bounds)) {
        mvn_set_error("Failed to get display bounds: %s", SDL_GetError());
        return 0;
    }

    return bounds.h;
}

/**
 * \brief           Get current render width
 * \return          Width of the renderer in pixels (accounts for high DPI)
 */
int32_t mvn_get_render_width(void)
{
    mvn_renderer_t *renderer = mvn_get_renderer();
    if (renderer == NULL) {
        mvn_set_error("Cannot get render width: No renderer available");
        return 0;
    }

    int width;
    int height;
    if (!SDL_GetCurrentRenderOutputSize(renderer, &width, &height)) {
        mvn_set_error("Failed to get render output size: %s", SDL_GetError());
        return 0;
    }

    return width;
}

/**
 * \brief           Get current render height
 * \return          Height of the renderer in pixels (accounts for high DPI)
 */
int32_t mvn_get_render_height(void)
{
    mvn_renderer_t *renderer = mvn_get_renderer();
    if (renderer == NULL) {
        mvn_set_error("Cannot get render height: No renderer available");
        return 0;
    }

    int width;
    int height;
    if (!SDL_GetCurrentRenderOutputSize(renderer, &width, &height)) {
        mvn_set_error("Failed to get render output size: %s", SDL_GetError());
        return 0;
    }

    return height;
}

/**
 * \brief           Get number of connected monitors
 * \return          Number of monitors connected to the system
 */
int32_t mvn_get_monitor_count(void)
{
    int            count    = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&count);

    if (displays == NULL) {
        mvn_set_error("Failed to get displays: %s", SDL_GetError());
        return 0;
    }

    MVN_FREE(displays);
    return count;
}

/**
 * \brief           Get specified monitor refresh rate
 * \param[in]       monitor: Monitor ID
 * \return          Refresh rate of the monitor in Hz, 0 on failure
 */
int32_t mvn_get_monitor_refresh_rate(mvn_display_id_t monitor)
{
    if (monitor == 0) {
        mvn_set_error("Cannot get monitor refresh rate: Invalid monitor ID");
        return 0;
    }

    // Get the current display mode
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(monitor);
    if (mode == NULL) {
        mvn_set_error("Failed to get current display mode: %s", SDL_GetError());
        return 0;
    }

    // Return the refresh rate as an integer (truncating decimals if any)
    return (int32_t)mode->refresh_rate;
}

/**
 * \brief           Get window position on monitor
 * \return          Window position as mvn_fpoint_t
 */
mvn_fpoint_t mvn_get_window_position(void)
{
    mvn_fpoint_t  position = {0.0f, 0.0f};
    mvn_window_t *window   = mvn_get_window();

    if (window == NULL) {
        mvn_set_error("Cannot get window position: No window available");
        return position;
    }

    int x;
    int y;
    SDL_GetWindowPosition(window, &x, &y);
    position.x = (float)x;
    position.y = (float)y;

    return position;
}

/**
 * \brief           Get window scale DPI factor
 * \return          Window scale DPI factor as mvn_fpoint_t
 */
mvn_fpoint_t mvn_get_window_scale_dpi(void)
{
    mvn_fpoint_t  scale  = {1.0f, 1.0f};
    mvn_window_t *window = mvn_get_window();

    if (window == NULL) {
        mvn_set_error("Cannot get window scale DPI: No window available");
        return scale;
    }

    // SDL3 provides a single scale factor that applies to both dimensions
    float factor = SDL_GetWindowDisplayScale(window);
    scale.x      = factor;
    scale.y      = factor;

    return scale;
}

/**
 * \brief           Get the human-readable, UTF-8 encoded name of the specified monitor
 * \param[in]       monitor: Monitor ID
 * \return          Name of the monitor, NULL on failure
 */
mvn_string_t *mvn_get_monitor_name(mvn_display_id_t monitor)
{
    if (monitor == 0) {
        mvn_set_error("Cannot get monitor name: Invalid monitor ID");
        return NULL;
    }

    const char *name = SDL_GetDisplayName(monitor);
    if (name == NULL) {
        mvn_set_error("Failed to get monitor name: %s", SDL_GetError());
        return NULL;
    }

    return mvn_string_from_cstr(name);
}

/**
 * \brief           Show the cursor
 */
void mvn_show_cursor(void)
{
    SDL_ShowCursor();
}

/**
 * \brief           Hide the cursor
 */
void mvn_hide_cursor(void)
{
    SDL_HideCursor();
}

/**
 * \brief           Check if cursor is hidden
 * \return          true if cursor is hidden, false otherwise
 */
bool mvn_is_cursor_hidden(void)
{
    return !SDL_CursorVisible();
}

/**
 * \brief           Enable cursor (unlock cursor)
 */
void mvn_enable_cursor(void)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot enable cursor: No window available");
        return;
    }

    if (SDL_SetWindowRelativeMouseMode(window, false)) {
        mvn_set_error("Failed to enable cursor: %s", SDL_GetError());
    }
}

/**
 * \brief           Disable cursor (lock cursor)
 */
void mvn_disable_cursor(void)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot disable cursor: No window available");
        return;
    }

    if (SDL_SetWindowRelativeMouseMode(window, true)) {
        mvn_set_error("Failed to disable cursor: %s", SDL_GetError());
    }
}

/**
 * \brief           Check if cursor is on the screen
 * \return          true if cursor is on screen, false otherwise
 */
bool mvn_is_cursor_on_screen(void)
{
    mvn_window_t *window = mvn_get_window();
    if (window == NULL) {
        mvn_set_error("Cannot check if cursor is on screen: No window available");
        return false;
    }

    int   window_w;
    int   window_h;
    float mouse_x;
    float mouse_y;

    // Get window size in pixels
    if (SDL_GetWindowSizeInPixels(window, &window_w, &window_h)) {
        mvn_set_error("Failed to get window size: %s", SDL_GetError());
        return false;
    }

    // Get mouse position
    SDL_GetMouseState(&mouse_x, &mouse_y);

    // Check if cursor is within window boundaries
    return (mouse_x >= 0 && mouse_x < (float)window_w && mouse_y >= 0 && mouse_y < (float)window_h);
}
