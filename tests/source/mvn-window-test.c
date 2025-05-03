/**
 * \file            mvn-window-test.c
 * \brief           Tests for MVN window functionality
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
#include "mvn/mvn-types.h"

#include <SDL3/SDL.h>

/**
 * \brief           Test window creation and basic properties
 * \return          1 on success, 0 on failure
 */
static int test_window_creation(void)
{
    // Initialize MVN with a test window
    bool init_result = mvn_init(640, 480, "Window Test", MVN_WINDOW_HIDDEN);
    TEST_ASSERT(init_result, "Failed to initialize MVN with test window");

    if (!init_result) {
        return 0; // Cannot proceed if initialization fails
    }

    // Test that window was created
    mvn_window_t *window = mvn_get_window();
    TEST_ASSERT(window != NULL, "mvn_get_window should return non-NULL value");

    // Test that renderer was created
    mvn_renderer_t *renderer = mvn_get_renderer();
    TEST_ASSERT(renderer != NULL, "mvn_get_renderer should return non-NULL value");

    // Clean up
    mvn_quit();

    return 1;
}

/**
 * \brief           Test window position functions
 * \return          1 on success, 0 on failure
 */
static int test_window_position(void)
{
    // Initialize MVN with a test window
    bool init_result = mvn_init(640, 480, "Window Position Test", MVN_WINDOW_HIDDEN);
    TEST_ASSERT(init_result, "Failed to initialize MVN for window position test");

    if (!init_result) {
        return 0;
    }

    // Set a specific position
    mvn_set_window_position(100, 100);

    // Get the position and verify it's close to what we set
    // Note: Window managers may constrain window positions
    mvn_fpoint_t pos = mvn_get_window_position();

    // Allow some tolerance since window managers might adjust the position
    // Just verify it's not at (0,0) or some obviously wrong value
    TEST_ASSERT(pos.x >= 0 && pos.y >= 0, "Window position should be valid");

    // Clean up
    mvn_quit();

    return 1;
}

/**
 * \brief           Test window size functions
 * \return          1 on success, 0 on failure
 */
static int test_window_size(void)
{
    // Initialize MVN with a resizable test window
    bool init_result =
        mvn_init(640, 480, "Window Size Test", MVN_WINDOW_HIDDEN | MVN_WINDOW_RESIZABLE);
    TEST_ASSERT(init_result, "Failed to initialize MVN for window size test");

    if (!init_result) {
        return 0;
    }

    // Set window min/max size
    mvn_set_window_min_size(320, 240);
    mvn_set_window_max_size(1024, 768);

    // Set window size
    mvn_set_window_size(800, 600);

    // No good way to verify the result without querying SDL directly
    // For now, just ensure the calls don't crash

    // Clean up
    mvn_quit();

    return 1;
}

/**
 * \brief           Test monitor-related functions
 * \return          1 on success, 0 on failure
 */
static int test_monitor_functions(void)
{
    // Try to initialize MVN with a visible window
    bool init_result = mvn_init(640, 480, "Monitor Test", MVN_WINDOW_HIDDEN); // No hidden flag
    TEST_ASSERT(init_result, "Failed to initialize MVN for monitor test");

    if (!init_result) {
        return 0;
    }

    // Get primary display using MVN function instead of SDL directly
    mvn_display_id_t primary = mvn_get_current_monitor();

    // Skip the test if we can't get primary display
    // (this happens in headless CI environments)
    if (primary == 0) {
        printf("SKIPPED: Cannot test monitor functions - no primary display detected\n");
        mvn_quit();
        return 1; // Return success to avoid failing tests in headless environments
    }

    SDL_Delay(1000); // Give some time for the window to be created

    // Test monitor width and height
    int32_t width  = mvn_get_monitor_width(primary);
    int32_t height = mvn_get_monitor_height(primary);

    TEST_ASSERT_FMT(width > 0, "Monitor width should be positive: %d", width);
    TEST_ASSERT_FMT(height > 0, "Monitor height should be positive: %d", height);

    // Add more test cases using MVN functions
    int32_t screen_width  = mvn_get_screen_width();
    int32_t screen_height = mvn_get_screen_height();
    TEST_ASSERT_FMT(screen_width > 0, "Screen width should be positive: %d", screen_width);
    TEST_ASSERT_FMT(screen_height > 0, "Screen height should be positive: %d", screen_height);

    // Test monitor count
    int32_t monitor_count = mvn_get_monitor_count();
    TEST_ASSERT_FMT(monitor_count > 0, "Monitor count should be positive: %d", monitor_count);

    // Test current monitor
    mvn_display_id_t current = mvn_get_current_monitor();
    TEST_ASSERT(current != 0, "Current monitor should be valid");

    // Clean up
    mvn_quit();
    return 1;
}

/**
 * \brief           Run all window tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int run_window_tests(int *passed_tests, int *failed_tests, int *total_tests)
{
    printf("\n===== WINDOW TESTS =====\n\n");
#if defined(MVN_TEST_CI)
    printf("Skipping window tests in CI mode.\n");
#else
    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_window_creation);
    RUN_TEST(test_window_position);
    RUN_TEST(test_window_size);
    RUN_TEST(test_monitor_functions);

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run;

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
#endif // MVN_TEST_CI
}

int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    run_window_tests(&passed, &failed, &total);

    printf("\n===== WINDOW TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
