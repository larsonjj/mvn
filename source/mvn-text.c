/**
 * \file            mvn-text.c
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

#include "mvn/mvn-text.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "mvn/mvn-core.h"
#include "mvn/mvn-logger.h"
#include "mvn/mvn-types.h"

/* Private variables */
static int32_t mvn_line_spacing = 0;

/**
 * \brief           Load a font from the assets directory
 * \param[in]       fileName: Name of the font file
 * \param[in]       size: Size of the font in points
 * \return          Font handle on success, NULL on failure
 */
TTF_Font*
mvn_load_font(const char* fileName, float size) {
    char path[512];
    TTF_Font* font = NULL;

    // Construct path to the font file
    SDL_snprintf(path, sizeof(path), "%s", fileName);

    // Load font with the specified size
    font = TTF_OpenFont(path, size);
    if (font == NULL) {
        mvn_log_error("Failed to load font: %s - %s", path, SDL_GetError());
    }

    return font;
}

/**
 * \brief           Load a font from the assets directory with preloaded codepoints
 * \param[in]       fileName: Name of the font file
 * \param[in]       size: Size of the font in points
 * \param[in]       codePoints: Array of codepoints to preload
 * \param[in]       codePointCount: Number of codepoints in the array
 * \return          Font handle on success, NULL on failure
 */
TTF_Font*
mvn_load_font_ex(const char* fileName, float size, const int* codePoints, int codePointCount) {
    char path[512];
    TTF_Font* font = NULL;

    // Construct path to the font file
    SDL_snprintf(path, sizeof(path), "%s", fileName);

    // Load font with the specified size
    font = TTF_OpenFont(path, size);
    if (font == NULL) {
        mvn_log_error("Failed to load font: %s - %s", path, SDL_GetError());
        return NULL;
    }

    // Preload specified codepoints if provided
    if (codePoints != NULL && codePointCount > 0) {
        for (int i = 0; i < codePointCount; i++) {
            // Ensure the glyph is loaded - this will cache it
            if (!TTF_FontHasGlyph(font, codePoints[i])) {
                mvn_log_warn("Codepoint %d not available in font %s", codePoints[i], fileName);
            }
        }
    }

    return font;
}

/**
 * \brief           Unload a font previously loaded with mvn_load_font
 * \param[in]       font: Font to unload
 */
void
mvn_unload_font(TTF_Font* font) {
    if (font != NULL) {
        TTF_CloseFont(font);
    }
}

/**
 * \brief           Set vertical line spacing when drawing text with line-breaks
 * \param[in]       spacing: Spacing value in pixels
 */
void
mvn_set_text_line_spacing(int32_t spacing) {
    mvn_line_spacing = spacing;
}

/**
 * \brief           Measure string size for a font
 * \param[in]       font: Font to use for measurement
 * \param[in]       text: Text to measure
 * \param[in]       spacing: Character spacing
 * \return          Width of the measured text in pixels
 */
int32_t
mvn_measure_text(TTF_Font* font, const char* text, float spacing) {
    size_t length = 0;
    int32_t max_width = 0; // Unbounded
    size_t measured_length;
    int32_t measured_width;

    if (font == NULL || text == NULL) {
        return 0;
    }

    /* TTF_Font already has size information from when it was loaded */

    /* Measure the text size */
    if (!TTF_MeasureString(font, text, length, max_width, &measured_width, &measured_length)) {
        mvn_log_error("Failed to measure text: %s", SDL_GetError());
        return 0;
    }

    /* Add additional spacing between characters */
    if (spacing != 0 && text[0] != '\0') {
        measured_width += (int32_t)((float)(SDL_strlen(text) - 1) * spacing);
    }

    return measured_width;
}

/**
 * \brief           Draw text using font and additional parameters
 * \param[in]       font: Font to use for drawing
 * \param[in]       text: Text to draw
 * \param[in]       position: Position (as mvn_fpoint_t) to draw text
 * \param[in]       tint: Color tint to apply to the text
 */
void
mvn_draw_text(TTF_Font* font, const char* text, mvn_fpoint_t position, mvn_color_t tint) {
    TTF_Text* text_obj;
    mvn_text_engine_t* text_engine;

    if (text == NULL || text[0] == '\0' || font == NULL) {
        return;
    }

    /* Get current text engine */
    text_engine = mvn_get_text_engine();
    if (text_engine == NULL) {
        mvn_log_error("No active text engine for text drawing");
        return;
    }

    /* Create text object */
    text_obj = TTF_CreateText(text_engine, font, text, 0);
    if (text_obj == NULL) {
        mvn_log_error("Failed to create text: %s", SDL_GetError());
        return;
    }

    /* Apply text settings */
    TTF_SetTextColorFloat(text_obj, (float)tint.r / 255.0f, (float)tint.g / 255.0f,
                          (float)tint.b / 255.0f, (float)tint.a / 255.0f);

    /* Draw the text */
    if (!TTF_DrawRendererText(text_obj, position.x, position.y)) {
        mvn_log_error("Failed to draw text: %s", SDL_GetError());
    }

    /* Clean up */
    TTF_DestroyText(text_obj);
}

/**
 * \brief           Draw text using font and pro parameters (rotation)
 * \param[in]       font: Font to use for drawing
 * \param[in]       text: Text to draw
 * \param[in]       position: Position (as mvn_fpoint_t) to draw text
 * \param[in]       origin: Origin of rotation/scaling (relative to text)
 * \param[in]       rotation: Rotation in degrees
 * \param[in]       tint: Color tint to apply to the text
 */
void
mvn_draw_text_pro(TTF_Font* font, const char* text, mvn_fpoint_t position, mvn_fpoint_t origin,
                  float rotation, mvn_color_t tint) {
    mvn_image_t* surface;
    mvn_texture_t* texture;
    mvn_renderer_t* renderer;
    mvn_frect_t dest;
    mvn_fpoint_t center;

    if (text == NULL || text[0] == '\0' || font == NULL) {
        return;
    }

    /* Get current renderer */
    renderer = mvn_get_renderer();
    if (renderer == NULL) {
        mvn_log_error("No active renderer for text drawing");
        return;
    }

    /* Render text to surface */
    surface = TTF_RenderText_Blended(font, text, 0, tint);
    if (surface == NULL) {
        mvn_log_error("Failed to render text: %s", SDL_GetError());
        return;
    }

    /* Create texture from surface */
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (texture == NULL) {
        mvn_log_error("Failed to create texture from text: %s", SDL_GetError());
        return;
    }

    /* Set destination rectangle */
    float width;
    float height;
    SDL_GetTextureSize(texture, &width, &height);
    dest.w = width;
    dest.h = height;
    dest.x = position.x;
    dest.y = position.y;

    /* Calculate rotation center */
    center.x = origin.x;
    center.y = origin.y;

    /* Draw the texture with rotation */
    SDL_RenderTextureRotated(renderer, texture, NULL, &dest, rotation, &center, SDL_FLIP_NONE);

    /* Clean up */
    SDL_DestroyTexture(texture);
}
