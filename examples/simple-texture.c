/**
 * \file            empty.c
 * \brief           Basic example showing MVN greeting functionality
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

#include "mvn/mvn.h" // IWYU pragma: keep

/**
 * \brief           Main application entry point
 */
int
main(void) {
    /* Initialize SDL */
    if (!mvn_init(800, 600, "MVN Simple Texture Example", 0)) {
        return 1;
    }

    mvn_logger_init();

    /* Display Engine Version */
    mvn_string_t* version = mvn_get_engine_version();
    mvn_log_info("MVN Engine Version: %s", mvn_string_to_cstr(version));

    /* Construct the file path using assets_dir */
    char filepath[512];
    SDL_snprintf(filepath, sizeof(filepath), "%s/%s", ASSET_DIR, "char-1.png");

    // Load the char-1 image as a texture
    mvn_texture_t* char_tex1 = mvn_load_texture(mvn_get_renderer(), filepath);
    if (!char_tex1) {
        mvn_log_error("Failed to load texture: %s", filepath);
        // Consider cleanup and returning 1 here or handling the missing texture in the draw loop
    }

    SDL_snprintf(filepath, sizeof(filepath), "%s/%s", ASSET_DIR, "char-2.png");
    mvn_texture_t* char_tex2 = mvn_load_texture(mvn_get_renderer(), filepath);
    if (!char_tex2) {
        mvn_log_error("Failed to load texture: %s", filepath);
        // Handle error
    }

    SDL_snprintf(filepath, sizeof(filepath), "%s/%s", ASSET_DIR, "char-3.png");
    mvn_texture_t* char_tex3 = mvn_load_texture(mvn_get_renderer(), filepath);
    if (!char_tex3) {
        mvn_log_error("Failed to load texture: %s", filepath);
        // Handle error
    }

    while (!mvn_window_should_close()) {
        // Begin drawing
        mvn_begin_drawing();

        // Clear background with white color
        mvn_clear_background(MVN_BLACK);

        // Draw the character images only if they loaded successfully
        if (char_tex1) {
            mvn_draw_texture(char_tex1, 0, 0, MVN_WHITE);
            mvn_draw_texture_rec(char_tex1,
                                 MVN_STRUCT(mvn_frect_t, {.x = 0, .y = 0, .w = 50, .h = 50}),
                                 MVN_STRUCT(mvn_fpoint_t, {.x = 300, .y = 300}), MVN_WHITE);
        }
        if (char_tex2) {
            mvn_draw_texture_v(char_tex2, MVN_STRUCT(mvn_fpoint_t, {.x = 100, .y = 100}),
                               MVN_WHITE);
            mvn_draw_texture_pro(char_tex2,
                                 MVN_STRUCT(mvn_frect_t, {.x = 0, .y = 0, .w = 50, .h = 50}),
                                 MVN_STRUCT(mvn_frect_t, {.x = 500, .y = 500, .w = 100, .h = 100}),
                                 MVN_STRUCT(mvn_fpoint_t, {.x = 50, .y = 50}), 45.0f, MVN_WHITE);
        }
        if (char_tex3) {
            mvn_draw_texture_ex(char_tex3, MVN_STRUCT(mvn_fpoint_t, {.x = 200, .y = 200}), 45.0f,
                                2.0f, MVN_WHITE);
        }

        // End drawing and present
        mvn_end_drawing();
    }

    // Clean up the texture before closing (check for NULL before unloading)
    if (char_tex1) {
        mvn_unload_texture(char_tex1);
    }
    if (char_tex2) {
        mvn_unload_texture(char_tex2);
    }
    if (char_tex3) {
        mvn_unload_texture(char_tex3);
    }

    // Clean up all resources
    mvn_string_free(version);
    mvn_quit();
    return 0;
}
