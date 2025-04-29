/**
 * \file            mvn-list-test.c
 * \brief           Tests for MVN list functionality
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
#include "mvn/mvn-list.h"
#include "mvn/mvn-types.h"

/**
 * \brief           Test basic list initialization and freeing
 * \return          1 on success, 0 on failure
 */
static int
test_list_init(void) {
    // Test with default capacity (0)
    mvn_list_t* list1 = MVN_LIST_INIT(int, 0);
    TEST_ASSERT(list1 != NULL, "Failed to initialize list with default capacity");
    TEST_ASSERT(mvn_list_length(list1) == 0, "New list should have length 0");
    mvn_list_free(list1);

    // Test with specific capacity
    mvn_list_t* list2 = MVN_LIST_INIT(int, 16);
    TEST_ASSERT(list2 != NULL, "Failed to initialize list with specific capacity");
    TEST_ASSERT(mvn_list_length(list2) == 0, "New list should have length 0");
    mvn_list_free(list2);

    // Test with type-safe macro
    mvn_list_t* list3 = MVN_LIST_INIT(mvn_point_t, 8);
    TEST_ASSERT(list3 != NULL, "Failed to initialize list with type-safe macro");
    mvn_list_free(list3);

    // Test with invalid parameters
    mvn_list_t* list4 = mvn_list_init(0, 10);
    TEST_ASSERT(list4 == NULL, "List initialization should fail with item_size = 0");

    return 1;
}

/**
 * \brief           Test push and pop operations
 * \return          1 on success, 0 on failure
 */
static int
test_list_push_pop(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 4);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Test pushing items
    int values[] = {10, 20, 30, 40, 50};
    for (int idx = 0; idx < 5; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
        TEST_ASSERT(mvn_list_length(list) == (size_t)(idx + 1), "List length incorrect after push");
    }

    // Test getting items
    for (int idx = 0; idx < 5; idx++) {
        int* value = MVN_LIST_GET(int, list, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from list");
        TEST_ASSERT(*value == values[idx], "Retrieved value is incorrect");
    }

    // Test popping items
    for (int idx = 4; idx >= 0; idx--) {
        int popped;
        TEST_ASSERT(MVN_LIST_POP(list, int, &popped), "Failed to pop value from list");
        TEST_ASSERT(popped == values[idx], "Popped value is incorrect");
        TEST_ASSERT(mvn_list_length(list) == (size_t)idx, "List length incorrect after pop");
    }

    // Test pop from empty list
    int value;
    TEST_ASSERT(!MVN_LIST_POP(list, int, &value), "Pop from empty list should fail");

    // Test push NULL
    TEST_ASSERT(!mvn_list_push(list, NULL), "Push NULL should fail");

    // Test push to NULL
    TEST_ASSERT(!mvn_list_push(NULL, &value), "Push to NULL list should fail");

    mvn_list_free(list);
    return 1;
}

/**
 * \brief           Test unshift and shift operations
 * \return          1 on success, 0 on failure
 */
static int
test_list_unshift_shift(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 4);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Test unshifting items
    int values[] = {10, 20, 30, 40, 50};
    for (int idx = 0; idx < 5; idx++) {
        MVN_LIST_UNSHIFT(list, int, values[idx]);
        TEST_ASSERT(mvn_list_length(list) == (size_t)(idx + 1),
                    "List length incorrect after unshift");

        // Verify the item was added to the beginning
        int* first = MVN_LIST_GET(int, list, 0);
        TEST_ASSERT(first != NULL, "Failed to get first value from list");
        TEST_ASSERT(*first == values[idx], "Unshifted value is incorrect");
    }

    // List should now contain: 50, 40, 30, 20, 10

    // Test shifting items
    for (int idx = 0; idx < 5; idx++) {
        int shifted;
        TEST_ASSERT(MVN_LIST_SHIFT(list, int, &shifted), "Failed to shift value from list");
        TEST_ASSERT(shifted == values[4 - idx], "Shifted value is incorrect");
        TEST_ASSERT(mvn_list_length(list) == (size_t)(4 - idx),
                    "List length incorrect after shift");
    }

    // Test shift from empty list
    int value;
    TEST_ASSERT(!MVN_LIST_SHIFT(list, int, &value), "Shift from empty list should fail");

    // Test unshift NULL
    TEST_ASSERT(!mvn_list_unshift(list, NULL), "Unshift NULL should fail");

    // Test unshift to NULL
    TEST_ASSERT(!mvn_list_unshift(NULL, &value), "Unshift to NULL list should fail");

    mvn_list_free(list);
    return 1;
}

