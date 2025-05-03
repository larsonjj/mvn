/**
 * \file            mvn_test.c
 * \brief           Main test runner for MVN library
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */

#include "mvn-core-test.h"
#include "mvn-error-test.h"
#include "mvn-file-test.h"
#include "mvn-hashmap-test.h"
#include "mvn-list-test.h"
#include "mvn-logger-test.h"
#include "mvn-string-test.h"
#include "mvn-test-utils.h"
#include "mvn-text-test.h"
#include "mvn-texture-test.h"
#include "mvn-window-test.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * \brief           Main entry point for tests
 * \return          EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    printf("Starting MVN library tests...\n");

    // Run all test suites
    run_core_tests(&passed, &failed, &total);
    run_window_tests(&passed, &failed, &total);
    run_texture_tests(&passed, &failed, &total);
    run_hashmap_tests(&passed, &failed, &total);
    run_list_tests(&passed, &failed, &total);
    run_string_tests(&passed, &failed, &total);
    run_logger_tests(&passed, &failed, &total);
    run_text_tests(&passed, &failed, &total);
    run_error_tests(&passed, &failed, &total);

    // Ensure running file operations last as they can interfere with other tests
    run_file_tests(&passed, &failed, &total);

    // Print summary
    printf("\n===== TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}
