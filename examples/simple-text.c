/**
 * \file            simple-text.c
 * \brief           Example demonstrating all MVN text drawing functions
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

int main(void)
{
    /* Initialize MVN */
    if (!mvn_init(800, 600, "MVN Simple Text Example", 0)) {
        return 1;
    }

    /* Load fonts at different sizes */
    TTF_Font *fontSmall  = mvn_load_font("assets/press_start_2p.ttf", 16);
    TTF_Font *fontMedium = mvn_load_font("assets/press_start_2p.ttf", 20);
    TTF_Font *fontLarge  = mvn_load_font("assets/press_start_2p.ttf", 24);

    if (fontSmall == NULL || fontMedium == NULL || fontLarge == NULL) {
        mvn_log_error("Failed to load fonts");
        mvn_quit();
        return 1;
    }

    /* For rotation animation */
    float rotation = 0.0f;

    /* Main game loop */
    while (!mvn_window_should_close()) {
        /* Update animation */
        rotation += 0.1f;
        if (rotation > 360.0f) {
            rotation = 0.0f;
        }

        /* Begin drawing */
        mvn_begin_drawing();
        mvn_clear_background(MVN_BLACK);

        /* Draw section titles */
        mvn_draw_text(fontMedium,
                      "MVN Text Rendering Showcase",
                      MVN_STRUCT(mvn_fpoint_t, {20, 20}),
                      MVN_YELLOW);

        mvn_draw_text(
            fontSmall, "1. Basic Text Drawing", MVN_STRUCT(mvn_fpoint_t, {20, 60}), MVN_WHITE);

        /* 1. Basic text drawing */
        mvn_draw_text(
            fontSmall, "Small Font (12pt)", MVN_STRUCT(mvn_fpoint_t, {40, 120}), MVN_WHITE);
        mvn_draw_text(
            fontMedium, "Medium Font (16pt)", MVN_STRUCT(mvn_fpoint_t, {40, 150}), MVN_WHITE);
        mvn_draw_text(
            fontLarge, "Large Font (24pt)", MVN_STRUCT(mvn_fpoint_t, {40, 180}), MVN_WHITE);

        /* 2. Text with colored tints */
        mvn_draw_text(fontSmall,
                      "2. Text with Different Colors",
                      MVN_STRUCT(mvn_fpoint_t, {20, 220}),
                      MVN_WHITE);
        mvn_draw_text(fontMedium, "Red Text", MVN_STRUCT(mvn_fpoint_t, {40, 250}), MVN_RED);
        mvn_draw_text(fontMedium, "Green Text", MVN_STRUCT(mvn_fpoint_t, {40, 280}), MVN_GREEN);
        mvn_draw_text(fontMedium, "Blue Text", MVN_STRUCT(mvn_fpoint_t, {40, 310}), MVN_BLUE);

        /* 4. Text with rotation (mvn_draw_text_pro) */
        mvn_draw_text(fontSmall, "4. Rotated Text", MVN_STRUCT(mvn_fpoint_t, {400, 60}), MVN_WHITE);

        /* Static rotated examples */
        mvn_draw_text_pro(fontMedium,
                          "Rotated 15°",
                          MVN_STRUCT(mvn_fpoint_t, {450, 100}),
                          MVN_STRUCT(mvn_fpoint_t, {0, 0}),
                          15.0f,
                          MVN_WHITE);

        mvn_draw_text_pro(fontMedium,
                          "Rotated 45°",
                          MVN_STRUCT(mvn_fpoint_t, {500, 150}),
                          MVN_STRUCT(mvn_fpoint_t, {0, 0}),
                          45.0f,
                          MVN_WHITE);

        /* Animated rotation with custom origin */
        int width = mvn_measure_text(fontLarge, "Rotating Text", 0);
        mvn_draw_text_pro(fontLarge,
                          "Rotating Text",
                          MVN_STRUCT(mvn_fpoint_t, {550.0f, 250.0f}),
                          MVN_STRUCT(mvn_fpoint_t, {(float)width / 2.0f, 12.0f}),
                          rotation,
                          MVN_YELLOW);

        /* 5. Text measurements */
        mvn_draw_text(
            fontSmall, "5. Text Measurements", MVN_STRUCT(mvn_fpoint_t, {400, 350}), MVN_WHITE);

        const char *measureText = "Measured text width";
        int         textWidth   = mvn_measure_text(fontMedium, measureText, 0);
        mvn_draw_text(fontMedium, measureText, MVN_STRUCT(mvn_fpoint_t, {420, 380}), MVN_WHITE);

        /* Draw a line showing the text width */
        mvn_renderer_t *renderer = mvn_get_renderer();
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderLine(renderer, 420.0f, 400.0f, 420.0f + (float)textWidth, 400.0f);

        /* Show the measurement */
        char widthStr[32];
        SDL_snprintf(widthStr, sizeof(widthStr), "Width: %d pixels", textWidth);
        mvn_draw_text(fontSmall, widthStr, MVN_STRUCT(mvn_fpoint_t, {420, 410}), MVN_YELLOW);

        /* Draw FPS counter */
        int  currentFps = mvn_get_fps();
        char fpsText[16];
        SDL_snprintf(fpsText, sizeof(fpsText), "FPS: %d", currentFps);

        int   screenWidth  = mvn_get_render_width();
        int   screenHeight = mvn_get_render_height();
        int   fpsTextWidth = mvn_measure_text(fontSmall, fpsText, 0);
        float margin       = 32.0f;

        mvn_fpoint_t fpsPos = MVN_STRUCT(mvn_fpoint_t,
                                         {
                                             .x = (float)screenWidth - (float)fpsTextWidth - margin,
                                             .y = (float)screenHeight - margin,
                                         });
        mvn_draw_text(fontSmall, fpsText, fpsPos, MVN_WHITE);

        /* End drawing */
        mvn_end_drawing();
    }

    /* Clean up resources */
    mvn_unload_font(fontSmall);
    mvn_unload_font(fontMedium);
    mvn_unload_font(fontLarge);

    mvn_quit();

    return 0;
}
