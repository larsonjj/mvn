// filepath: tests/source/mvn-string-test.c
/**
 * \file            mvn-string-test.c
 * \brief           Tests for MVN string functionality
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */

#include "mvn-test-utils.h"
#include "mvn/mvn-list.h" // Needed for split tests
#include "mvn/mvn-string.h"

#include <stdio.h>
#include <string.h>

/**
 * \brief           Test string initialization and freeing
 * \return          1 on success, 0 on failure
 */
static int test_string_init_free(void)
{
    // Test init with default capacity
    mvn_string_t *str1 = mvn_string_init(0);
    TEST_ASSERT(str1 != NULL, "Failed to initialize string with default capacity");
    TEST_ASSERT(mvn_string_length(str1) == 0, "New string should have length 0");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(str1), "") == 0, "New string data should be empty");
    mvn_string_free(str1);

    // Test init with specific capacity
    mvn_string_t *str2 = mvn_string_init(32);
    TEST_ASSERT(str2 != NULL, "Failed to initialize string with specific capacity");
    TEST_ASSERT(mvn_string_length(str2) == 0, "New string should have length 0");
    mvn_string_free(str2);

    // Test freeing NULL
    mvn_string_free(NULL); // Should not crash
    TEST_ASSERT(1, "Freeing NULL string did not crash");

    return 1;
}

/**
 * \brief           Test creating string from C string
 * \return          1 on success, 0 on failure
 */
static int test_string_from(void)
{
    const char   *cstr = "Hello, World!";
    mvn_string_t *str  = mvn_string_from_cstr(cstr);
    TEST_ASSERT(str != NULL, "Failed to create string from C string");
    TEST_ASSERT(mvn_string_length(str) == SDL_strlen(cstr), "String length is incorrect");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(str), cstr) == 0, "String data is incorrect");
    mvn_string_free(str);

    // Test from empty C string
    mvn_string_t *empty_str = mvn_string_from_cstr("");
    TEST_ASSERT(empty_str != NULL, "Failed to create string from empty C string");
    TEST_ASSERT(mvn_string_length(empty_str) == 0, "Empty string length should be 0");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(empty_str), "") == 0,
                "Empty string data should be empty");
    mvn_string_free(empty_str);

    // Test from NULL C string (should be treated as empty)
    mvn_string_t *null_str = mvn_string_from_cstr(NULL);
    TEST_ASSERT(null_str != NULL, "Failed to create string from NULL C string");
    TEST_ASSERT(mvn_string_length(null_str) == 0, "NULL C string length should be 0");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(null_str), "") == 0,
                "NULL C string data should be empty");
    mvn_string_free(null_str);

    return 1;
}

/**
 * \brief           Test string concatenation and appending
 * \return          1 on success, 0 on failure
 */
static int test_string_concat_append(void)
{
    mvn_string_t *str1  = mvn_string_from_cstr("Hello, ");
    mvn_string_t *str2  = mvn_string_from_cstr("World!");
    const char   *cstr3 = " How are you?";

    TEST_ASSERT(str1 != NULL && str2 != NULL, "Failed to create initial strings");

    // Test concatenation
    mvn_string_t *concat_str = mvn_string_concat(str1, str2);
    TEST_ASSERT(concat_str != NULL, "Failed to concatenate strings");
    TEST_ASSERT(mvn_string_length(concat_str) == 13, "Concatenated length is incorrect");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(concat_str), "Hello, World!") == 0,
                "Concatenated data is incorrect");

    // Test appending C string
    TEST_ASSERT(mvn_string_append(concat_str, cstr3), "Failed to append C string");
    TEST_ASSERT(mvn_string_length(concat_str) == 26, "Appended length is incorrect");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(concat_str), "Hello, World! How are you?") == 0,
                "Appended data is incorrect");

    // Test concat with NULL
    mvn_string_t *concat_null1 = mvn_string_concat(NULL, str2);
    TEST_ASSERT(concat_null1 != NULL, "Concat with first NULL failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(concat_null1), "World!") == 0,
                "Concat NULL data incorrect");
    mvn_string_free(concat_null1);

    mvn_string_t *concat_null2 = mvn_string_concat(str1, NULL);
    TEST_ASSERT(concat_null2 != NULL, "Concat with second NULL failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(concat_null2), "Hello, ") == 0,
                "Concat NULL data incorrect");
    mvn_string_free(concat_null2);

    // Test append NULL C string (should do nothing)
    size_t len_before = mvn_string_length(str1);
    TEST_ASSERT(mvn_string_append(str1, NULL) == false, "Append NULL C string should return false");
    TEST_ASSERT(mvn_string_length(str1) == len_before,
                "Length should not change after appending NULL");

    // Test append to NULL string
    TEST_ASSERT(mvn_string_append(NULL, "test") == false, "Append to NULL string should fail");

    mvn_string_free(str1);
    mvn_string_free(str2);
    mvn_string_free(concat_str);

    return 1;
}