/**
 * \brief           Test get and set operations
 * \return          1 on success, 0 on failure
 */
static int
test_list_get_set(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 4);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Add some items
    int values[] = {10, 20, 30, 40};
    for (int idx = 0; idx < 4; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
    }

    // Test get with valid index
    for (int idx = 0; idx < 4; idx++) {
        int* value = MVN_LIST_GET(int, list, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from list");
        TEST_ASSERT(*value == values[idx], "Retrieved value is incorrect");
    }

    // Test get with invalid index
    TEST_ASSERT(MVN_LIST_GET(int, list, 4) == NULL, "Get with invalid index should return NULL");
    TEST_ASSERT(MVN_LIST_GET(int, list, 100) == NULL, "Get with invalid index should return NULL");

    // Test get from NULL list
    TEST_ASSERT(MVN_LIST_GET(int, NULL, 0) == NULL, "Get from NULL list should return NULL");

    // Test set with valid index
    int new_value = 99;
    for (int idx = 0; idx < 4; idx++) {
        int temp_set_value = new_value + idx;
        MVN_LIST_SET(list, idx, int, temp_set_value);
        int* value = MVN_LIST_GET(int, list, idx);
        TEST_ASSERT(value != NULL, "Failed to get value after set");
        TEST_ASSERT(*value == temp_set_value, "Set value is incorrect");
    }

    // Test set with invalid index
    TEST_ASSERT(!mvn_list_set(list, 4, &new_value), "Set with invalid index should fail");
    TEST_ASSERT(!mvn_list_set(list, 100, &new_value), "Set with invalid index should fail");

    // Test set NULL value
    TEST_ASSERT(!mvn_list_set(list, 0, NULL), "Set NULL value should fail");

    // Test set on NULL list
    TEST_ASSERT(!mvn_list_set(NULL, 0, &new_value), "Set on NULL list should fail");

    mvn_list_free(list);
    return 1;
}

/**
 * \brief           Test slice operation
 * \return          1 on success, 0 on failure
 */
