/**
 * \file            mvn-text-test.c
 * \brief           Tests for MVN text functionality
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */
#include "mvn-test-utils.h"
#include "mvn/mvn-core.h"
#include "mvn/mvn-text.h"
#include "mvn/mvn-types.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

#define TEST_FONT_PATH "./assets/test-font.ttf" // Path to the test font file
#define TEST_FONT_SIZE 16.0f

static int test_unload_font(void);
static int test_load_font_ex(void);
static int test_set_text_line_spacing(void);
static int test_measure_text(void);
static int test_draw_text(void);
static int test_draw_text_pro(void);

/**
 * \brief           Test loading and unloading a font
 * \return          1 on success, 0 on failure
 */
static int test_unload_font(void)
{
    TTF_Font   *font = NULL;
    char        msg[512];
    const char *test_font_name = "test-font.ttf";
    char        test_font_path[512];

    // Get the test assets directory from environment variable
    const char *assets_dir = ASSET_DIR;
    if (!assets_dir) {
        assets_dir = ""; // Fallback default path
    }

    // Create full path to the test image
    SDL_snprintf(test_font_path, sizeof(test_font_path), "%s/%s", ASSET_DIR, test_font_name);

    // Test loading a potentially non-existent font (error handling)
    font = mvn_load_font("non_existent_font.ttf", TEST_FONT_SIZE);
    TEST_ASSERT(font == NULL, "Loading non-existent font should return NULL");

    // Test loading a valid font (assuming TEST_FONT_PATH is valid)
    font = mvn_load_font(test_font_path, TEST_FONT_SIZE);
    SDL_snprintf(msg, sizeof(msg), "Successfully load test font at %s", test_font_path);
    TEST_ASSERT(font != NULL, msg);

    // Test unloading NULL font (should not crash)
    mvn_unload_font(NULL);

    // Test unloading the loaded font
    if (font != NULL) {
        mvn_unload_font(font);
        font = NULL; // Prevent double free
    }

    return 1;
}

/**
 * \brief           Test loading a font with extended options (codepoints)
 * \return          1 on success, 0 on failure
 */
static int test_load_font_ex(void)
{
    TTF_Font *font         = NULL;
    TTF_Font *font2        = NULL;
    const int codepoints[] = {'A', 'B', 'C'};
    int       count        = SDL_arraysize(codepoints);
    char      msg[128];

    // Test loading non-existent font
    font = mvn_load_font_ex("non_existent_font.ttf", TEST_FONT_SIZE, codepoints, count);
    TEST_ASSERT(font == NULL, "mvn_load_font_ex: Loading non-existent font should return NULL");

    // Test loading valid font with codepoints
    font = mvn_load_font_ex(TEST_FONT_PATH, TEST_FONT_SIZE, codepoints, count);
    SDL_snprintf(
        msg, sizeof(msg), "mvn_load_font_ex: Failed to load test font at %s", TEST_FONT_PATH);
    TEST_ASSERT(font != NULL, msg);

    // Test loading with NULL codepoints
    font2 = mvn_load_font_ex(TEST_FONT_PATH, TEST_FONT_SIZE, NULL, 0);
    TEST_ASSERT(font2 != NULL, "mvn_load_font_ex: Loading with NULL codepoints failed");
    if (font2 != NULL) {
        mvn_unload_font(font2);
    }

    if (font != NULL) {
        mvn_unload_font(font);
    }

    return 1;
}

/**
 * \brief           Test setting text line spacing
 * \return          1 on success, 0 on failure
 */
static int test_set_text_line_spacing(void)
{
    mvn_set_text_line_spacing(5);
    mvn_set_text_line_spacing(-2);
    mvn_set_text_line_spacing(0);
    TEST_ASSERT(true, "mvn_set_text_line_spacing called (no check possible)");
    return 1;
}

/**
 * \brief           Test measuring text dimensions
 * \return          1 on success, 0 on failure
 */
static int test_measure_text(void)
{
    TTF_Font *font                      = NULL;
    int32_t   width                     = 0;
    int32_t   width_no_spacing          = 0;
    int32_t   expected_spacing_increase = 0;
    char      msg[128];

    font = mvn_load_font(TEST_FONT_PATH, TEST_FONT_SIZE);
    SDL_snprintf(msg,
                 sizeof(msg),
                 "mvn_measure_text: Failed to load font for measurement at %s",
                 TEST_FONT_PATH);
    TEST_ASSERT(font != NULL, msg);
    if (font == NULL) {
        return 0;
    }

    width = mvn_measure_text(font, NULL, 0);
    TEST_ASSERT(width == 0, "Measuring NULL text should return 0 width");

    width = mvn_measure_text(font, "", 0);
    TEST_ASSERT(width == 0, "Measuring empty text should return 0 width");

    width = mvn_measure_text(font, "Hello", 0);
    TEST_ASSERT(width > 0, "Measuring 'Hello' should return positive width");

    width_no_spacing = width;
    width            = mvn_measure_text(font, "Hello", 2.0f);
    TEST_ASSERT(width > width_no_spacing, "Width with spacing should be greater");
    expected_spacing_increase = (int32_t)((float)(SDL_strlen("Hello") - 1) * 2.0f);
    SDL_snprintf(msg,
                 sizeof(msg),
                 "Width with spacing calculation mismatch: got %d, expected %d",
                 width,
                 width_no_spacing + expected_spacing_increase);
    TEST_ASSERT(width == width_no_spacing + expected_spacing_increase, msg);

    width = mvn_measure_text(NULL, "Test", 0);
    TEST_ASSERT(width == 0, "Measuring with NULL font should return 0 width");

    mvn_unload_font(font);
    return 1;
}

