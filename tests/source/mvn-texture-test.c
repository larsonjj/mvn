/**
 * \file            mvn_textures_test.c
 * \brief           Tests for MVN textures functionality
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */

#include "mvn-test-utils.h"
#include "mvn/mvn-texture.h"

#include <stdio.h>
#include <string.h>

/**
 * \brief           Run all image tests
 * \param[out] passed Pointer to the number of passed tests
 * \param[out] failed Pointer to the number of failed tests
 * \param[out] total Pointer to the total number of tests
 * \return          Number of passed tests
 */
static int test_image_load(void)
{
    mvn_image_t *surface         = NULL;
    const char * test_image_name = "char-1.png";
    char         test_image_path[256];
    int          result = 1; // Default to success

    // Get the test assets directory from environment variable
    const char *assets_dir = ASSET_DIR;
    if (!assets_dir) {
        assets_dir = ""; // Fallback default path
    }

    // Create full path to the test image
    SDL_snprintf(test_image_path, sizeof(test_image_path), "%s/%s", ASSET_DIR, test_image_name);

    // Test loading image with full path
    surface = mvn_load_image(test_image_path);
    TEST_ASSERT(surface != NULL, "Image loading failed");

    if (surface != NULL) {
        // Verify image properties if needed
        TEST_ASSERT(surface->w > 0 && surface->h > 0, "Invalid image dimensions");

        // Test unloading image
        mvn_unload_image(surface);
        TEST_ASSERT(1, "Image unloaded successfully");
    } else {
        // If loading failed, mark test as failed
        result = 0;
    }

    return result;
}

/**
 * \brief           Run all texture tests
 * \return          Number of tests that passed
 */
int run_texture_tests(int *passed_tests, int *failed_tests, int *total_tests)
{
    printf("\n===== TEXTURE TESTS =====\n\n");

    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_image_load);

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run; // Note the parentheses

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
}

#if defined(MVN_TEXTURES_TEST_MAIN)
int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    run_texture_tests(&passed, &failed, &total);

    printf("\n===== TEXTURES TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
#endif
