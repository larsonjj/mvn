/**
 * \file            mvn-file-test.c
 * \brief           Tests for MVN file functionality
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */
#include "mvn-test-utils.h"
#include "mvn/mvn-file.h"
#include "mvn/mvn-logger.h"
#include "mvn/mvn-string.h"

#include <SDL3/SDL.h>

#define TEMP_DIR_NAME  "mvn_test_temp_dir"
#define TEMP_FILE_NAME "mvn_test_temp_file.txt"
#define TEMP_FILE_PATH TEMP_DIR_NAME "/" TEMP_FILE_NAME

/**
 * \brief           Test mvn_get_application_directory
 * \return          1 on success, 0 on failure
 */
static int test_get_application_directory(void)
{
    mvn_string_t *app_dir = mvn_get_application_directory();
    TEST_ASSERT(app_dir != NULL, "mvn_get_application_directory returned NULL");
    if (app_dir == NULL) {
        return 0;
    }

    const char *path_cstr = mvn_string_to_cstr(app_dir);
    TEST_ASSERT(path_cstr != NULL, "String data is NULL");
    TEST_ASSERT(SDL_strlen(path_cstr) > 0, "Application directory path is empty");

    size_t len = SDL_strlen(path_cstr);
    TEST_ASSERT(path_cstr[len - 1] == '/' || path_cstr[len - 1] == '\\',
                "Application directory path should end with a separator");

    mvn_string_free(app_dir);
    return 1;
}

/**
 * \brief           Test mvn_is_path_file and mvn_is_path_directory
 * \return          1 on success, 0 on failure
 */
static int test_is_path_file_directory(void)
{
    // Create temporary directory and file
    if (!SDL_CreateDirectory(TEMP_DIR_NAME)) {
        mvn_log_warn("Temp dir may already exist: %s", TEMP_DIR_NAME);
    }
    SDL_IOStream *file = SDL_IOFromFile(TEMP_FILE_PATH, "w");
    if (file == NULL) {
        mvn_log_error("Failed to create temp file: %s", TEMP_FILE_PATH);
        (void)SDL_RemovePath(TEMP_DIR_NAME);
    } else {
        const char *content = "test";
        size_t      written = SDL_WriteIO(file, content, SDL_strlen(content));
        if (written != SDL_strlen(content)) {
            mvn_log_error("Failed to write to temp file: %s", TEMP_FILE_PATH);
        }
        if (!SDL_CloseIO(file)) {
            mvn_log_error("Failed to close temp file: %s", TEMP_FILE_PATH);
        }
    }

    // Use SDL_GetPathInfo to check existence and type
    bool file_exists = mvn_is_path_file(TEMP_FILE_PATH);
    bool dir_exists  = mvn_is_path_directory(TEMP_DIR_NAME);

    if (dir_exists && file_exists) {
        TEST_ASSERT(mvn_is_path_file(TEMP_FILE_PATH), "mvn_is_path_file failed for existing file");
        TEST_ASSERT(!mvn_is_path_directory(TEMP_FILE_PATH),
                    "mvn_is_path_directory succeeded for existing file");
        TEST_ASSERT(!mvn_is_path_file(TEMP_DIR_NAME),
                    "mvn_is_path_file succeeded for existing directory");
        TEST_ASSERT(mvn_is_path_directory(TEMP_DIR_NAME),
                    "mvn_is_path_directory failed for existing directory");
    } else {
        TEST_ASSERT(false, "Could not create temp file/dir, skipping path type tests");
    }

    // Test non-existent paths
    bool non_file_exists = mvn_is_path_file("non_existent_path_file");
    bool non_dir_exists  = mvn_is_path_directory("non_existent_path_dir");
    TEST_ASSERT(!non_file_exists, "SDL_GetPathInfo succeeded for non-existent file");
    TEST_ASSERT(!non_dir_exists, "SDL_GetPathInfo succeeded for non-existent dir");
    TEST_ASSERT(!mvn_is_path_file("non_existent_path_file"),
                "mvn_is_path_file succeeded for non-existent path");
    TEST_ASSERT(!mvn_is_path_directory("non_existent_path_dir"),
                "mvn_is_path_directory succeeded for non-existent path");

    // Clean up
    if (file_exists) {
        if (!SDL_RemovePath(TEMP_FILE_PATH)) {
            mvn_log_error("Failed to remove temp file: %s", SDL_GetError());
        }
    }
    if (dir_exists) {
        if (!SDL_RemovePath(TEMP_DIR_NAME)) {
            mvn_log_error("Failed to remove temp directory: %s", SDL_GetError());
        }
    }

    return 1;
}