/**
 * \brief           Test drawing basic text
 * \return          1 on success, 0 on failure
 */
static int test_draw_text(void)
{
    TTF_Font    *font  = NULL;
    mvn_fpoint_t pos   = {10.0f, 10.0f};
    mvn_color_t  color = MVN_WHITE;
    char         msg[128];

    font = mvn_load_font(TEST_FONT_PATH, TEST_FONT_SIZE);
    SDL_snprintf(
        msg, sizeof(msg), "mvn_draw_text: Failed to load font for drawing at %s", TEST_FONT_PATH);
    TEST_ASSERT(font != NULL, msg);
    if (font == NULL) {
        return 0;
    }

    mvn_draw_text(font, NULL, pos, color);
    TEST_ASSERT(true, "mvn_draw_text with NULL text called (no crash check)");

    mvn_draw_text(font, "", pos, color);
    TEST_ASSERT(true, "mvn_draw_text with empty text called (no crash check)");

    mvn_draw_text(NULL, "Test", pos, color);
    TEST_ASSERT(true, "mvn_draw_text with NULL font called (no crash check)");

    mvn_draw_text(font, "Hello World", pos, color);
    TEST_ASSERT(true, "mvn_draw_text with valid text called (no crash check)");

    mvn_unload_font(font);
    return 1;
}

/**
 * \brief           Test drawing text with extended parameters (rotation)
 * \return          1 on success, 0 on failure
 */
static int test_draw_text_pro(void)
{
    TTF_Font    *font     = NULL;
    mvn_fpoint_t pos      = {50.0f, 50.0f};
    mvn_fpoint_t origin   = {0.0f, 0.0f};
    mvn_color_t  color    = MVN_WHITE;
    float        rotation = 45.0f;
    char         msg[128];

    font = mvn_load_font(TEST_FONT_PATH, TEST_FONT_SIZE);
    SDL_snprintf(msg,
                 sizeof(msg),
                 "mvn_draw_text_pro: Failed to load font for drawing at %s",
                 TEST_FONT_PATH);
    TEST_ASSERT(font != NULL, msg);
    if (font == NULL) {
        return 0;
    }

    mvn_draw_text_pro(font, NULL, pos, origin, rotation, color);
    TEST_ASSERT(true, "mvn_draw_text_pro with NULL text called (no crash check)");

    mvn_draw_text_pro(font, "", pos, origin, rotation, color);
    TEST_ASSERT(true, "mvn_draw_text_pro with empty text called (no crash check)");

    mvn_draw_text_pro(NULL, "Test", pos, origin, rotation, color);
    TEST_ASSERT(true, "mvn_draw_text_pro with NULL font called (no crash check)");

    mvn_draw_text_pro(font, "Rotated Text", pos, origin, rotation, color);
    TEST_ASSERT(true, "mvn_draw_text_pro with valid text called (no crash check)");

    origin.x = 10.0f;
    origin.y = 5.0f;
    mvn_draw_text_pro(font, "Rotated Text Origin", pos, origin, rotation, color);
    TEST_ASSERT(true, "mvn_draw_text_pro with origin offset called (no crash check)");

    mvn_unload_font(font);
    return 1;
}

/**
 * \brief           Run all string tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int run_text_tests(int *passed_tests, int *failed_tests, int *total_tests)
{

    printf("\n===== TEXT TESTS =====\n\n");

#if defined(MVN_TEST_CI)
    printf("Skipping text tests in CI mode.\n");
#else
    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    // Initialize SDL and TTF for these tests
    // A minimal window is needed for renderer/text engine context
    if (!mvn_init(100, 100, "Text Test", MVN_WINDOW_HIDDEN)) {
        printf("ERROR: Failed to initialize MVN for text tests. Skipping text tests.\n");
        // Increment total for the category attempt, but mark all as failed implicitly
        (*total_tests)++;
        (*failed_tests)++;
        return 0;
    }

    RUN_TEST(test_unload_font);
    RUN_TEST(test_load_font_ex);
    RUN_TEST(test_set_text_line_spacing);
    RUN_TEST(test_measure_text);
    RUN_TEST(test_draw_text);
    RUN_TEST(test_draw_text_pro);

    // Clean up SDL and TTF
    mvn_quit();

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run; // Note the parentheses

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
#endif // MVN_TEST_CI
}

int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    run_text_tests(&passed, &failed, &total);

    printf("\n===== TEXT TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
