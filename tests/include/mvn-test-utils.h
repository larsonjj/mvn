#ifndef MVN_TEST_UTILS_H
#define MVN_TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Floating point comparison constants
#define FLOAT_EPSILON  1e-6f
#define DOUBLE_EPSILON 1e-14

// Basic assertion macro
#define TEST_ASSERT(condition, message)                                                            \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            printf("FAILED: %s\nLine: %d\n", message, __LINE__);                                   \
            return 0;                                                                              \
        }                                                                                          \
    } while (0)

// Floating point comparison macros
#define TEST_ASSERT_FLOAT_EQ(actual, expected, message)                                            \
    do {                                                                                           \
        if (fabsf((actual) - (expected)) > FLOAT_EPSILON) {                                        \
            printf("FAILED: %s\nExpected: %f, Got: %f\nLine: %d\n",                                \
                   message,                                                                        \
                   (double)(expected),                                                             \
                   (double)(actual),                                                               \
                   __LINE__);                                                                      \
            return 0;                                                                              \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_DOUBLE_EQ(actual, expected, message)                                           \
    do {                                                                                           \
        if (fabs((actual) - (expected)) > DOUBLE_EPSILON) {                                        \
            printf("FAILED: %s\nExpected: %.16f, Got: %.16f\nLine: %d\n",                          \
                   message,                                                                        \
                   (expected),                                                                     \
                   (actual),                                                                       \
                   __LINE__);                                                                      \
            return 0;                                                                              \
        }                                                                                          \
    } while (0)

// Test runner macro
// Note: This requires passed_tests, failed_tests, and total_tests variables to be defined in the
// scope
#define RUN_TEST(test_func)                                                                        \
    do {                                                                                           \
        printf("Running test: %s\n", #test_func);                                                  \
        if (test_func()) {                                                                         \
            printf("✅ PASSED: %s\n", #test_func);                                                  \
            (*passed_tests)++;                                                                     \
        } else {                                                                                   \
            printf("❌ FAILED: %s\n", #test_func);                                                  \
            (*failed_tests)++;                                                                     \
        }                                                                                          \
    } while (0)

// Convenience function to print test summary
static inline void print_test_summary(int total_tests, int passed_tests, int failed_tests)
{
    printf("Test summary:\n");
    printf("  Total tests: %d\n", total_tests);
    printf("  Passed: %d\n", passed_tests);
    printf("  Failed: %d\n", failed_tests);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_TEST_UTILS_H */
