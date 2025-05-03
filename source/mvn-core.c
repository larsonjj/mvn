/**
 * \file            mvn-core.c
 * \brief           MVN core framework implementation
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

#include "mvn/mvn-core.h"

#include "mvn/mvn-error.h" // Added error module
#include "mvn/mvn-file.h"  // IWYU pragma: keep
#include "mvn/mvn-logger.h"
#include "mvn/mvn-string.h"
#include "mvn/mvn-types.h"
#include "mvn/mvn-utils.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/* Static variables to hold the window and renderer */
static mvn_window_t      *g_window      = NULL;
static mvn_renderer_t    *g_renderer    = NULL;
static mvn_text_engine_t *g_text_engine = NULL;

/* Static variables for timing */
static uint64_t g_performance_frequency = 0;   // Frequency of the performance counter
static uint64_t g_start_time            = 0;   // Time point when mvn_init was called
static uint64_t g_last_frame_time       = 0;   // Time point at the beginning of the last frame
static uint64_t g_current_frame_time    = 0;   // Time point at the end of the current frame
static double   g_delta_time            = 0.0; // Time elapsed for the last frame in seconds
static int      g_target_fps            = 300; // Target FPS (default 300)
static double   g_target_frame_time     = 0.0; // Minimum time per frame in seconds
static int      g_frame_counter         = 0;   // Frames counted in the current second
static uint64_t g_fps_timer             = 0;   // Timer to track 1 second for FPS calculation
static int      g_current_fps           = 0;   // Calculated FPS for the last second

/**
 * \brief           Get the current version of the MVN engine
 * \return          Pointer to string containing version info, NULL on error
 */
mvn_string_t *mvn_get_engine_version(void)
{
    return mvn_string_from_cstr("0.1.0");
}

/**
 * \brief           Initialize the MVN framework
 * \param[in]       width: Width of the window to create
 * \param[in]       height: Height of the window to create
 * \param[in]       title: Title for the window
 * \param[in]       flags: SDL window flags, use 0 for default
 * \return          true on success, false on failure
 */
bool mvn_init(int width, int height, const char *title, mvn_window_flags_t flags)
{
    // Initialize SDL first - needed for video and other features
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return mvn_set_error("SDL initialization failed: %s", SDL_GetError());
    }

    // Add default high DPI flag if no flags provided
    if (flags == 0) {
        flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
    } else if (!(flags & SDL_WINDOW_HIGH_PIXEL_DENSITY)) {
        // Add high DPI flag if not explicitly disabled
        flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
    }

    // Create the window with specified flags
    g_window = SDL_CreateWindow(title, width, height, flags);
    if (!g_window) {
        SDL_Quit();
        return mvn_set_error("Window creation failed: %s", SDL_GetError());
    }

    // Create renderer for window
    g_renderer = SDL_CreateRenderer(g_window, NULL);
    if (!g_renderer) {
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return mvn_set_error("Renderer creation failed: %s", SDL_GetError());
    }

    // Initialize SDL_ttf
    if (!TTF_WasInit() && !TTF_Init()) {
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return mvn_set_error("Failed to initialize SDL_ttf: %s", SDL_GetError());
    }

    // Create the renderer text engine
    g_text_engine = TTF_CreateRendererTextEngine(mvn_get_renderer());
    if (!g_text_engine) {
        TTF_Quit();
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return mvn_set_error("Failed to create renderer text engine: %s", SDL_GetError());
    }

    // Initialize timing variables
    g_performance_frequency = SDL_GetPerformanceFrequency();
    g_start_time            = SDL_GetPerformanceCounter();
    g_last_frame_time       = g_start_time;
    g_current_frame_time    = g_start_time;
    g_fps_timer             = g_start_time;
    mvn_set_target_fps(300); // Set default target FPS

    return true;
}

/**
 * \brief           Clean up the MVN framework resources
 */
void mvn_quit(void)
{
    // Clean up in reverse order of creation
    if (g_renderer != NULL) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = NULL;
    }

    // Clean up the window
    if (g_window != NULL) {
        SDL_DestroyWindow(g_window);
        g_window = NULL;
    }

    // Clean up text engine
    if (g_text_engine != NULL) {
        TTF_DestroyRendererTextEngine(g_text_engine);
        g_text_engine = NULL;
    }

    // Quit SDL_ttf
    TTF_Quit();

    // Quit SDL
    SDL_Quit();
}

/**
 * \brief           Get the SDL window
 * \return          Pointer to the SDL window, NULL if not initialized
 */
mvn_window_t *mvn_get_window(void)
{
    return g_window;
}

/**
 * \brief           Get the SDL renderer
 * \return          Pointer to the SDL renderer, NULL if not initialized
 */
mvn_renderer_t *mvn_get_renderer(void)
{
    return g_renderer;
}

/**
 * \brief           Get the SDL_ttf text engine
 * \return          Pointer to the SDL_ttf text engine, NULL if not initialized
 */
mvn_text_engine_t *mvn_get_text_engine(void)
{
    return g_text_engine;
}

/**
 * \brief           Check if the window should close
 * \return          true if window should close, false otherwise
 */
bool mvn_window_should_close(void)
{
    SDL_Event event;
    bool      should_close = false;

    /* Process all pending events */
    while (SDL_PollEvent(&event)) {
        /* Check for quit events */
        if (event.type == SDL_EVENT_QUIT) {
            should_close = true;
            break;
        }

        /* Check for ESC key press */
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            should_close = true;
            break;
        }
    }

    return should_close;
}

/**
 * \brief           Begin drawing to the window and update frame timing
 * \return          true if successful, false on failure
 */