static int
test_list_slice(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 10);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Add some items
    int values[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for (int idx = 0; idx < 10; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
    }

    // Test slice with valid indices
    mvn_list_t* slice1 = mvn_list_slice(list, 2, 5);
    TEST_ASSERT(slice1 != NULL, "Failed to create slice");
    TEST_ASSERT(mvn_list_length(slice1) == 3, "Slice length is incorrect");

    // Verify slice contents (30, 40, 50)
    for (int idx = 0; idx < 3; idx++) {
        int* value = MVN_LIST_GET(int, slice1, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from slice");
        TEST_ASSERT(*value == values[idx + 2], "Slice value is incorrect");
    }

    // Test slice to end with -1
    mvn_list_t* slice2 = mvn_list_slice(list, 7, (size_t)-1);
    TEST_ASSERT(slice2 != NULL, "Failed to create slice to end");
    TEST_ASSERT(mvn_list_length(slice2) == 3, "Slice to end length is incorrect");

    // Verify slice contents (80, 90, 100)
    for (int idx = 0; idx < 3; idx++) {
        int* value = MVN_LIST_GET(int, slice2, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from slice to end");
        TEST_ASSERT(*value == values[idx + 7], "Slice to end value is incorrect");
    }

    // Test empty slice
    mvn_list_t* slice3 = mvn_list_slice(list, 3, 3);
    TEST_ASSERT(slice3 != NULL, "Failed to create empty slice");
    TEST_ASSERT(mvn_list_length(slice3) == 0, "Empty slice should have length 0");

    // Test invalid slice (start > end)
    mvn_list_t* slice4 = mvn_list_slice(list, 5, 3);
    TEST_ASSERT(slice4 == NULL, "Slice with start > end should fail");

    // Test invalid slice (start > length)
    mvn_list_t* slice5 = mvn_list_slice(list, 11, 15);
    TEST_ASSERT(slice5 == NULL, "Slice with start > length should fail");

    // Test slice from NULL list
    mvn_list_t* slice6 = mvn_list_slice(NULL, 0, 3);
    TEST_ASSERT(slice6 == NULL, "Slice from NULL list should fail");

    mvn_list_free(slice1);
    mvn_list_free(slice2);
    mvn_list_free(slice3);
    mvn_list_free(list);
    return 1;
}

/**
 * \brief           Test concat operation
 * \return          1 on success, 0 on failure
 */
static int
test_list_concat(void) {
    mvn_list_t* list1 = MVN_LIST_INIT(int, 3);
    mvn_list_t* list2 = MVN_LIST_INIT(int, 3);
    TEST_ASSERT(list1 != NULL && list2 != NULL, "Failed to initialize lists");

    // Add items to first list
    int values1[] = {10, 20, 30};
    for (int idx = 0; idx < 3; idx++) {
        MVN_LIST_PUSH(list1, int, values1[idx]);
    }

    // Add items to second list
    int values2[] = {40, 50, 60};
    for (int idx = 0; idx < 3; idx++) {
        MVN_LIST_PUSH(list2, int, values2[idx]);
    }

    // Concatenate lists
    mvn_list_t* concat = mvn_list_concat(list1, list2);
    TEST_ASSERT(concat != NULL, "Failed to concatenate lists");
    TEST_ASSERT(mvn_list_length(concat) == 6, "Concatenated list length is incorrect");

    // Verify concatenated list (10, 20, 30, 40, 50, 60)
    for (int idx = 0; idx < 3; idx++) {
        int* value = MVN_LIST_GET(int, concat, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from concatenated list");
        TEST_ASSERT(*value == values1[idx], "Concatenated list value is incorrect");
    }

    for (int idx = 0; idx < 3; idx++) {
        int* value = MVN_LIST_GET(int, concat, idx + 3);
        TEST_ASSERT(value != NULL, "Failed to get value from concatenated list");
        TEST_ASSERT(*value == values2[idx], "Concatenated list value is incorrect");
    }

    // Test concat with empty lists
    mvn_list_t* empty1 = MVN_LIST_INIT(int, 0);
    mvn_list_t* empty2 = MVN_LIST_INIT(int, 0);
    TEST_ASSERT(empty1 != NULL && empty2 != NULL, "Failed to initialize empty lists");

    mvn_list_t* empty_concat = mvn_list_concat(empty1, empty2);
    TEST_ASSERT(empty_concat != NULL, "Failed to concatenate empty lists");
    TEST_ASSERT(mvn_list_length(empty_concat) == 0, "Empty concatenated list should have length 0");

    // Test concat with one empty list
    mvn_list_t* half_concat1 = mvn_list_concat(list1, empty1);
    TEST_ASSERT(half_concat1 != NULL, "Failed to concatenate with empty list");
    TEST_ASSERT(mvn_list_length(half_concat1) == 3, "Half concatenated list length is incorrect");

    mvn_list_t* half_concat2 = mvn_list_concat(empty2, list2);
    TEST_ASSERT(half_concat2 != NULL, "Failed to concatenate with empty list");
    TEST_ASSERT(mvn_list_length(half_concat2) == 3, "Half concatenated list length is incorrect");

    // Test concat with NULL list
    TEST_ASSERT(mvn_list_concat(NULL, list2) == NULL, "Concat with NULL list should fail");
    TEST_ASSERT(mvn_list_concat(list1, NULL) == NULL, "Concat with NULL list should fail");

    mvn_list_free(concat);
    mvn_list_free(empty_concat);
    mvn_list_free(half_concat1);
    mvn_list_free(half_concat2);
    mvn_list_free(list1);
    mvn_list_free(list2);
    mvn_list_free(empty1);
    mvn_list_free(empty2);
    return 1;
}

/**
 * \brief           Test clone operation
 * \return          1 on success, 0 on failure
 */
static int
test_list_clone(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 5);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Add some items
    int values[] = {10, 20, 30, 40, 50};
    for (int idx = 0; idx < 5; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
    }

    // Clone the list
    mvn_list_t* clone = mvn_list_clone(list);
    TEST_ASSERT(clone != NULL, "Failed to clone list");
    TEST_ASSERT(mvn_list_length(clone) == 5, "Clone length is incorrect");

    // Verify clone contents
    for (int idx = 0; idx < 5; idx++) {
        int* orig_value = MVN_LIST_GET(int, list, idx);
        int* clone_value = MVN_LIST_GET(int, clone, idx);

        TEST_ASSERT(orig_value != NULL && clone_value != NULL,
                    "Failed to get values from original or clone");
        TEST_ASSERT(*orig_value == *clone_value, "Clone value doesn't match original");

        // Verify it's a deep copy (changing one doesn't affect the other)
        int new_value = 99;
        MVN_LIST_SET(list, idx, int, new_value);
        TEST_ASSERT(*clone_value != new_value,
                    "Clone should not be affected by changes to original");
    }

    // Test clone empty list
    mvn_list_t* empty = MVN_LIST_INIT(int, 0);
    mvn_list_t* empty_clone = mvn_list_clone(empty);
    TEST_ASSERT(empty_clone != NULL, "Failed to clone empty list");
    TEST_ASSERT(mvn_list_length(empty_clone) == 0, "Empty clone should have length 0");

    // Test clone NULL list
    TEST_ASSERT(mvn_list_clone(NULL) == NULL, "Clone NULL list should fail");

    mvn_list_free(clone);
    mvn_list_free(list);
    mvn_list_free(empty);
    mvn_list_free(empty_clone);
    return 1;
}

/**
 * \brief           Test resize operation
 * \return          1 on success, 0 on failure
 */
static int
test_list_resize(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 3);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Add some items to fill the initial capacity
    int values[] = {10, 20, 30};
    for (int idx = 0; idx < 3; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
    }

    // Resize to larger capacity
    TEST_ASSERT(mvn_list_resize(list, 10), "Failed to resize list to larger capacity");

    // Add more items to the resized list
    int more_values[] = {40, 50, 60, 70};
    for (int idx = 0; idx < 4; idx++) {
        MVN_LIST_PUSH(list, int, more_values[idx]);
    }

    // Verify all items are still there
    TEST_ASSERT(mvn_list_length(list) == 7, "List length incorrect after resize and additions");

    for (int idx = 0; idx < 3; idx++) {
        int* value = MVN_LIST_GET(int, list, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from resized list");
        TEST_ASSERT(*value == values[idx], "Value in resized list is incorrect");
    }

    for (int idx = 0; idx < 4; idx++) {
        int* value = MVN_LIST_GET(int, list, idx + 3);
        TEST_ASSERT(value != NULL, "Failed to get value from resized list");
        TEST_ASSERT(*value == more_values[idx], "Value in resized list is incorrect");
    }

    // Attempt to resize to smaller than length (should resize to length)
    size_t length_before = mvn_list_length(list);
    TEST_ASSERT(mvn_list_resize(list, 2), "Failed to resize list to smaller capacity");
    TEST_ASSERT(mvn_list_length(list) == length_before,
                "List length should not change when resizing to smaller than length");

    // Test resize on NULL list
    TEST_ASSERT(!mvn_list_resize(NULL, 10), "Resize NULL list should fail");

    mvn_list_free(list);
    return 1;
}

/**
 * \brief           Test reverse operation
 * \return          1 on success, 0 on failure
 */
static int
test_list_reverse(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 5);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Add some items
    int values[] = {10, 20, 30, 40, 50};
    for (int idx = 0; idx < 5; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
    }

    // Reverse the list
    TEST_ASSERT(mvn_list_reverse(list), "Failed to reverse list");

    // Verify reversed contents (50, 40, 30, 20, 10)
    for (int idx = 0; idx < 5; idx++) {
        int* value = MVN_LIST_GET(int, list, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from reversed list");
        TEST_ASSERT(*value == values[4 - idx], "Reversed value is incorrect");
    }

    // Test reverse with single item
    mvn_list_t* single = MVN_LIST_INIT(int, 1);
    int single_value = 42;
    MVN_LIST_PUSH(single, int, single_value);

    TEST_ASSERT(mvn_list_reverse(single), "Failed to reverse single-item list");
    int* result = MVN_LIST_GET(int, single, 0);
    TEST_ASSERT(result != NULL && *result == 42,
                "Single item should remain unchanged after reverse");

    // Test reverse with empty list
    mvn_list_t* empty = MVN_LIST_INIT(int, 0);
    TEST_ASSERT(mvn_list_reverse(empty), "Failed to reverse empty list");
    TEST_ASSERT(mvn_list_length(empty) == 0, "Empty list should remain empty after reverse");

    // Test reverse with NULL list
    TEST_ASSERT(!mvn_list_reverse(NULL), "Reverse NULL list should fail");

    mvn_list_free(list);
    mvn_list_free(single);
    mvn_list_free(empty);
    return 1;
}

/**
 * \brief           Compare function for sorting
 * \return          Negative if a < b, 0 if a == b, positive if a > b
 */
static int
compare_ints(const void* ptr_a, const void* ptr_b) {
    const int* int_a = (const int*)ptr_a;
    const int* int_b = (const int*)ptr_b;
    return *int_a - *int_b;
}

/**
 * \brief           Reverse compare function for sorting
 * \return          Negative if a > b, 0 if a == b, positive if a < b
 */
static int
compare_ints_reverse(const void* a, const void* b) {
    const int* int_a = (const int*)a;
    const int* int_b = (const int*)b;
    return *int_b - *int_a;
}

/**
 * \brief           Test sort operation
 * \return          1 on success, 0 on failure
 */
static int
test_list_sort(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 7);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Add items in unsorted order
    int values[] = {30, 10, 50, 40, 20, 70, 60};
    for (int idx = 0; idx < 7; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
    }

    // Sort the list
    TEST_ASSERT(mvn_list_sort(list, compare_ints), "Failed to sort list");

    // Verify sorted contents (10, 20, 30, 40, 50, 60, 70)
    int sorted[] = {10, 20, 30, 40, 50, 60, 70};
    for (int idx = 0; idx < 7; idx++) {
        int* value = MVN_LIST_GET(int, list, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from sorted list");
        TEST_ASSERT(*value == sorted[idx], "Sorted value is incorrect");
    }

    // Test reverse sort
    TEST_ASSERT(mvn_list_sort(list, compare_ints_reverse), "Failed to reverse sort list");

    // Verify reverse sorted contents (70, 60, 50, 40, 30, 20, 10)
    for (int idx = 0; idx < 7; idx++) {
        int* value = MVN_LIST_GET(int, list, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from reverse sorted list");
        TEST_ASSERT(*value == sorted[6 - idx], "Reverse sorted value is incorrect");
    }

    // Test sort with single item
    mvn_list_t* single = MVN_LIST_INIT(int, 1);
    int single_value = 42;
    MVN_LIST_PUSH(single, int, single_value);

    TEST_ASSERT(mvn_list_sort(single, compare_ints), "Failed to sort single-item list");
    int* result = MVN_LIST_GET(int, single, 0);
    TEST_ASSERT(result != NULL && *result == 42, "Single item should remain unchanged after sort");

    // Test sort with empty list
    mvn_list_t* empty = MVN_LIST_INIT(int, 0);
    TEST_ASSERT(mvn_list_sort(empty, compare_ints), "Failed to sort empty list");
    TEST_ASSERT(mvn_list_length(empty) == 0, "Empty list should remain empty after sort");

    // Test sort with NULL list
    TEST_ASSERT(!mvn_list_sort(NULL, compare_ints), "Sort NULL list should fail");

    // Test sort with NULL compare function
    TEST_ASSERT(!mvn_list_sort(list, NULL), "Sort with NULL compare function should fail");

    mvn_list_free(list);
    mvn_list_free(single);
    mvn_list_free(empty);
    return 1;
}

/**
 * \brief           Filter function that keeps even numbers
 * \return          true to keep item, false to filter out
 */
static bool
filter_even(const void* item, void* user_data) {
    (void)user_data; // Mark parameter as unused
    const int* value = (const int*)item;
    return (*value % 2) == 0;
}

/**
 * \brief           Filter function with user data
 * \return          true to keep item, false to filter out
 */
static bool
filter_greater_than(const void* item, void* user_data) {
    const int* value = (const int*)item;
    const int* threshold = (const int*)user_data;
    return *value > *threshold;
}

/**
 * \brief           Test filter operation
 * \return          1 on success, 0 on failure
 */
static int
test_list_filter(void) {
    mvn_list_t* list = MVN_LIST_INIT(int, 10);
    TEST_ASSERT(list != NULL, "Failed to initialize list");

    // Add some items
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int idx = 0; idx < 10; idx++) {
        MVN_LIST_PUSH(list, int, values[idx]);
    }

    // Filter for even numbers
    mvn_list_t* evens = mvn_list_filter(list, filter_even, NULL);
    TEST_ASSERT(evens != NULL, "Failed to filter list");
    TEST_ASSERT(mvn_list_length(evens) == 5, "Filtered list length is incorrect");

    // Verify filtered contents (2, 4, 6, 8, 10)
    int expected[] = {2, 4, 6, 8, 10};
    for (int idx = 0; idx < 5; idx++) {
        int* value = MVN_LIST_GET(int, evens, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from filtered list");
        TEST_ASSERT(*value == expected[idx], "Filtered value is incorrect");
    }

    // Test filter with user data
    int threshold = 7;
    mvn_list_t* greater = mvn_list_filter(list, filter_greater_than, &threshold);
    TEST_ASSERT(greater != NULL, "Failed to filter list with user data");
    TEST_ASSERT(mvn_list_length(greater) == 3, "Filtered list length is incorrect");

    // Verify filtered contents (8, 9, 10)
    int expected2[] = {8, 9, 10};
    for (int idx = 0; idx < 3; idx++) {
        int* value = MVN_LIST_GET(int, greater, idx);
        TEST_ASSERT(value != NULL, "Failed to get value from filtered list");
        TEST_ASSERT(*value == expected2[idx], "Filtered value is incorrect");
    }

    // Test filter with empty list
    mvn_list_t* empty = MVN_LIST_INIT(int, 0);
    mvn_list_t* empty_filtered = mvn_list_filter(empty, filter_even, NULL);
    TEST_ASSERT(empty_filtered != NULL, "Failed to filter empty list");
    TEST_ASSERT(mvn_list_length(empty_filtered) == 0, "Empty filtered list should have length 0");

    // Test filter with NULL list
    TEST_ASSERT(mvn_list_filter(NULL, filter_even, NULL) == NULL, "Filter NULL list should fail");

    // Test filter with NULL filter function
    TEST_ASSERT(mvn_list_filter(list, NULL, NULL) == NULL, "Filter with NULL function should fail");

    mvn_list_free(list);
    mvn_list_free(evens);
    mvn_list_free(greater);
    mvn_list_free(empty);
    mvn_list_free(empty_filtered);
    return 1;
}

/**
 * \brief           Test complex data structures
 * \return          1 on success, 0 on failure
 */
static int
test_list_complex_types(void) {
    // Test with points
    mvn_list_t* points = MVN_LIST_INIT(mvn_point_t, 3);
    TEST_ASSERT(points != NULL, "Failed to initialize points list");

    mvn_point_t pnt1 = {10, 20};
    mvn_point_t pnt2 = {30, 40};
    mvn_point_t pnt3 = {50, 60};

    MVN_LIST_PUSH(points, mvn_point_t, pnt1);
    MVN_LIST_PUSH(points, mvn_point_t, pnt2);
    MVN_LIST_PUSH(points, mvn_point_t, pnt3);

    TEST_ASSERT(mvn_list_length(points) == 3, "Points list length is incorrect");

    // Verify contents
    mvn_point_t* point1 = MVN_LIST_GET(mvn_point_t, points, 0);
    mvn_point_t* point2 = MVN_LIST_GET(mvn_point_t, points, 1);
    mvn_point_t* point3 = MVN_LIST_GET(mvn_point_t, points, 2);

    TEST_ASSERT(point1 != NULL && point2 != NULL && point3 != NULL,
                "Failed to get points from list");

    TEST_ASSERT(point1->x == 10 && point1->y == 20, "Point 1 has incorrect values");
    TEST_ASSERT(point2->x == 30 && point2->y == 40, "Point 2 has incorrect values");
    TEST_ASSERT(point3->x == 50 && point3->y == 60, "Point 3 has incorrect values");

    // Test with colors
    mvn_list_t* colors = MVN_LIST_INIT(mvn_color_t, 2);
    TEST_ASSERT(colors != NULL, "Failed to initialize colors list");

    mvn_color_t red = {1, 0, 0, 1};
    mvn_color_t blue = {0, 0, 1, 1};

    MVN_LIST_PUSH(colors, mvn_color_t, red);
    MVN_LIST_PUSH(colors, mvn_color_t, blue);

    TEST_ASSERT(mvn_list_length(colors) == 2, "Colors list length is incorrect");

    // Verify contents
    mvn_color_t* color1 = MVN_LIST_GET(mvn_color_t, colors, 0);
    mvn_color_t* color2 = MVN_LIST_GET(mvn_color_t, colors, 1);

    TEST_ASSERT(color1 != NULL && color2 != NULL, "Failed to get colors from list");

    TEST_ASSERT(color1->r == 1.0f && color1->g == 0.0f && color1->b == 0.0f && color1->a == 1.0f,
                "Color 1 has incorrect values");
    TEST_ASSERT(color2->r == 0.0f && color2->g == 0.0f && color2->b == 1.0f && color2->a == 1.0f,
                "Color 2 has incorrect values");

    mvn_list_free(points);
    mvn_list_free(colors);
    return 1;
}

/**
 * \brief           Run all list tests
 * \param[out] passed_tests Pointer to the number of passed tests
 * \param[out] failed_tests Pointer to the number of failed tests
 * \param[out] total_tests Pointer to the total number of tests
 * \return          Number of passed tests
 */
int
run_list_tests(int* passed_tests, int* failed_tests, int* total_tests) {
    printf("\n===== LIST TESTS =====\n\n");

    int passed_before = *passed_tests;
    int failed_before = *failed_tests;

    RUN_TEST(test_list_init);
    RUN_TEST(test_list_push_pop);
    RUN_TEST(test_list_unshift_shift);
    RUN_TEST(test_list_get_set);
    RUN_TEST(test_list_slice);
    RUN_TEST(test_list_concat);
    RUN_TEST(test_list_clone);
    RUN_TEST(test_list_resize);
    RUN_TEST(test_list_reverse);
    RUN_TEST(test_list_sort);
    RUN_TEST(test_list_filter);
    RUN_TEST(test_list_complex_types);

    // Calculate how many tests were run
    int tests_run = (*passed_tests - passed_before) + (*failed_tests - failed_before);
    (*total_tests) += tests_run;

    // Return number of passed tests from this suite
    return *passed_tests - passed_before;
}

#if defined(MVN_LIST_TEST_MAIN)
int
main(void) {
    int passed = 0;
    int failed = 0;
    int total = 0;

    run_list_tests(&passed, &failed, &total);

    printf("\n===== LIST TEST SUMMARY =====\n");
    print_test_summary(total, passed, failed);

    return (failed > 0) ? 1 : 0;
}
#endif
