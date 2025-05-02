/**
 * \file            mvn-texture.c
 * \brief           MVN texture loading and management implementation
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

#include "mvn/mvn-texture.h"

#include "mvn/mvn-logger.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>


/**
 * \brief           Load an image from the assets directory
 * \param[in]       filename: Name of the image file in assets/images/ directory
 * \return          Surface with loaded image, NULL on failure
 */
mvn_image_t *mvn_load_image(const char *filename)
{
    char         path[512];
    mvn_image_t *surface = NULL;

    // Construct path to the asset file
    SDL_snprintf(path, sizeof(path), "%s", filename);

    // Load image directly using SDL_image
    surface = IMG_Load(path);

    if (!surface) {
        mvn_log_error("Failed to load image: %s - %s", path, SDL_GetError());
    }

    return surface;
}

/**
 * \brief           Unload an image surface
 * \param[in]       surface: Surface to be unloaded
 */
void mvn_unload_image(mvn_image_t *surface)
{
    if (surface != NULL) {
        SDL_DestroySurface(surface);
    }
}

/**
 * \brief           Convert mvn_image_t to mvn_texture_t
 * \param[in]       renderer: SDL renderer to create texture with
 * \param[in]       surface: Surface to convert to texture
 * \return          Texture created from surface, NULL on failure
 */
mvn_texture_t *mvn_image_to_texture(mvn_renderer_t *renderer, mvn_image_t *surface)
{
    mvn_texture_t *texture = NULL;

    // Validate parameters
    if (renderer == NULL || surface == NULL) {
        mvn_log_error("Invalid parameters for image to texture conversion");
        return NULL;
    }

    // Create texture from the surface
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        mvn_log_error("Failed to create texture from surface: %s", SDL_GetError());
        return NULL;
    }

    return texture;
}

/**
 * \brief           Load an image and convert directly to texture
 * \param[in]       renderer: SDL renderer to create texture with
 * \param[in]       filename: Name of the image file to load
 * \return          Texture created from loaded image, NULL on failure
 */
mvn_texture_t *mvn_load_texture(mvn_renderer_t *renderer, const char *filename)
{
    mvn_image_t *  surface = NULL;
    mvn_texture_t *texture = NULL;

    // Load the image first
    surface = mvn_load_image(filename);
    if (!surface) {
        // Error already logged in mvn_load_image
        return NULL;
    }

    // Convert to texture
    texture = mvn_image_to_texture(renderer, surface);

    if (!texture) {
        // Error already logged in mvn_image_to_texture
        mvn_unload_image(surface);
        return NULL;
    }

    // Set texture scale mode to nearest for pixel art
    // This is important for pixel art to avoid smoothing
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    /* Free the surface as it's no longer needed */
    mvn_unload_image(surface);

    return texture;
}

/**
 * \brief           Unload a texture
 * \param[in]       texture: Texture to be unloaded
 */
void mvn_unload_texture(mvn_texture_t *texture)
{
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
}

/**
 * \brief           Draw a texture with position and tint
 * \param[in]       texture: Texture to be drawn
 * \param[in]       posX: X position to draw texture
 * \param[in]       posY: Y position to draw texture
 * \param[in]       tint: Color tint to apply to the texture
 */
void mvn_draw_texture(mvn_texture_t *texture, int32_t posX, int32_t posY, mvn_color_t tint)
{
    if (texture == NULL) {
        return;
    }

    float           width;
    float           height;
    mvn_renderer_t *renderer;

    // Get the texture's renderer and dimensions
    if (SDL_GetTextureSize(texture, &width, &height)) {
        renderer = SDL_GetRendererFromTexture(texture);

        // Apply the tint color
        SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
        SDL_SetTextureAlphaMod(texture, tint.a);

        // Define the destination rectangle
        mvn_frect_t dest = {.x = (float)posX, .y = (float)posY, .w = width, .h = height};

        // Render the texture
        SDL_RenderTexture(renderer, texture, NULL, &dest);
    }
}

/**
 * \brief           Draw a texture with vector position and tint
 * \param[in]       texture: Texture to be drawn
 * \param[in]       position: Position (as mvn_fpoint_t) to draw texture
 * \param[in]       tint: Color tint to apply to the texture
 */
void mvn_draw_texture_v(mvn_texture_t *texture, mvn_fpoint_t position, mvn_color_t tint)
{
    mvn_draw_texture(texture, (int)position.x, (int)position.y, tint);
}