/**
 * \brief           Test mvn_get_file_mod_time
 * \return          1 on success, 0 on failure
 */
static int test_get_file_mod_time(void)
{
    if (!SDL_CreateDirectory(TEMP_DIR_NAME)) {
        mvn_log_warn("Temp dir may already exist: %s", TEMP_DIR_NAME);
    }
    SDL_IOStream *file = SDL_IOFromFile(TEMP_FILE_PATH, "w");
    if (file == NULL) {
        mvn_log_error("Failed to create temp file for mod time test: %s", TEMP_FILE_PATH);
        (void)SDL_RemovePath(TEMP_DIR_NAME);
        return 0;
    }

    const char *content = "initial";
    size_t      written = SDL_WriteIO(file, content, SDL_strlen(content));
    if (written != SDL_strlen(content)) {
        mvn_log_error("Failed to write initial content for mod time test: %s", TEMP_FILE_PATH);
    }
    if (!SDL_CloseIO(file)) {
        mvn_log_error("Failed to close initial file for mod time test: %s", TEMP_FILE_PATH);
    }

    // Add a delay to ensure the file system records the modification time
    SDL_Delay(5000);

    bool file_exists = mvn_is_path_file(TEMP_FILE_PATH);
    bool dir_exists  = mvn_is_path_directory(TEMP_DIR_NAME);

    if (file_exists) {
        long time1 = mvn_get_file_mod_time(TEMP_FILE_PATH);
        TEST_ASSERT_FMT(
            time1 >= 0, "mvn_get_file_mod_time returned negative for existing file: %ld", time1);

        SDL_Delay(5000); // Wait 2 seconds to ensure timestamp changes
        SDL_IOStream *file_mod = SDL_IOFromFile(TEMP_FILE_PATH, "a");
        if (file_mod != NULL) {
            const char *mod_content = " modified";
            size_t      written     = SDL_WriteIO(file_mod, mod_content, SDL_strlen(mod_content));
            if (written != SDL_strlen(mod_content)) {
                mvn_log_error("Failed to write modification for mod time test: %s", TEMP_FILE_PATH);
            }
            if (!SDL_CloseIO(file_mod)) {
                mvn_log_error("Failed to close modified file for mod time test: %s",
                              TEMP_FILE_PATH);
            }

            // Add a delay to ensure the file system records the modification time
            SDL_Delay(5000);

            long time2 = mvn_get_file_mod_time(TEMP_FILE_PATH);
            TEST_ASSERT_FMT(time2 >= time1,
                            "Modification time did not increase after modification: %ld",
                            time2);
        } else {
            TEST_ASSERT(false, "Failed to reopen temp file for modification");
        }
    } else {
        TEST_ASSERT(false, "Could not create temp file, skipping mod time tests");
    }

    long non_existent_time = mvn_get_file_mod_time("non_existent_file_for_time_test");
    TEST_ASSERT(non_existent_time == -1,
                "mvn_get_file_mod_time did not return -1 for non-existent file");

    // Clean up
    if (file_exists) {
        if (!SDL_RemovePath(TEMP_FILE_PATH)) {
            mvn_log_error("Failed to remove temp file: %s", SDL_GetError());
        }
    }
    if (dir_exists) {
        if (!SDL_RemovePath(TEMP_DIR_NAME)) {
            mvn_log_error("Failed to remove temp directory: %s", SDL_GetError());
        }
    }

    return 1;
}

/**
 * \brief           Run all file tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int run_file_tests(int *passed_tests, int *failed_tests, int *total_tests)
{
    printf("\n===== FILE TESTS =====\n\n");

    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_get_application_directory);
    RUN_TEST(test_is_path_file_directory);
    RUN_TEST(test_get_file_mod_time);

    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run;

    return *passed_tests - passed_before;
}

#if defined(MVN_FILE_TEST_MAIN)
int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    run_file_tests(&passed, &failed, &total);

    printf("\n===== FILE TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
#endif