/**
 * \brief           Test string starts_with and ends_with
 * \return          1 on success, 0 on failure
 */
static int test_string_starts_ends_with(void)
{
    mvn_string_t *str = mvn_string_from_cstr("Hello World");
    TEST_ASSERT(str != NULL, "Failed to create string");

    // starts_with
    TEST_ASSERT(mvn_string_starts_with(str, "Hello"), "starts_with 'Hello' failed");
    TEST_ASSERT(mvn_string_starts_with(str, "Hello World"), "starts_with full string failed");
    TEST_ASSERT(!mvn_string_starts_with(str, "hello"), "starts_with case sensitive failed");
    TEST_ASSERT(!mvn_string_starts_with(str, "World"), "starts_with incorrect prefix failed");
    TEST_ASSERT(mvn_string_starts_with(str, ""), "starts_with empty prefix should succeed");
    TEST_ASSERT(!mvn_string_starts_with(str, "Hello World More"),
                "starts_with longer prefix failed");
    TEST_ASSERT(!mvn_string_starts_with(NULL, "Hello"), "starts_with NULL string failed");
    TEST_ASSERT(!mvn_string_starts_with(str, NULL), "starts_with NULL prefix failed");

    // ends_with
    TEST_ASSERT(mvn_string_ends_with(str, "World"), "ends_with 'World' failed");
    TEST_ASSERT(mvn_string_ends_with(str, "Hello World"), "ends_with full string failed");
    TEST_ASSERT(!mvn_string_ends_with(str, "world"), "ends_with case sensitive failed");
    TEST_ASSERT(!mvn_string_ends_with(str, "Hello"), "ends_with incorrect suffix failed");
    TEST_ASSERT(mvn_string_ends_with(str, ""), "ends_with empty suffix should succeed");
    TEST_ASSERT(!mvn_string_ends_with(str, "More Hello World"), "ends_with longer suffix failed");
    TEST_ASSERT(!mvn_string_ends_with(NULL, "World"), "ends_with NULL string failed");
    TEST_ASSERT(!mvn_string_ends_with(str, NULL), "ends_with NULL suffix failed");

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string includes
 * \return          1 on success, 0 on failure
 */
static int test_string_includes(void)
{
    mvn_string_t *str = mvn_string_from_cstr("Hello World Wide Web");
    TEST_ASSERT(str != NULL, "Failed to create string");

    TEST_ASSERT(mvn_string_includes(str, "World"), "includes 'World' failed");
    TEST_ASSERT(mvn_string_includes(str, "Hello"), "includes 'Hello' failed");
    TEST_ASSERT(mvn_string_includes(str, "Web"), "includes 'Web' failed");
    TEST_ASSERT(mvn_string_includes(str, " "), "includes space failed");
    TEST_ASSERT(mvn_string_includes(str, "Hello World Wide Web"), "includes full string failed");
    TEST_ASSERT(mvn_string_includes(str, ""), "includes empty string should succeed");
    TEST_ASSERT(!mvn_string_includes(str, "world"), "includes case sensitive failed");
    TEST_ASSERT(!mvn_string_includes(str, "Universe"), "includes non-existent failed");
    TEST_ASSERT(!mvn_string_includes(str, "Hello World Wide Web More"), "includes longer failed");
    TEST_ASSERT(!mvn_string_includes(NULL, "World"), "includes NULL string failed");
    TEST_ASSERT(!mvn_string_includes(str, NULL), "includes NULL substring failed");

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string padding
 * \return          1 on success, 0 on failure
 */
static int test_string_padding(void)
{
    mvn_string_t *str = mvn_string_from_cstr("Pad");
    TEST_ASSERT(str != NULL, "Failed to create string");

    // pad_start
    mvn_string_t *padded_start = mvn_string_pad_start(str, 5, '*');
    TEST_ASSERT(padded_start != NULL, "pad_start failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(padded_start), "**Pad") == 0,
                "pad_start content incorrect");
    TEST_ASSERT(mvn_string_length(padded_start) == 5, "pad_start length incorrect");
    mvn_string_free(padded_start);

    mvn_string_t *padded_start_no_change = mvn_string_pad_start(str, 2, '*');
    TEST_ASSERT(padded_start_no_change != NULL, "pad_start no change failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(padded_start_no_change), "Pad") == 0,
                "pad_start no change content incorrect");
    mvn_string_free(padded_start_no_change);

    TEST_ASSERT(mvn_string_pad_start(NULL, 5, '*') == NULL, "pad_start NULL string failed");

    // pad_end
    mvn_string_t *padded_end = mvn_string_pad_end(str, 6, '-');
    TEST_ASSERT(padded_end != NULL, "pad_end failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(padded_end), "Pad---") == 0, "pad_end content incorrect");
    TEST_ASSERT(mvn_string_length(padded_end) == 6, "pad_end length incorrect");
    mvn_string_free(padded_end);

    mvn_string_t *padded_end_no_change = mvn_string_pad_end(str, 3, '-');
    TEST_ASSERT(padded_end_no_change != NULL, "pad_end no change failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(padded_end_no_change), "Pad") == 0,
                "pad_end no change content incorrect");
    mvn_string_free(padded_end_no_change);

    TEST_ASSERT(mvn_string_pad_end(NULL, 5, '-') == NULL, "pad_end NULL string failed");

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string repeat
 * \return          1 on success, 0 on failure
 */
static int test_string_repeat(void)
{
    mvn_string_t *str = mvn_string_from_cstr("Abc");
    TEST_ASSERT(str != NULL, "Failed to create string");

    mvn_string_t *repeated = mvn_string_repeat(str, 3);
    TEST_ASSERT(repeated != NULL, "repeat failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(repeated), "AbcAbcAbc") == 0, "repeat content incorrect");
    TEST_ASSERT(mvn_string_length(repeated) == 9, "repeat length incorrect");
    mvn_string_free(repeated);

    mvn_string_t *repeated_zero = mvn_string_repeat(str, 0);
    TEST_ASSERT(repeated_zero != NULL, "repeat zero failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(repeated_zero), "") == 0,
                "repeat zero content incorrect");
    TEST_ASSERT(mvn_string_length(repeated_zero) == 0, "repeat zero length incorrect");
    mvn_string_free(repeated_zero);

    mvn_string_t *repeated_one = mvn_string_repeat(str, 1);
    TEST_ASSERT(repeated_one != NULL, "repeat one failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(repeated_one), "Abc") == 0,
                "repeat one content incorrect");
    TEST_ASSERT(mvn_string_length(repeated_one) == 3, "repeat one length incorrect");
    mvn_string_free(repeated_one);

    TEST_ASSERT(mvn_string_repeat(NULL, 3) == NULL, "repeat NULL string failed");

    mvn_string_t *empty_str      = mvn_string_from_cstr("");
    mvn_string_t *repeated_empty = mvn_string_repeat(empty_str, 5);
    TEST_ASSERT(repeated_empty != NULL, "repeat empty string failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(repeated_empty), "") == 0,
                "repeat empty string content incorrect");
    mvn_string_free(repeated_empty);
    mvn_string_free(empty_str);

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string replace
 * \return          1 on success, 0 on failure
 */
static int test_string_replace(void)
{
    mvn_string_t *str = mvn_string_from_cstr("one two one three one");
    TEST_ASSERT(str != NULL, "Failed to create string");

    // replace_first
    mvn_string_t *replaced_first = mvn_string_replace(str, "one", "1");
    TEST_ASSERT(replaced_first != NULL, "replace_first failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(replaced_first), "1 two one three one") == 0,
                "replace_first content incorrect");
    mvn_string_free(replaced_first);

    mvn_string_t *replaced_first_no_match = mvn_string_replace(str, "four", "4");
    TEST_ASSERT(replaced_first_no_match != NULL, "replace_first no match failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(replaced_first_no_match), "one two one three one") == 0,
                "replace_first no match content incorrect");
    mvn_string_free(replaced_first_no_match);

    TEST_ASSERT(mvn_string_replace(NULL, "one", "1") == NULL, "replace_first NULL string failed");
    TEST_ASSERT(mvn_string_replace(str, NULL, "1") == NULL, "replace_first NULL search failed");
    TEST_ASSERT(mvn_string_replace(str, "one", NULL) == NULL, "replace_first NULL replace failed");

    // replace_all
    mvn_string_t *replaced_all = mvn_string_replace_all(str, "one", "1");
    TEST_ASSERT(replaced_all != NULL, "replace_all failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(replaced_all), "1 two 1 three 1") == 0,
                "replace_all content incorrect");
    mvn_string_free(replaced_all);

    mvn_string_t *replaced_all_longer = mvn_string_replace_all(str, "one", "first");
    TEST_ASSERT(replaced_all_longer != NULL, "replace_all longer failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(replaced_all_longer), "first two first three first") == 0,
                "replace_all longer content incorrect");
    mvn_string_free(replaced_all_longer);

    mvn_string_t *replaced_all_shorter = mvn_string_replace_all(str, "three", "3");
    TEST_ASSERT(replaced_all_shorter != NULL, "replace_all shorter failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(replaced_all_shorter), "one two one 3 one") == 0,
                "replace_all shorter content incorrect");
    mvn_string_free(replaced_all_shorter);

    mvn_string_t *replaced_all_empty_search = mvn_string_replace_all(str, "", "X");
    TEST_ASSERT(replaced_all_empty_search != NULL, "replace_all empty search failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(replaced_all_empty_search), "one two one three one") == 0,
                "replace_all empty search content incorrect"); // Should not replace anything
    mvn_string_free(replaced_all_empty_search);

    mvn_string_t *replaced_all_empty_replace = mvn_string_replace_all(str, " ", "");
    TEST_ASSERT(replaced_all_empty_replace != NULL, "replace_all empty replace failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(replaced_all_empty_replace), "onetwoonethreeone") == 0,
                "replace_all empty replace content incorrect");
    mvn_string_free(replaced_all_empty_replace);

    TEST_ASSERT(mvn_string_replace_all(NULL, "one", "1") == NULL, "replace_all NULL string failed");
    TEST_ASSERT(mvn_string_replace_all(str, NULL, "1") == NULL, "replace_all NULL search failed");
    TEST_ASSERT(mvn_string_replace_all(str, "one", NULL) == NULL,
                "replace_all NULL replace failed");

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string split
 * \return          1 on success, 0 on failure
 */
static int test_string_split(void)
{
    mvn_string_t *str = mvn_string_from_cstr("apple,banana,orange");
    TEST_ASSERT(str != NULL, "Failed to create string");

    mvn_list_t *list = mvn_string_split(str, ",");
    TEST_ASSERT(list != NULL, "split failed");
    TEST_ASSERT(mvn_list_length(list) == 3, "split list length incorrect");

    if (list && mvn_list_length(list) == 3) {
        mvn_string_t **item0 = MVN_LIST_GET(mvn_string_t *, list, 0);
        mvn_string_t **item1 = MVN_LIST_GET(mvn_string_t *, list, 1);
        mvn_string_t **item2 = MVN_LIST_GET(mvn_string_t *, list, 2);
        TEST_ASSERT(item0 && *item0 && strcmp(mvn_string_to_cstr(*item0), "apple") == 0,
                    "split item 0 incorrect");
        TEST_ASSERT(item1 && *item1 && strcmp(mvn_string_to_cstr(*item1), "banana") == 0,
                    "split item 1 incorrect");
        TEST_ASSERT(item2 && *item2 && strcmp(mvn_string_to_cstr(*item2), "orange") == 0,
                    "split item 2 incorrect");
    }
    // Free the list and the strings inside
    if (list) {
        for (size_t i = 0; i < mvn_list_length(list); ++i) {
            mvn_string_t **item = MVN_LIST_GET(mvn_string_t *, list, i);
            if (item && *item) {
                mvn_string_free(*item);
            }
        }
        mvn_list_free(list);
    }

    // Test split with delimiter at start/end/multiple
    mvn_string_t *str2  = mvn_string_from_cstr(",a,,b,");
    mvn_list_t   *list2 = mvn_string_split(str2, ",");
    TEST_ASSERT(list2 != NULL, "split complex failed");
    TEST_ASSERT(mvn_list_length(list2) == 5,
                "split complex list length incorrect"); // "", "a", "", "b", ""
    if (list2 && mvn_list_length(list2) == 5) {
        mvn_string_t **item0 = MVN_LIST_GET(mvn_string_t *, list2, 0);
        mvn_string_t **item1 = MVN_LIST_GET(mvn_string_t *, list2, 1);
        mvn_string_t **item2 = MVN_LIST_GET(mvn_string_t *, list2, 2);
        mvn_string_t **item3 = MVN_LIST_GET(mvn_string_t *, list2, 3);
        mvn_string_t **item4 = MVN_LIST_GET(mvn_string_t *, list2, 4);
        TEST_ASSERT(item0 && *item0 && strcmp(mvn_string_to_cstr(*item0), "") == 0,
                    "split complex item 0 incorrect");
        TEST_ASSERT(item1 && *item1 && strcmp(mvn_string_to_cstr(*item1), "a") == 0,
                    "split complex item 1 incorrect");
        TEST_ASSERT(item2 && *item2 && strcmp(mvn_string_to_cstr(*item2), "") == 0,
                    "split complex item 2 incorrect");
        TEST_ASSERT(item3 && *item3 && strcmp(mvn_string_to_cstr(*item3), "b") == 0,
                    "split complex item 3 incorrect");
        TEST_ASSERT(item4 && *item4 && strcmp(mvn_string_to_cstr(*item4), "") == 0,
                    "split complex item 4 incorrect");
    }
    if (list2) {
        for (size_t i = 0; i < mvn_list_length(list2); ++i) {
            mvn_string_t **item = MVN_LIST_GET(mvn_string_t *, list2, i);
            if (item && *item) {
                mvn_string_free(*item);
            }
        }
        mvn_list_free(list2);
    }
    mvn_string_free(str2);

    // Test split with no delimiter
    mvn_string_t *str3  = mvn_string_from_cstr("noddelimiter");
    mvn_list_t   *list3 = mvn_string_split(str3, ",");
    TEST_ASSERT(list3 != NULL, "split no delimiter failed");
    TEST_ASSERT(mvn_list_length(list3) == 1, "split no delimiter length incorrect");
    if (list3 && mvn_list_length(list3) == 1) {
        mvn_string_t **item0 = MVN_LIST_GET(mvn_string_t *, list3, 0);
        TEST_ASSERT(item0 && *item0 && strcmp(mvn_string_to_cstr(*item0), "noddelimiter") == 0,
                    "split no delimiter item 0 incorrect");
    }
    if (list3) {
        for (size_t i = 0; i < mvn_list_length(list3); ++i) {
            mvn_string_t **item = MVN_LIST_GET(mvn_string_t *, list3, i);
            if (item && *item) {
                mvn_string_free(*item);
            }
        }
        mvn_list_free(list3);
    }
    mvn_string_free(str3);

    // Test split empty string
    mvn_string_t *empty_str  = mvn_string_from_cstr("");
    mvn_list_t   *empty_list = mvn_string_split(empty_str, ",");
    TEST_ASSERT(empty_list != NULL, "split empty string failed");
    TEST_ASSERT(mvn_list_length(empty_list) == 1,
                "split empty string length incorrect"); // Should contain one empty string
    if (empty_list && mvn_list_length(empty_list) == 1) {
        mvn_string_t **item0 = MVN_LIST_GET(mvn_string_t *, empty_list, 0);
        TEST_ASSERT(item0 && *item0 && strcmp(mvn_string_to_cstr(*item0), "") == 0,
                    "split empty string item 0 incorrect");
    }
    if (empty_list) {
        for (size_t i = 0; i < mvn_list_length(empty_list); ++i) {
            mvn_string_t **item = MVN_LIST_GET(mvn_string_t *, empty_list, i);
            if (item && *item) {
                mvn_string_free(*item);
            }
        }
        mvn_list_free(empty_list);
    }
    mvn_string_free(empty_str);

    TEST_ASSERT(mvn_string_split(NULL, ",") == NULL, "split NULL string failed");

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string case conversion
 * \return          1 on success, 0 on failure
 */
static int test_string_case_conversion(void)
{
    mvn_string_t *str = mvn_string_from_cstr("Hello World 123");
    TEST_ASSERT(str != NULL, "Failed to create string");

    // to_lower
    mvn_string_t *lower = mvn_string_to_lowercase(str);
    TEST_ASSERT(lower != NULL, "to_lower failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(lower), "hello world 123") == 0,
                "to_lower content incorrect");
    mvn_string_free(lower);
    TEST_ASSERT(mvn_string_to_lowercase(NULL) == NULL, "to_lower NULL string failed");

    // to_upper
    mvn_string_t *upper = mvn_string_to_uppercase(str);
    TEST_ASSERT(upper != NULL, "to_upper failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(upper), "HELLO WORLD 123") == 0,
                "to_upper content incorrect");
    mvn_string_free(upper);
    TEST_ASSERT(mvn_string_to_uppercase(NULL) == NULL, "to_upper NULL string failed");

    mvn_string_t *empty_str   = mvn_string_from_cstr("");
    mvn_string_t *lower_empty = mvn_string_to_lowercase(empty_str);
    TEST_ASSERT(lower_empty != NULL && mvn_string_length(lower_empty) == 0,
                "to_lower empty failed");
    mvn_string_free(lower_empty);
    mvn_string_t *upper_empty = mvn_string_to_uppercase(empty_str);
    TEST_ASSERT(upper_empty != NULL && mvn_string_length(upper_empty) == 0,
                "to_upper empty failed");
    mvn_string_free(upper_empty);
    mvn_string_free(empty_str);

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string trimming
 * \return          1 on success, 0 on failure
 */
static int test_string_trimming(void)
{
    mvn_string_t *str = mvn_string_from_cstr("   Hello World   ");
    TEST_ASSERT(str != NULL, "Failed to create string");

    // trim_start
    mvn_string_t *trimmed_start = mvn_string_trim_start(str);
    TEST_ASSERT(trimmed_start != NULL, "trim_start failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(trimmed_start), "Hello World   ") == 0,
                "trim_start content incorrect");
    mvn_string_free(trimmed_start);
    TEST_ASSERT(mvn_string_trim_start(NULL) == NULL, "trim_start NULL string failed");

    // trim_end
    mvn_string_t *trimmed_end = mvn_string_trim_end(str);
    TEST_ASSERT(trimmed_end != NULL, "trim_end failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(trimmed_end), "   Hello World") == 0,
                "trim_end content incorrect");
    mvn_string_free(trimmed_end);
    TEST_ASSERT(mvn_string_trim_end(NULL) == NULL, "trim_end NULL string failed");

    // trim
    mvn_string_t *trimmed = mvn_string_trim(str);
    TEST_ASSERT(trimmed != NULL, "trim failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(trimmed), "Hello World") == 0, "trim content incorrect");
    mvn_string_free(trimmed);
    TEST_ASSERT(mvn_string_trim(NULL) == NULL, "trim NULL string failed");

    // Test trimming strings with no whitespace
    mvn_string_t *no_space         = mvn_string_from_cstr("NoSpace");
    mvn_string_t *trimmed_no_space = mvn_string_trim(no_space);
    TEST_ASSERT(trimmed_no_space != NULL &&
                    strcmp(mvn_string_to_cstr(trimmed_no_space), "NoSpace") == 0,
                "trim no space failed");
    mvn_string_free(trimmed_no_space);
    mvn_string_free(no_space);

    // Test trimming strings with only whitespace
    mvn_string_t *only_space         = mvn_string_from_cstr(" \t\n\r ");
    mvn_string_t *trimmed_only_space = mvn_string_trim(only_space);
    TEST_ASSERT(trimmed_only_space != NULL &&
                    strcmp(mvn_string_to_cstr(trimmed_only_space), "") == 0,
                "trim only space failed");
    mvn_string_free(trimmed_only_space);
    mvn_string_free(only_space);

    // Test trimming empty string
    mvn_string_t *empty_str     = mvn_string_from_cstr("");
    mvn_string_t *trimmed_empty = mvn_string_trim(empty_str);
    TEST_ASSERT(trimmed_empty != NULL && strcmp(mvn_string_to_cstr(trimmed_empty), "") == 0,
                "trim empty failed");
    mvn_string_free(trimmed_empty);
    mvn_string_free(empty_str);

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test string substring
 * \return          1 on success, 0 on failure
 */
static int test_string_substring(void)
{
    mvn_string_t *str = mvn_string_from_cstr("Substring Test");
    TEST_ASSERT(str != NULL, "Failed to create string");

    // Valid substring
    mvn_string_t *sub1 = mvn_string_substring(str, 10, 4); // "Test"
    TEST_ASSERT(sub1 != NULL, "substring 'Test' failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(sub1), "Test") == 0,
                "substring 'Test' content incorrect");
    mvn_string_free(sub1);

    // Substring from start
    mvn_string_t *sub2 = mvn_string_substring(str, 0, 9); // "Substring"
    TEST_ASSERT(sub2 != NULL, "substring 'Substring' failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(sub2), "Substring") == 0,
                "substring 'Substring' content incorrect");
    mvn_string_free(sub2);

    // Substring to end
    mvn_string_t *sub3 = mvn_string_substring(str, 10, 100); // "Test" (length clamped)
    TEST_ASSERT(sub3 != NULL, "substring to end failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(sub3), "Test") == 0,
                "substring to end content incorrect");
    mvn_string_free(sub3);

    // Substring zero length
    mvn_string_t *sub4 = mvn_string_substring(str, 5, 0); // ""
    TEST_ASSERT(sub4 != NULL, "substring zero length failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(sub4), "") == 0,
                "substring zero length content incorrect");
    mvn_string_free(sub4);

    // Invalid start index
    mvn_string_t *sub5 =
        mvn_string_substring(str, 20, 4); // Should return empty string or NULL? Let's assume empty
    TEST_ASSERT(sub5 != NULL, "substring invalid start failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(sub5), "") == 0,
                "substring invalid start content incorrect");
    mvn_string_free(sub5);

    // Negative start index (should be treated as 0?) - Assuming it returns empty for now
    // If the API allows negative indices from end, this test needs adjustment.
    // Based on typical C APIs, negative index is likely invalid or treated as 0.
    // Let's assume invalid -> empty string.
    mvn_string_t *sub6 = mvn_string_substring(str, -2, 4);
    TEST_ASSERT(sub6 != NULL, "substring negative start failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(sub6), "") == 0,
                "substring negative start content incorrect");
    mvn_string_free(sub6);

    // NULL string
    TEST_ASSERT(mvn_string_substring(NULL, 0, 5) == NULL, "substring NULL string failed");

    mvn_string_free(str);
    return 1;
}

/**
 * \brief           Test various edge cases
 * \return          1 on success, 0 on failure
 */
static int test_string_edge_cases(void)
{
    // Most edge cases (NULL inputs, empty strings) are tested within
    // the specific function tests above. This function can be used
    // for any additional complex or combined edge cases if needed.

    // Example: Operations on a string after modification
    mvn_string_t *str = mvn_string_from_cstr("Initial");
    TEST_ASSERT(str != NULL, "Edge case: Initial creation failed");

    mvn_string_append(str, " Appended");
    TEST_ASSERT(str != NULL, "Edge case: Append failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(str), "Initial Appended") == 0,
                "Edge case: Append content incorrect");

    mvn_string_t *sub = mvn_string_substring(str, 8, 8); // "Appended"
    TEST_ASSERT(sub != NULL, "Edge case: Substring after append failed");
    TEST_ASSERT(strcmp(mvn_string_to_cstr(sub), "Appended") == 0,
                "Edge case: Substring content incorrect");

    mvn_string_free(sub);
    mvn_string_free(str); // appended is the same as str after append_cstr

    return 1;
}

/**
 * \brief           Test string compare
 * \return          1 on success, 0 on failure
 */
static int test_string_compare(void)
{
    mvn_string_t *str1   = mvn_string_from_cstr("Hello");
    mvn_string_t *str2   = mvn_string_from_cstr("Hello");
    mvn_string_t *str3   = mvn_string_from_cstr("World");
    mvn_string_t *str4   = mvn_string_from_cstr("Hello!");
    mvn_string_t *empty1 = mvn_string_from_cstr("");
    mvn_string_t *empty2 = mvn_string_from_cstr("");

    TEST_ASSERT(mvn_string_compare(str1, str2), "Identical strings should compare equal");
    TEST_ASSERT(!mvn_string_compare(str1, str3), "Different strings should not compare equal");
    TEST_ASSERT(!mvn_string_compare(str1, str4),
                "Strings with different length should not compare equal");
    TEST_ASSERT(mvn_string_compare(empty1, empty2), "Empty strings should compare equal");
    TEST_ASSERT(!mvn_string_compare(str1, NULL), "Compare with NULL should return false");
    TEST_ASSERT(!mvn_string_compare(NULL, str2), "Compare with NULL should return false");
    TEST_ASSERT(!mvn_string_compare(NULL, NULL), "Compare NULL with NULL should return false");

    mvn_string_free(str1);
    mvn_string_free(str2);
    mvn_string_free(str3);
    mvn_string_free(str4);
    mvn_string_free(empty1);
    mvn_string_free(empty2);

    return 1;
}

/**
 * \brief           Run all string tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int run_string_tests(int *passed_tests, int *failed_tests, int *total_tests)
{
    printf("\n===== STRING TESTS =====\n\n");

    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_string_init_free);
    RUN_TEST(test_string_from);
    RUN_TEST(test_string_concat_append);
    RUN_TEST(test_string_starts_ends_with);
    RUN_TEST(test_string_includes);
    RUN_TEST(test_string_padding);
    RUN_TEST(test_string_repeat);
    RUN_TEST(test_string_replace);
    RUN_TEST(test_string_split);
    RUN_TEST(test_string_case_conversion);
    RUN_TEST(test_string_trimming);
    RUN_TEST(test_string_substring);
    RUN_TEST(test_string_compare);
    RUN_TEST(test_string_edge_cases);

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run;

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
}

#if defined(MVN_STRING_TEST_MAIN)
int main(void)
{
    int passed = 0;
    int failed = 0;
    int total  = 0;

    run_string_tests(&passed, &failed, &total);

    printf("\n===== STRING TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
#endif