/**
 * \brief           Draw a texture with rotation, scale and tint
 * \param[in]       texture: Texture to be drawn
 * \param[in]       position: Position (as mvn_fpoint_t) to draw texture
 * \param[in]       rotation: Rotation in degrees
 * \param[in]       scale: Scale factor (1.0f for default)
 * \param[in]       tint: Color tint to apply to the texture
 */
void mvn_draw_texture_ex(mvn_texture_t *texture,
                         mvn_fpoint_t   position,
                         float          rotation,
                         float          scale,
                         mvn_color_t    tint)
{
    if (texture == NULL) {
        return;
    }

    float           width;
    float           height;
    mvn_renderer_t *renderer;

    // Get the texture's renderer and dimensions
    if (SDL_GetTextureSize(texture, &width, &height)) {
        renderer = SDL_GetRendererFromTexture(texture);

        // Apply the tint color
        SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
        SDL_SetTextureAlphaMod(texture, tint.a);

        // Define the destination rectangle with scaling
        mvn_frect_t dest = {
            .x = position.x, .y = position.y, .w = width * scale, .h = height * scale};

        // Create center point for rotation (center of the texture)
        mvn_fpoint_t center = {.x = dest.w / 2.0f, .y = dest.h / 2.0f};

        // Render the texture with rotation and scaling
        SDL_RenderTextureRotated(renderer, texture, NULL, &dest, rotation, &center, SDL_FLIP_NONE);
    }
}

/**
 * \brief           Draw a portion of a texture
 * \param[in]       texture: Texture to be drawn
 * \param[in]       source: Source rectangle defining the portion to draw
 * \param[in]       position: Position (as mvn_fpoint_t) to draw texture
 * \param[in]       tint: Color tint to apply to the texture
 */
void mvn_draw_texture_rec(mvn_texture_t *texture,
                          mvn_frect_t    source,
                          mvn_fpoint_t   position,
                          mvn_color_t    tint)
{
    if (texture == NULL) {
        return;
    }

    mvn_renderer_t *renderer = SDL_GetRendererFromTexture(texture);
    if (renderer != NULL) {
        // Apply the tint color
        SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
        SDL_SetTextureAlphaMod(texture, tint.a);

        // Define the destination rectangle
        mvn_frect_t dest = {.x = position.x, .y = position.y, .w = source.w, .h = source.h};

        // Render the texture
        SDL_RenderTexture(renderer, texture, &source, &dest);
    }
}

/**
 * \brief           Draw a texture with advanced parameters
 * \param[in]       texture: Texture to be drawn
 * \param[in]       source: Source rectangle defining the portion to draw
 * \param[in]       dest: Destination rectangle to draw the texture to
 * \param[in]       origin: Origin position for rotation/scaling (relative to dest)
 * \param[in]       rotation: Rotation in degrees
 * \param[in]       tint: Color tint to apply to the texture
 */
void mvn_draw_texture_pro(mvn_texture_t *texture,
                          mvn_frect_t    source,
                          mvn_frect_t    dest,
                          mvn_fpoint_t   origin,
                          float          rotation,
                          mvn_color_t    tint)
{
    if (texture == NULL) {
        return;
    }

    mvn_renderer_t *renderer = SDL_GetRendererFromTexture(texture);
    if (renderer != NULL) {
        // Apply the tint color
        SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
        SDL_SetTextureAlphaMod(texture, tint.a);

        // Convert rect to float rect for SDL_RenderTextureRotated
        mvn_frect_t destf = {.x = dest.x, .y = dest.y, .w = dest.w, .h = dest.h};

        // Render the texture with all parameters
        SDL_RenderTextureRotated(
            renderer, texture, &source, &destf, rotation, &origin, SDL_FLIP_NONE);
    }
}

/**
 * \brief           Draw a texture using 9-patch (or 3-patch) layout
 * \param[in]       texture: Texture to be drawn
 * \param[in]       nPatchInfo: NPatch layout information
 * \param[in]       dest: Destination rectangle to draw the texture to
 * \param[in]       origin: Origin position for rotation (relative to dest)
 * \param[in]       rotation: Rotation in degrees
 * \param[in]       tint: Color tint to apply to the texture
 */
