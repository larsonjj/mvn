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
#include "mvn/mvn-window.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/* Static variables to hold the window and renderer */
mvn_window_t             *g_window      = NULL;
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
