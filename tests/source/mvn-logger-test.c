/**
 * \file            mvn-test-logger.c
 * \brief           Tests for MVN logging functionality
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

#include "mvn-test-utils.h"
#include "mvn/mvn-logger.h"

#include <SDL3/SDL.h>
#include <stdio.h>

/**
 * \brief           Custom log output function to redirect logs to a buffer for testing
 * \param[in]       userdata: User data pointer (our log buffer)
 * \param[in]       category: Log category
 * \param[in]       priority: Log priority
 * \param[in]       message: Log message
 */
static void
test_log_output(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    (void)category; /* Unused */
    (void)priority; /* Unused */

    char **buffer = (char **)userdata;
    if (buffer != NULL && *buffer != NULL) {
        /* Append to our test buffer */
        size_t current_len = SDL_strlen(*buffer);
        size_t message_len = SDL_strlen(message);

        char *new_buffer =
            SDL_realloc(*buffer, current_len + message_len + 2); /* +2 for \n and \0 */
        if (new_buffer) {
            *buffer = new_buffer;
            SDL_strlcpy(*buffer + current_len, message, message_len + 1);
            (*buffer)[current_len + message_len]     = '\n';
            (*buffer)[current_len + message_len + 1] = '\0';
        }
    }
}

/**
 * \brief           Test logger initialization
 * \return          true if test passes, false otherwise
 */
static bool test_logger_init(void)
{
    printf("Testing logger initialization...\n");

    bool result = mvn_logger_init();
    if (!result) {
        printf("FAIL: Logger initialization failed\n");
        return false;
    }

    printf("PASS: Logger initialized successfully\n");
    return true;
}

/**
 * \brief           Test setting log levels
 * \return          true if test passes, false otherwise
 */
static bool test_logger_set_levels(void)
{
    printf("Testing logger level settings...\n");

    /* Save the original output function to restore later */
    SDL_LogOutputFunction original_fn;
    void *                original_userdata;
    SDL_GetLogOutputFunction(&original_fn, &original_userdata);

    /* Test setting individual category levels */
    mvn_logger_set_level(MVN_LOG_CATEGORY_DEFAULT, MVN_LOG_DEBUG);
    mvn_logger_set_level(MVN_LOG_CATEGORY_ERROR, MVN_LOG_ERROR);

    /* Test setting all levels at once */
    mvn_logger_set_all_levels(MVN_LOG_INFO);

    /* Restore original output function */
    SDL_SetLogOutputFunction(original_fn, original_userdata);

    printf("PASS: Logger level settings applied\n");
    return true;
}

/**
 * \brief           Test basic logging functionality
 * \return          true if test passes, false otherwise
 */
