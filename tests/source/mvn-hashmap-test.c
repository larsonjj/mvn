/**
 * \file            mvn-hashmap-test.c
 * \brief           Tests for MVN hashmap functionality
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */

#include <stdio.h>
#include <string.h>

#include "mvn-test-utils.h"
#include "mvn/mvn-hashmap.h"
#include "mvn/mvn-types.h"

/**
 * \brief           Test basic hashmap initialization and freeing
 * \return          1 on success, 0 on failure
 */
static int
test_hashmap_init(void) {
    // Test with default capacity (0)
    mvn_hmap_t* hmap1 = mvn_hmap_init(sizeof(int), 0);
    TEST_ASSERT(hmap1 != NULL, "Failed to initialize hashmap with default capacity");
    TEST_ASSERT(mvn_hmap_length(hmap1) == 0, "New hashmap should have length 0");
    mvn_hmap_free(hmap1);

    // Test with specific capacity
    mvn_hmap_t* hmap2 = mvn_hmap_init(sizeof(int), 16);
    TEST_ASSERT(hmap2 != NULL, "Failed to initialize hashmap with specific capacity");
    TEST_ASSERT(mvn_hmap_length(hmap2) == 0, "New hashmap should have length 0");
    mvn_hmap_free(hmap2);

    // Test with type-safe macro
    mvn_hmap_t* hmap3 = MVN_HMAP_INIT(mvn_point_t, 8);
    TEST_ASSERT(hmap3 != NULL, "Failed to initialize hashmap with type-safe macro");
    mvn_hmap_free(hmap3);

    // Test with invalid parameters
    mvn_hmap_t* hmap4 = mvn_hmap_init(0, 10);
    TEST_ASSERT(hmap4 == NULL, "Hashmap initialization should fail with item_size = 0");

    return 1;
}

/**
 * \brief           Test setting and getting values in the hashmap
 * \return          1 on success, 0 on failure
 */
static int
test_hashmap_set_get(void) {
    // Initialize hashmap for integers
    mvn_hmap_t* hmap = MVN_HMAP_INIT(int, 8);
    TEST_ASSERT(hmap != NULL, "Failed to initialize hashmap");

    // Test setting and getting simple values
    int value1 = 42;
    int value2 = 100;
    int value3 = -10;

    TEST_ASSERT(mvn_hmap_set(hmap, "key1", &value1), "Failed to set key1");
    TEST_ASSERT(mvn_hmap_set(hmap, "key2", &value2), "Failed to set key2");
    TEST_ASSERT(mvn_hmap_set(hmap, "key3", &value3), "Failed to set key3");

    TEST_ASSERT(mvn_hmap_length(hmap) == 3, "Hashmap should have 3 items");

    int* retrieved1 = MVN_HMAP_GET(int, hmap, "key1");
    int* retrieved2 = MVN_HMAP_GET(int, hmap, "key2");
    int* retrieved3 = MVN_HMAP_GET(int, hmap, "key3");
    int* not_found = MVN_HMAP_GET(int, hmap, "nonexistent");

    TEST_ASSERT(retrieved1 != NULL, "Failed to get key1");
    TEST_ASSERT(retrieved2 != NULL, "Failed to get key2");
    TEST_ASSERT(retrieved3 != NULL, "Failed to get key3");
    TEST_ASSERT(not_found == NULL, "Non-existent key should return NULL");

    TEST_ASSERT(*retrieved1 == 42, "Retrieved value for key1 is incorrect");
    TEST_ASSERT(*retrieved2 == 100, "Retrieved value for key2 is incorrect");
    TEST_ASSERT(*retrieved3 == -10, "Retrieved value for key3 is incorrect");

    // Test updating an existing key
    int updated_value = 999;
    TEST_ASSERT(mvn_hmap_set(hmap, "key1", &updated_value), "Failed to update key1");
    retrieved1 = MVN_HMAP_GET(int, hmap, "key1");
    TEST_ASSERT(retrieved1 != NULL, "Failed to get updated key1");
    TEST_ASSERT(*retrieved1 == 999, "Retrieved value for updated key1 is incorrect");
    TEST_ASSERT(mvn_hmap_length(hmap) == 3, "Hashmap length should not change after update");

    mvn_hmap_free(hmap);

    return 1;
}

/**
 * \brief           Test deleting items from the hashmap
 * \return          1 on success, 0 on failure
 */