bool mvn_begin_drawing(void)
{
    MVN_CHECK_NULL(g_renderer, "Cannot begin drawing: Renderer not initialized");

    // Calculate delta time from the previous frame
    uint64_t frame_start_time = SDL_GetPerformanceCounter();
    g_delta_time = (double)(frame_start_time - g_last_frame_time) / (double)g_performance_frequency;
    g_last_frame_time = frame_start_time; // Update last frame time for the next frame

    // No longer clearing automatically - user should call mvn_clear_background
    return true;
}

/**
 * \brief           Clear the background with specified color
 * \param[in]       color: Color to clear the background with
 * \return          true if successful, false on failure
 */
bool mvn_clear_background(mvn_color_t color)
{
    MVN_CHECK_NULL(g_renderer, "Cannot clear background: Renderer not initialized");

    // Set the renderer draw color
    if (!SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a)) {
        return mvn_set_error("Failed to set render color: %s", SDL_GetError());
    }

    // Clear the entire renderer with the set color
    if (!SDL_RenderClear(g_renderer)) {
        return mvn_set_error("Failed to clear renderer: %s", SDL_GetError());
    }

    return true;
}

/**
 * \brief           End drawing, present the rendered content, and manage frame timing/FPS
 * \return          true if successful, false on failure
 */
bool mvn_end_drawing(void)
{
    MVN_CHECK_NULL(g_renderer, "Cannot end drawing: Renderer not initialized");

    // Present the renderer contents to the screen
    SDL_RenderPresent(g_renderer);

    // --- Accurate Frame Limiting ---
    uint64_t frame_end_time = SDL_GetPerformanceCounter();
    double   elapsed_frame_time_seconds =
        (double)(frame_end_time - g_last_frame_time) / (double)g_performance_frequency;

    if (g_target_fps > 0 && elapsed_frame_time_seconds < g_target_frame_time) {
        double time_to_wait_seconds = g_target_frame_time - elapsed_frame_time_seconds;

        // Use SDL_Delay for most of the wait time, leave ~1.5ms for busy-wait
        const double busy_wait_threshold_seconds = 0.0015;
        if (time_to_wait_seconds > busy_wait_threshold_seconds) {
            uint32_t delay_ms =
                (uint32_t)((time_to_wait_seconds - busy_wait_threshold_seconds) * 1000.0);
            if (delay_ms > 0) {
                SDL_Delay(delay_ms);
            }
        }

        // Busy-wait for the remaining time for precision
        uint64_t target_ticks =
            g_last_frame_time + (uint64_t)(g_target_frame_time * (double)g_performance_frequency);
        while (SDL_GetPerformanceCounter() < target_ticks) {
            // Spin-lock briefly to free up CPU resources
        }
        g_current_frame_time = SDL_GetPerformanceCounter(); // Update after waiting
    } else {
        // Frame took too long, no wait needed
        g_current_frame_time = frame_end_time;
    }
    // --- End Accurate Frame Limiting ---

    // FPS calculation (uses the final current_frame_time)
    g_frame_counter++;
    double time_since_fps_reset =
        (double)(g_current_frame_time - g_fps_timer) / (double)g_performance_frequency;

    if (time_since_fps_reset >= 1.0) {
        g_current_fps   = g_frame_counter;
        g_frame_counter = 0;
        g_fps_timer     = g_current_frame_time; // Reset timer for the next second
    }

    return true;
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
    if (SDL_GetDisplayBounds((SDL_DisplayID)monitor, &bounds) != 0) {
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
    if (SDL_GetDisplayBounds((SDL_DisplayID)monitor, &bounds) != 0) {
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
    if (SDL_GetDisplayBounds((SDL_DisplayID)monitor, &bounds) != 0) {
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
    if (SDL_SetWindowPosition(window, x, y) != 0) {
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
    if (SDL_GetDisplayBounds(monitor, &bounds) != 0) {
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
    if (SDL_SetWindowPosition(window, x, y) != 0) {
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
    if (SDL_SetWindowSize(window, width, height) != 0) {
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
    mvn_display_id_t display = SDL_GetPrimaryDisplay();
    if (display == 0) {
        mvn_set_error("Failed to get primary display: %s", SDL_GetError());
        return 0;
    }

    SDL_Rect bounds;
    if (SDL_GetDisplayBounds(display, &bounds) != 0) {
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
    mvn_display_id_t display = SDL_GetPrimaryDisplay();
    if (display == 0) {
        mvn_set_error("Failed to get primary display: %s", SDL_GetError());
        return 0;
    }

    SDL_Rect bounds;
    if (SDL_GetDisplayBounds(display, &bounds) != 0) {
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

/**
 * \brief           Set target FPS (maximum frame rate)
 * \param[in]       fps: Target frames per second (0 or negative means uncapped)
 */
void mvn_set_target_fps(int fps)
{
    g_target_fps = fps;
    if (g_target_fps <= 0) {
        g_target_frame_time = 0.0; // Uncapped
    } else {
        g_target_frame_time = 1.0 / (double)g_target_fps;
    }
}

/**
 * \brief           Get time in seconds for the last frame drawn (delta time)
 * \return          Time elapsed for the last frame in seconds
 */
float mvn_get_frame_time(void)
{
    return (float)g_delta_time;
}

/**
 * \brief           Get elapsed time in seconds since mvn_init() was called
 * \return          Total elapsed time in seconds
 */
double mvn_get_time(void)
{
    if (g_performance_frequency == 0) {
        return 0.0; // Avoid division by zero if not initialized
    }
    uint64_t current_time = SDL_GetPerformanceCounter();
    return (double)(current_time - g_start_time) / (double)g_performance_frequency;
}

/**
 * \brief           Get current FPS (frames per second)
 * \return          Current calculated FPS
 */
int mvn_get_fps(void)
{
    return g_current_fps;
}