static bool test_basic_logging(void)
{
    printf("Testing basic logging functionality...\n");

    /* Set up our test log output */
    char *log_buffer = SDL_calloc(1, 1); /* Start with empty string */
    if (!log_buffer) {
        printf("FAIL: Could not allocate log buffer\n");
        return false;
    }

    /* Save the original output function to restore later */
    SDL_LogOutputFunction original_fn;
    void *                original_userdata;
    SDL_GetLogOutputFunction(&original_fn, &original_userdata);

    /* Set our test log function */
    SDL_SetLogOutputFunction(test_log_output, (void *)&log_buffer);

    /* Set all levels to debug to ensure we capture everything */
    mvn_logger_set_all_levels(MVN_LOG_DEBUG);

    /* Test all logging functions */
    mvn_log_debug("This is a debug message");
    mvn_log_info("This is an info message");
    mvn_log_warn("This is a warning message");
    mvn_log_error("This is an error message");
    mvn_log_critical("This is a critical message");

    /* Check if our buffer contains the messages */
    bool result = true;
    if (SDL_strstr(log_buffer, "debug message") == NULL) {
        printf("FAIL: Debug message was not logged\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "info message") == NULL) {
        printf("FAIL: Info message was not logged\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "warning message") == NULL) {
        printf("FAIL: Warning message was not logged\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "error message") == NULL) {
        printf("FAIL: Error message was not logged\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "critical message") == NULL) {
        printf("FAIL: Critical message was not logged\n");
        result = false;
    }

    /* Restore original output function */
    SDL_SetLogOutputFunction(original_fn, original_userdata);

    /* Free our buffer */
    SDL_free(log_buffer);

    if (result) {
        printf("PASS: Basic logging functionality working\n");
    }

    return result;
}

/**
 * \brief           Test log filtering based on level
 * \return          true if test passes, false otherwise
 */
static bool test_log_filtering(void)
{
    printf("Testing log level filtering...\n");

    /* Set up our test log output */
    char *log_buffer = SDL_calloc(1, 1); /* Start with empty string */
    if (!log_buffer) {
        printf("FAIL: Could not allocate log buffer\n");
        return false;
    }

    /* Save the original output function to restore later */
    SDL_LogOutputFunction original_fn;
    void *                original_userdata;
    SDL_GetLogOutputFunction(&original_fn, &original_userdata);

    /* Set our test log function */
    SDL_SetLogOutputFunction(test_log_output, (void *)&log_buffer);

    /* Set log level to WARN - this should filter out DEBUG and INFO messages */
    mvn_logger_set_all_levels(MVN_LOG_WARN);

    /* Test all logging functions */
    mvn_log_debug("This debug message should be filtered");
    mvn_log_info("This info message should be filtered");
    mvn_log_warn("This warning message should appear");
    mvn_log_error("This error message should appear");
    mvn_log_critical("This critical message should appear");

    /* Check filtering */
    bool result = true;
    if (SDL_strstr(log_buffer, "should be filtered") != NULL) {
        printf("FAIL: Debug or info messages were not filtered\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "warning message should appear") == NULL) {
        printf("FAIL: Warning message was filtered incorrectly\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "error message should appear") == NULL) {
        printf("FAIL: Error message was filtered incorrectly\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "critical message should appear") == NULL) {
        printf("FAIL: Critical message was filtered incorrectly\n");
        result = false;
    }

    /* Restore original output function */
    SDL_SetLogOutputFunction(original_fn, original_userdata);

    /* Free our buffer */
    SDL_free(log_buffer);

    if (result) {
        printf("PASS: Log level filtering working correctly\n");
    }

    return result;
}

/**
 * \brief           Test category-specific logging
 * \return          true if test passes, false otherwise
 */
static bool test_category_logging(void)
{
    printf("Testing category-specific logging...\n");

    /* Set up our test log output */
    char *log_buffer = SDL_calloc(1, 1); /* Start with empty string */
    if (!log_buffer) {
        printf("FAIL: Could not allocate log buffer\n");
        return false;
    }

    /* Save the original output function to restore later */
    SDL_LogOutputFunction original_fn;
    void *                original_userdata;
    SDL_GetLogOutputFunction(&original_fn, &original_userdata);

    /* Set our test log function */
    SDL_SetLogOutputFunction(test_log_output, (void *)&log_buffer);

    /* Set different levels for different categories */
    mvn_logger_set_level(MVN_LOG_CATEGORY_DEFAULT, MVN_LOG_ERROR); /* Only ERROR and above */
    mvn_logger_set_level(MVN_LOG_CATEGORY_RENDER, MVN_LOG_DEBUG);  /* All messages */

    /* Test category-specific logging */
    mvn_log(MVN_LOG_CATEGORY_DEFAULT, MVN_LOG_DEBUG, "Default debug should be filtered");
    mvn_log(MVN_LOG_CATEGORY_DEFAULT, MVN_LOG_ERROR, "Default error should appear");
    mvn_log(MVN_LOG_CATEGORY_RENDER, MVN_LOG_DEBUG, "Render debug should appear");

    /* Check category filtering */
    bool result = true;
    if (SDL_strstr(log_buffer, "Default debug should be filtered") != NULL) {
        printf("FAIL: Default category debug message was not filtered\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "Default error should appear") == NULL) {
        printf("FAIL: Default category error message was filtered incorrectly\n");
        result = false;
    }
    if (SDL_strstr(log_buffer, "Render debug should appear") == NULL) {
        printf("FAIL: Render category debug message was filtered incorrectly\n");
        result = false;
    }

    /* Restore original output function */
    SDL_SetLogOutputFunction(original_fn, original_userdata);

    /* Free our buffer */
    SDL_free(log_buffer);

    if (result) {
        printf("PASS: Category-specific logging working correctly\n");
    }

    return result;
}

/**
 * \brief           Test debug vs. release logging configuration example
 * \return          true if test passes, false otherwise
 */
static bool test_debug_vs_release_config(void)
{
    printf("Testing debug vs. release logging configuration...\n");

    /* Set up our test log output */
    char *log_buffer = SDL_calloc(1, 1); /* Start with empty string */
    if (!log_buffer) {
        printf("FAIL: Could not allocate log buffer\n");
        return false;
    }

    /* Save the original output function to restore later */
    SDL_LogOutputFunction original_fn;
    void *                original_userdata;
    SDL_GetLogOutputFunction(&original_fn, &original_userdata);

    /* Set our test log function */
    SDL_SetLogOutputFunction(test_log_output, (void *)&log_buffer);

    /* Clear the buffer */
    SDL_free(log_buffer);
    log_buffer = SDL_calloc(1, 1);

    /* First test "debug" configuration */
    printf("  Testing 'debug' configuration...\n");
    mvn_logger_set_all_levels(MVN_LOG_VERBOSE);

    /* Log at various levels */
    mvn_log_debug("Debug message for debugging");
    mvn_log_info("Info message for debugging");
    mvn_log_warn("Warning message for debugging");

    /* Verify all messages were logged */
    bool debug_result = true;
    if (SDL_strstr(log_buffer, "Debug message for debugging") == NULL) {
        printf("FAIL: Debug message not logged in debug configuration\n");
        debug_result = false;
    }
    if (SDL_strstr(log_buffer, "Info message for debugging") == NULL) {
        printf("FAIL: Info message not logged in debug configuration\n");
        debug_result = false;
    }

    /* Clear the buffer for release test */
    SDL_free(log_buffer);
    log_buffer = SDL_calloc(1, 1);

    /* Now test "release" configuration */
    printf("  Testing 'release' configuration...\n");
    mvn_logger_set_all_levels(MVN_LOG_INFO);

    /* Log at various levels */
    mvn_log_debug("Debug message for release");
    mvn_log_info("Info message for release");
    mvn_log_warn("Warning message for release");

    /* Verify debug messages were filtered out */
    bool release_result = true;
    if (SDL_strstr(log_buffer, "Debug message for release") != NULL) {
        printf("FAIL: Debug message was logged in release configuration\n");
        release_result = false;
    }
    if (SDL_strstr(log_buffer, "Info message for release") == NULL) {
        printf("FAIL: Info message not logged in release configuration\n");
        release_result = false;
    }

    /* Restore original output function */
    SDL_SetLogOutputFunction(original_fn, original_userdata);

    /* Free our buffer */
    SDL_free(log_buffer);

    if (debug_result && release_result) {
        printf("PASS: Debug vs. release configuration working correctly\n");
    }

    return debug_result && release_result;
}

/**
 * \brief           Run all logger tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int run_logger_tests(int *passed_tests, int *failed_tests, int *total_tests)
{
    printf("\n===== LIST TESTS =====\n\n");
    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_logger_init);
    RUN_TEST(test_logger_set_levels);
    RUN_TEST(test_basic_logging);
    RUN_TEST(test_log_filtering);
    RUN_TEST(test_category_logging);
    RUN_TEST(test_debug_vs_release_config);
    RUN_TEST(test_logger_set_levels);

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run;

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
}

int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    run_logger_tests(&passed, &failed, &total);

    printf("\n===== LOGGER TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