static int
test_hashmap_delete(void) {
    mvn_hmap_t* hmap = MVN_HMAP_INIT(int, 8);
    TEST_ASSERT(hmap != NULL, "Failed to initialize hashmap");

    // Add some items
    int value1 = 42;
    int value2 = 100;
    int value3 = -10;

    TEST_ASSERT(mvn_hmap_set(hmap, "key1", &value1), "Failed to set key1");
    TEST_ASSERT(mvn_hmap_set(hmap, "key2", &value2), "Failed to set key2");
    TEST_ASSERT(mvn_hmap_set(hmap, "key3", &value3), "Failed to set key3");
    TEST_ASSERT(mvn_hmap_length(hmap) == 3, "Hashmap should have 3 items");

    // Test deleting a key
    TEST_ASSERT(mvn_hmap_delete(hmap, "key2"), "Failed to delete key2");
    TEST_ASSERT(mvn_hmap_length(hmap) == 2, "Hashmap should have 2 items after deletion");
    TEST_ASSERT(MVN_HMAP_GET(int, hmap, "key2") == NULL, "Deleted key should return NULL");

    // Test deleting a non-existent key
    TEST_ASSERT(mvn_hmap_delete(hmap, "nonexistent") == false,
                "Deleting nonexistent key should return false");
    TEST_ASSERT(mvn_hmap_length(hmap) == 2,
                "Hashmap length should not change after failed deletion");

    // Test that other keys are still accessible
    int* retrieved1 = MVN_HMAP_GET(int, hmap, "key1");
    int* retrieved3 = MVN_HMAP_GET(int, hmap, "key3");
    TEST_ASSERT(retrieved1 != NULL && *retrieved1 == 42, "key1 should still be accessible");
    TEST_ASSERT(retrieved3 != NULL && *retrieved3 == -10, "key3 should still be accessible");

    // Delete all remaining items
    TEST_ASSERT(mvn_hmap_delete(hmap, "key1"), "Failed to delete key1");
    TEST_ASSERT(mvn_hmap_delete(hmap, "key3"), "Failed to delete key3");
    TEST_ASSERT(mvn_hmap_length(hmap) == 0, "Hashmap should be empty after deleting all items");

    mvn_hmap_free(hmap);

    return 1;
}

/**
 * \brief           Test keys and values collections
 * \return          1 on success, 0 on failure
 */
