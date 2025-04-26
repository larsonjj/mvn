/**
 * \file            mvn_core_test.c
 * \brief           Tests for MVN core functionality
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */

#include <SDL3/SDL.h>
#include "mvn-test-utils.h"
#include "mvn/mvn-core.h"
#include "mvn/mvn-string.h"

/**
  * \brief           Test the engine version function
  * \return          1 on success, 0 on failure
  */
static int
test_engine_version(void) {
    mvn_string_t* version;

    // Get the engine version
    version = mvn_get_engine_version();

    // Check that the function returns a non-NULL value
    TEST_ASSERT(version != NULL, "Engine version should not be NULL");

    // Check that the version string matches the expected value (from CMakeLists.txt)
    TEST_ASSERT(SDL_strcmp(mvn_string_to_cstr(version), "0.1.0") == 0,
                "Engine version should match expected value");

    // Free the string when done
    mvn_string_free(version);

    return 1;
}

/**
  * \brief           Test color functionality
  * \return          1 on success, 0 on failure
  */
static int
test_colors(void) {
    // Test predefined colors
    mvn_color_t white = MVN_WHITE;
    mvn_color_t black = MVN_BLACK;

    TEST_ASSERT(white.r == 255 && white.g == 255 && white.b == 255 && white.a == 255,
                "WHITE color should be (255, 255, 255, 255)");

    TEST_ASSERT(black.r == 0 && black.g == 0 && black.b == 0 && black.a == 255,
                "BLACK color should be (0, 0, 0, 255)");

    return 1;
}

/**
 * \brief           Test core timing functions
 * \return          1 on success, 0 on failure
 */
static int
test_core_timing(void) {
    // Initialize MVN (assuming it's not already done by a test fixture)
    // A minimal window is needed for timing initialization in mvn_init
    if (!mvn_init(10, 10, "Timing Test", MVN_WINDOW_HIDDEN)) {
        TEST_ASSERT(false, "mvn_init failed for timing test");
        return 0; // Cannot proceed if init fails
    }

    // --- Test mvn_get_time ---
    double time1 = mvn_get_time();
    TEST_ASSERT(time1 >= 0.0, "Initial mvn_get_time should be non-negative");
    SDL_Delay(110); // Delay for ~110ms
    double time2 = mvn_get_time();
    TEST_ASSERT(time2 > time1, "mvn_get_time should increase after delay");
    // Check if time increased by roughly the delay amount (allow tolerance)
    double elapsed = time2 - time1;
    TEST_ASSERT(elapsed > 0.09 && elapsed < 0.13,
                "Elapsed time is not within expected range (0.09s - 0.13s)");

    // --- Test mvn_set_target_fps ---
    // Call with various values to ensure it doesn't crash. Direct verification is hard.
    mvn_set_target_fps(30);
    mvn_set_target_fps(60);
    mvn_set_target_fps(0);  // Uncapped
    mvn_set_target_fps(-1); // Uncapped

    // --- Test mvn_get_frame_time and mvn_get_fps ---
    mvn_set_target_fps(60); // Set a target for testing
    int frame_count = 0;
    uint64_t loop_start_ticks = SDL_GetPerformanceCounter();
    uint64_t loop_duration_ticks = SDL_GetPerformanceFrequency(); // 1 second

    // Run a loop for slightly over 1 second to allow FPS calculation
    while (SDL_GetPerformanceCounter()
           < loop_start_ticks + loop_duration_ticks + (loop_duration_ticks / 10)) {
        mvn_begin_drawing();
        // Simulate some work or just continue
        mvn_end_drawing(); // This handles frame delay and FPS calculation update
        frame_count++;

        // Check frame time after a few frames (it might be zero initially)
        if (frame_count > 5) {
            float frame_time = mvn_get_frame_time();
            TEST_ASSERT(frame_time >= 0.0f, "mvn_get_frame_time should be non-negative");
            // If limited, frame time should be around 1/60th of a second (~0.0166)
            // If not limited (CPU is slower), it will be higher. Just check non-negative here.
        }
    }

    int fps = mvn_get_fps();
    // Allow a generous range for FPS due to timing variations, especially on test runners
    TEST_ASSERT(fps > 0 && fps < 100, "mvn_get_fps seems unreasonable (expected ~60)");
    // A tighter check might be: TEST_ASSERT(fps > 50 && fps < 70, "mvn_get_fps not close to target 60");

    // Clean up MVN
    mvn_quit();

    return 1; // Success
}

/**
 * \brief           Run all core tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int
run_core_tests(int* passed_tests, int* failed_tests, int* total_tests) {
    printf("\n===== CORE TESTS =====\n\n");

    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_engine_version);
    RUN_TEST(test_colors);
    RUN_TEST(test_core_timing);

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run; // Note the parentheses

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
}

#if defined(MVN_CORE_TEST_MAIN)
int
main(void) {
    int passed = 0;
    int failed = 0;
    int total = 0;

    run_core_tests(&passed, &failed, &total);

    printf("\n===== CORE TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
#endif
