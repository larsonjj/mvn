/**
 * \file            mvn-error-test.c
 * \brief           Tests for MVN error handling functionality
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

#include "mvn-error-test.h"

#include "mvn-test-utils.h"
#include "mvn/mvn-error.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>

/**
 * \brief           Test basic error setting and retrieval
 * \return          true if test passes, false otherwise
 */
static bool test_basic_error(void)
{
    printf("Testing basic error functionality...\n");

    mvn_clear_error(); // Start with a clean state

    // Set an error message
    mvn_set_error("Test error message");

    // Check if error was set correctly
    const char *error_msg = mvn_get_error();
    TEST_ASSERT(error_msg != NULL, "Error message should not be NULL");
    TEST_ASSERT(SDL_strcmp(error_msg, "Test error message") == 0,
                "Error message not set correctly");

    // Test clearing the error
    mvn_clear_error();
    error_msg = mvn_get_error();
    TEST_ASSERT(SDL_strcmp(error_msg, "") == 0, "Error should be empty after clearing");

    printf("PASS: Basic error functionality working correctly\n");
    return true;
}

/**
 * \brief           Test error formatting with parameters
 * \return          true if test passes, false otherwise
 */
static bool test_error_formatting(void)
{
    printf("Testing error formatting...\n");

    // Set an error with formatting
    mvn_set_error("Error with number %d and string %s", 42, "test");

    // Check if formatting worked
    const char *error_msg = mvn_get_error();
    TEST_ASSERT(SDL_strstr(error_msg, "42") != NULL, "Numeric parameter not formatted correctly");
    TEST_ASSERT(SDL_strstr(error_msg, "test") != NULL, "String parameter not formatted correctly");

    printf("PASS: Error formatting working correctly\n");
    return true;
}

/**
 * \brief           Test error function return values
 * \return          true if test passes, false otherwise
 */
static bool test_error_return_values(void)
{
    printf("Testing error function return values...\n");

    // Test that mvn_set_error returns false
    bool result = mvn_set_error("Return test");
    TEST_ASSERT(result == false, "mvn_set_error should return false");

    printf("PASS: Error function return values correct\n");
    return true;
}

/**
 * \brief           Run all error tests
 * \param[out]      passed_tests: Pointer to passed tests counter
 * \param[out]      failed_tests: Pointer to failed tests counter
 * \param[out]      total_tests: Pointer to total tests counter
 */
void run_error_tests(int *passed_tests, int *failed_tests, int *total_tests)
{
    printf("\n===== RUNNING ERROR TESTS =====\n");

    *total_tests += 3; // Update this if you add more tests

    RUN_TEST(test_basic_error);
    RUN_TEST(test_error_formatting);
    RUN_TEST(test_error_return_values);

    printf("\n");
}

int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    run_error_tests(&passed, &failed, &total);

    printf("\n===== FILE TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