static int
test_hashmap_keys_values(void) {
    mvn_hmap_t* hmap = MVN_HMAP_INIT(int, 8);
    TEST_ASSERT(hmap != NULL, "Failed to initialize hashmap");

    // Add some items with predictable values
    int values[5] = {10, 20, 30, 40, 50};
    const char* keys[5] = {"key1", "key2", "key3", "key4", "key5"};

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(mvn_hmap_set(hmap, keys[i], &values[i]), "Failed to set key");
    }

    // Test keys collection
    mvn_list_t* key_list = mvn_hmap_keys(hmap);
    TEST_ASSERT(key_list != NULL, "Failed to get keys list");
    TEST_ASSERT(mvn_list_length(key_list) == 5, "Keys list should have 5 items");

    // Verify all keys are present (order may vary)
    bool keys_found[5] = {false};
    for (size_t i = 0; i < mvn_list_length(key_list); i++) {
        char** key_ptr = (char**)mvn_list_get(key_list, i);
        TEST_ASSERT(key_ptr != NULL, "Failed to get key from list");

        const char* key = *key_ptr;
        for (int j = 0; j < 5; j++) {
            if (strcmp(key, keys[j]) == 0) {
                keys_found[j] = true;
                break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(keys_found[i], "Not all keys were found in keys list");
    }

    // Test values collection
    mvn_list_t* value_list = mvn_hmap_values(hmap);
    TEST_ASSERT(value_list != NULL, "Failed to get values list");
    TEST_ASSERT(mvn_list_length(value_list) == 5, "Values list should have 5 items");

    // Verify all values are present (order may vary)
    bool values_found[5] = {false};
    for (size_t i = 0; i < mvn_list_length(value_list); i++) {
        int* value = (int*)mvn_list_get(value_list, i);
        TEST_ASSERT(value != NULL, "Failed to get value from list");

        for (int j = 0; j < 5; j++) {
            if (*value == values[j]) {
                values_found[j] = true;
                break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(values_found[i], "Not all values were found in values list");
    }

    // Clean up
    mvn_list_free(key_list);
    mvn_list_free(value_list);
    mvn_hmap_free(hmap);

    return 1;
}

/**
 * \brief           Test complex data structures in hashmap
 * \return          1 on success, 0 on failure
 */
static int
test_hashmap_complex_types(void) {
    // Test with Points
    mvn_hmap_t* point_map = MVN_HMAP_INIT(mvn_point_t, 8);
    TEST_ASSERT(point_map != NULL, "Failed to initialize point hashmap");

    mvn_point_t pt1 = {10, 20};
    mvn_point_t pt2 = {-5, 30};

    TEST_ASSERT(mvn_hmap_set(point_map, "point1", &pt1), "Failed to set point1");
    TEST_ASSERT(mvn_hmap_set(point_map, "point2", &pt2), "Failed to set point2");

    mvn_point_t* retrieved_p1 = MVN_HMAP_GET(mvn_point_t, point_map, "point1");
    mvn_point_t* retrieved_p2 = MVN_HMAP_GET(mvn_point_t, point_map, "point2");

    TEST_ASSERT(retrieved_p1 != NULL, "Failed to get point1");
    TEST_ASSERT(retrieved_p2 != NULL, "Failed to get point2");
    TEST_ASSERT(retrieved_p1->x == 10 && retrieved_p1->y == 20,
                "Retrieved point1 has incorrect values");
    TEST_ASSERT(retrieved_p2->x == -5 && retrieved_p2->y == 30,
                "Retrieved point2 has incorrect values");

    mvn_hmap_free(point_map);

    // Test with Colors
    mvn_hmap_t* color_map = MVN_HMAP_INIT(mvn_color_t, 8);
    TEST_ASSERT(color_map != NULL, "Failed to initialize color hashmap");

    mvn_color_t red = {1, 0, 0, 1};
    mvn_color_t blue = {0, 0, 1, 1};
    mvn_color_t transparent_green = {0, 1, 0, 125};

    TEST_ASSERT(mvn_hmap_set(color_map, "red", &red), "Failed to set red");
    TEST_ASSERT(mvn_hmap_set(color_map, "blue", &blue), "Failed to set blue");
    TEST_ASSERT(mvn_hmap_set(color_map, "transparent_green", &transparent_green),
                "Failed to set transparent_green");

    mvn_color_t* retrieved_red = MVN_HMAP_GET(mvn_color_t, color_map, "red");
    mvn_color_t* retrieved_blue = MVN_HMAP_GET(mvn_color_t, color_map, "blue");
    mvn_color_t* retrieved_green = MVN_HMAP_GET(mvn_color_t, color_map, "transparent_green");

    TEST_ASSERT(retrieved_red != NULL, "Failed to get red");
    TEST_ASSERT(retrieved_blue != NULL, "Failed to get blue");
    TEST_ASSERT(retrieved_green != NULL, "Failed to get transparent_green");

    TEST_ASSERT(retrieved_red->r == 1 && retrieved_red->g == 0 && retrieved_red->b == 0
                    && retrieved_red->a == 1,
                "Retrieved red has incorrect values");

    TEST_ASSERT(retrieved_blue->r == 0 && retrieved_blue->g == 0 && retrieved_blue->b == 1
                    && retrieved_blue->a == 1,
                "Retrieved blue has incorrect values");

    TEST_ASSERT(retrieved_green->r == 0 && retrieved_green->g == 1 && retrieved_green->b == 0
                    && retrieved_green->a == 125,
                "Retrieved transparent_green has incorrect values");

    mvn_hmap_free(color_map);

    return 1;
}

/**
 * \brief           Test edge cases
 * \return          1 on success, 0 on failure
 */
static int
test_hashmap_edge_cases(void) {
    mvn_hmap_t* hmap = MVN_HMAP_INIT(int, 2); // Small initial capacity to test resizing
    TEST_ASSERT(hmap != NULL, "Failed to initialize hashmap");

    // Test NULL parameters
    int test_value = 1;

    // These will log errors, but that's expected behavior
    TEST_ASSERT(mvn_hmap_set(NULL, "key", &test_value) == false,
                "mvn_hmap_set with NULL hmap should return false");
    TEST_ASSERT(mvn_hmap_set(hmap, NULL, &test_value) == false,
                "mvn_hmap_set with NULL key should return false");
    TEST_ASSERT(mvn_hmap_set(hmap, "key", NULL) == false,
                "mvn_hmap_set with NULL value should return false");

    TEST_ASSERT(mvn_hmap_get(NULL, "key") == NULL,
                "mvn_hmap_get with NULL hmap should return NULL");
    TEST_ASSERT(mvn_hmap_get(hmap, NULL) == NULL, "mvn_hmap_get with NULL key should return NULL");

    TEST_ASSERT(mvn_hmap_delete(NULL, "key") == false,
                "mvn_hmap_delete with NULL hmap should return false");
    TEST_ASSERT(mvn_hmap_delete(hmap, NULL) == false,
                "mvn_hmap_delete with NULL key should return false");

    TEST_ASSERT(mvn_hmap_keys(NULL) == NULL, "mvn_hmap_keys with NULL hmap should return NULL");
    TEST_ASSERT(mvn_hmap_values(NULL) == NULL, "mvn_hmap_values with NULL hmap should return NULL");

    TEST_ASSERT(mvn_hmap_length(NULL) == 0, "mvn_hmap_length with NULL hmap should return 0");

    // Test inserting many items to trigger resizing
    const int NUM_ITEMS = 50; // Should trigger at least one resize with initial capacity 2
    char keys[NUM_ITEMS][20];
    int values[NUM_ITEMS];

    for (int i = 0; i < NUM_ITEMS; i++) {
        snprintf(keys[i], sizeof(keys[i]), "key%d", i);
        values[i] = i * 10;
        TEST_ASSERT(mvn_hmap_set(hmap, keys[i], &values[i]),
                    "Failed to set key during resize test");
    }

    TEST_ASSERT(mvn_hmap_length(hmap) == NUM_ITEMS,
                "Hashmap should contain all items after resizing");

    // Verify all items are accessible
    for (int i = 0; i < NUM_ITEMS; i++) {
        int* value = MVN_HMAP_GET(int, hmap, keys[i]);
        TEST_ASSERT(value != NULL, "Failed to get item after resize");
        TEST_ASSERT(*value == i * 10, "Retrieved value is incorrect after resize");
    }

    // Test empty hashmap behavior
    mvn_hmap_t* empty_map = MVN_HMAP_INIT(int, 8);
    TEST_ASSERT(empty_map != NULL, "Failed to initialize empty hashmap");

    TEST_ASSERT(mvn_hmap_length(empty_map) == 0, "Empty hashmap should have length 0");
    TEST_ASSERT(MVN_HMAP_GET(int, empty_map, "any_key") == NULL,
                "Getting from empty hashmap should return NULL");
    TEST_ASSERT(mvn_hmap_delete(empty_map, "any_key") == false,
                "Deleting from empty hashmap should return false");

    mvn_list_t* empty_keys = mvn_hmap_keys(empty_map);
    mvn_list_t* empty_values = mvn_hmap_values(empty_map);

    TEST_ASSERT(empty_keys != NULL, "mvn_hmap_keys should return an empty list, not NULL");
    TEST_ASSERT(empty_values != NULL, "mvn_hmap_values should return an empty list, not NULL");
    TEST_ASSERT(mvn_list_length(empty_keys) == 0, "Keys list from empty hashmap should be empty");
    TEST_ASSERT(mvn_list_length(empty_values) == 0,
                "Values list from empty hashmap should be empty");

    mvn_list_free(empty_keys);
    mvn_list_free(empty_values);
    mvn_hmap_free(empty_map);
    mvn_hmap_free(hmap);

    return 1;
}

/**
 * \brief           Run all hashmap tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int
run_hashmap_tests(int* passed_tests, int* failed_tests, int* total_tests) {
    printf("\n===== HASHMAP TESTS =====\n\n");

    // Store the initial values to calculate the delta
    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_hashmap_init);
    RUN_TEST(test_hashmap_set_get);
    RUN_TEST(test_hashmap_delete);
    RUN_TEST(test_hashmap_keys_values);
    RUN_TEST(test_hashmap_complex_types);
    RUN_TEST(test_hashmap_edge_cases);

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run; // Add to total tests count

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
}

#if defined(MVN_HASHMAP_TEST_MAIN)
int
main(void) {
    int passed = 0;
    int failed = 0;
    int total = 0;

    run_hashmap_tests(&passed, &failed, &total);

    printf("\n===== HASHMAP TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
#endif