void mvn_draw_texture_npatch(mvn_texture_t *   texture,
                             mvn_npatch_info_t nPatchInfo,
                             mvn_frect_t       dest,
                             mvn_fpoint_t      origin,
                             float             rotation,
                             mvn_color_t       tint)
{
    // Check for invalid parameters
    if (!texture || dest.w <= 0 || dest.h <= 0) {
        return;
    }

    mvn_renderer_t *renderer = SDL_GetRendererFromTexture(texture);
    if (!renderer) {
        return;
    }

    // Get the texture dimensions
    float texWidth;
    float texHeight;
    if (SDL_GetTextureSize(texture, &texWidth, &texHeight)) {
        return;
    }

    // Apply the tint color
    SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
    SDL_SetTextureAlphaMod(texture, tint.a);

    // Source rectangle variables
    float sourceX = (float)nPatchInfo.source.x;
    float sourceY = (float)nPatchInfo.source.y;
    float sourceW = (float)nPatchInfo.source.w;
    float sourceH = (float)nPatchInfo.source.h;

    // Calculate corners dimensions
    float leftWidth    = (float)nPatchInfo.left;
    float rightWidth   = (float)nPatchInfo.right;
    float topHeight    = (float)nPatchInfo.top;
    float bottomHeight = (float)nPatchInfo.bottom;

    // Ensure that border dimensions don't exceed the texture size
    if (leftWidth + rightWidth > sourceW) {
        float scale = sourceW / (leftWidth + rightWidth);
        leftWidth *= scale;
        rightWidth *= scale;
    }

    if (topHeight + bottomHeight > sourceH) {
        float scale = sourceH / (topHeight + bottomHeight);
        topHeight *= scale;
        bottomHeight *= scale;
    }

    // Calculate center dimensions
    float centerWidth  = sourceW - leftWidth - rightWidth;
    float centerHeight = sourceH - topHeight - bottomHeight;

    // Calculate destination dimensions
    float destX = dest.x;
    float destY = dest.y;
    float destW = dest.w;
    float destH = dest.h;

    // Dimension values for drawing
    mvn_frect_t sourceRects[9];
    mvn_frect_t destRects[9];
    int         drawCount = 0;

    // Initialize all rects (needed for certain layouts)
    for (int i = 0; i < 9; i++) {
        sourceRects[i] = (mvn_frect_t){0, 0, 0, 0};
        destRects[i]   = (mvn_frect_t){0, 0, 0, 0};
    }

    // Setup based on layout type
    switch (nPatchInfo.layout) {
        case NPATCH_NINE_PATCH: {
            // Calculate source rectangles for 9-patch
            // Top-left
            sourceRects[0] = (mvn_frect_t){sourceX, sourceY, leftWidth, topHeight};
            // Top-center
            sourceRects[1] = (mvn_frect_t){sourceX + leftWidth, sourceY, centerWidth, topHeight};
            // Top-right
            sourceRects[2] =
                (mvn_frect_t){sourceX + leftWidth + centerWidth, sourceY, rightWidth, topHeight};

            // Middle-left
            sourceRects[3] = (mvn_frect_t){sourceX, sourceY + topHeight, leftWidth, centerHeight};
            // Middle-center
            sourceRects[4] =
                (mvn_frect_t){sourceX + leftWidth, sourceY + topHeight, centerWidth, centerHeight};
            // Middle-right
            sourceRects[5] = (mvn_frect_t){
                sourceX + leftWidth + centerWidth, sourceY + topHeight, rightWidth, centerHeight};

            // Bottom-left
            sourceRects[6] =
                (mvn_frect_t){sourceX, sourceY + topHeight + centerHeight, leftWidth, bottomHeight};
            // Bottom-center
            sourceRects[7] = (mvn_frect_t){
                sourceX + leftWidth, sourceY + topHeight + centerHeight, centerWidth, bottomHeight};
            // Bottom-right
            sourceRects[8] = (mvn_frect_t){sourceX + leftWidth + centerWidth,
                                           sourceY + topHeight + centerHeight,
                                           rightWidth,
                                           bottomHeight};

            // Calculate destination rectangles
            float stretchedCenterWidth  = destW - leftWidth - rightWidth;
            float stretchedCenterHeight = destH - topHeight - bottomHeight;

            if (stretchedCenterWidth < 0) {
                // Adjust if destination width is too small
                float scale = destW / (leftWidth + rightWidth);
                leftWidth *= scale;
                rightWidth *= scale;
                stretchedCenterWidth = 0;
            }

            if (stretchedCenterHeight < 0) {
                // Adjust if destination height is too small
                float scale = destH / (topHeight + bottomHeight);
                topHeight *= scale;
                bottomHeight *= scale;
                stretchedCenterHeight = 0;
            }

            // Top-left
            destRects[0] = (mvn_frect_t){destX, destY, leftWidth, topHeight};
            // Top-center
            destRects[1] = (mvn_frect_t){destX + leftWidth, destY, stretchedCenterWidth, topHeight};
            // Top-right
            destRects[2] = (mvn_frect_t){
                destX + leftWidth + stretchedCenterWidth, destY, rightWidth, topHeight};

            // Middle-left
            destRects[3] =
                (mvn_frect_t){destX, destY + topHeight, leftWidth, stretchedCenterHeight};
            // Middle-center
            destRects[4] = (mvn_frect_t){
                destX + leftWidth, destY + topHeight, stretchedCenterWidth, stretchedCenterHeight};
            // Middle-right
            destRects[5] = (mvn_frect_t){destX + leftWidth + stretchedCenterWidth,
                                         destY + topHeight,
                                         rightWidth,
                                         stretchedCenterHeight};

            // Bottom-left
            destRects[6] = (mvn_frect_t){
                destX, destY + topHeight + stretchedCenterHeight, leftWidth, bottomHeight};
            // Bottom-center
            destRects[7] = (mvn_frect_t){destX + leftWidth,
                                         destY + topHeight + stretchedCenterHeight,
                                         stretchedCenterWidth,
                                         bottomHeight};
            // Bottom-right
            destRects[8] = (mvn_frect_t){destX + leftWidth + stretchedCenterWidth,
                                         destY + topHeight + stretchedCenterHeight,
                                         rightWidth,
                                         bottomHeight};

            drawCount = 9;
            break;
        }

        case NPATCH_THREE_PATCH_HORIZONTAL: {
            // Left
            sourceRects[0] = (mvn_frect_t){sourceX, sourceY, leftWidth, sourceH};
            // Center
            sourceRects[1] = (mvn_frect_t){sourceX + leftWidth, sourceY, centerWidth, sourceH};
            // Right
            sourceRects[2] =
                (mvn_frect_t){sourceX + leftWidth + centerWidth, sourceY, rightWidth, sourceH};

            float stretchedCenterWidth = destW - leftWidth - rightWidth;

            if (stretchedCenterWidth < 0) {
                float scale = destW / (leftWidth + rightWidth);
                leftWidth *= scale;
                rightWidth *= scale;
                stretchedCenterWidth = 0;
            }

            // Left
            destRects[0] = (mvn_frect_t){destX, destY, leftWidth, destH};
            // Center
            destRects[1] = (mvn_frect_t){destX + leftWidth, destY, stretchedCenterWidth, destH};
            // Right
            destRects[2] =
                (mvn_frect_t){destX + leftWidth + stretchedCenterWidth, destY, rightWidth, destH};

            drawCount = 3;
            break;
        }

        case NPATCH_THREE_PATCH_VERTICAL: {
            // Top
            sourceRects[0] = (mvn_frect_t){sourceX, sourceY, sourceW, topHeight};
            // Center
            sourceRects[1] = (mvn_frect_t){sourceX, sourceY + topHeight, sourceW, centerHeight};
            // Bottom
            sourceRects[2] =
                (mvn_frect_t){sourceX, sourceY + topHeight + centerHeight, sourceW, bottomHeight};

            float stretchedCenterHeight = destH - topHeight - bottomHeight;

            if (stretchedCenterHeight < 0) {
                float scale = destH / (topHeight + bottomHeight);
                topHeight *= scale;
                bottomHeight *= scale;
                stretchedCenterHeight = 0;
            }

            // Top
            destRects[0] = (mvn_frect_t){destX, destY, destW, topHeight};
            // Center
            destRects[1] = (mvn_frect_t){destX, destY + topHeight, destW, stretchedCenterHeight};
            // Bottom
            destRects[2] = (mvn_frect_t){
                destX, destY + topHeight + stretchedCenterHeight, destW, bottomHeight};

            drawCount = 3;
            break;
        }
    }

    // Apply rotation to all rectangles if needed
    if (rotation != 0.0f) {
        // Calculate center of destination rect for rotation
        mvn_fpoint_t centerPoint = {dest.x + origin.x, dest.y + origin.y};

        // For each patch
        for (int i = 0; i < drawCount; i++) {
            // Draw the current patch with rotation around the specified origin
            SDL_RenderTextureRotated(renderer,
                                     texture,
                                     &sourceRects[i],
                                     &destRects[i],
                                     rotation,
                                     &centerPoint,
                                     SDL_FLIP_NONE);
        }
    } else {
        // No rotation needed, simply render each patch
        for (int i = 0; i < drawCount; i++) {
            SDL_RenderTexture(renderer, texture, &sourceRects[i], &destRects[i]);
        }
    }
}
