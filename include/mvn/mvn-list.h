/**
 * \file            mvn-list.h
 * \brief           Dynamic array list implementation for MVN game framework
 */

#ifndef MVN_LIST_H
#define MVN_LIST_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Dynamic array list structure
 */
typedef struct mvn_list_t {
    void* data;       /*!< Pointer to the array of items */
    size_t item_size; /*!< Size of each item in bytes */
    size_t length;    /*!< Current number of items */
    size_t capacity;  /*!< Current allocated capacity */
} mvn_list_t;

mvn_list_t* mvn_list_init(size_t item_size, size_t initial_capacity);
void mvn_list_free(mvn_list_t* list);
size_t mvn_list_length(const mvn_list_t* list);
bool mvn_list_push(mvn_list_t* list, const void* item);
bool mvn_list_pop(mvn_list_t* list, void* out_item);
bool mvn_list_unshift(mvn_list_t* list, const void* item);
bool mvn_list_shift(mvn_list_t* list, void* out_item);
void* mvn_list_get(const mvn_list_t* list, size_t index);
bool mvn_list_set(mvn_list_t* list, size_t index, const void* item);
mvn_list_t* mvn_list_slice(const mvn_list_t* list, size_t start, size_t end);
mvn_list_t* mvn_list_concat(const mvn_list_t* list1, const mvn_list_t* list2);
mvn_list_t* mvn_list_clone(const mvn_list_t* list);
bool mvn_list_resize(mvn_list_t* list, size_t new_capacity);
bool mvn_list_reverse(mvn_list_t* list);

/**
 * \brief           Sort function typedef
 */
typedef int (*mvn_list_compare_fn)(const void*, const void*);

bool mvn_list_sort(mvn_list_t* list, mvn_list_compare_fn compare);

/**
 * \brief           Filter function typedef
 */
typedef bool (*mvn_list_filter_fn)(const void* item, void* user_data);

mvn_list_t* mvn_list_filter(const mvn_list_t* list, mvn_list_filter_fn filter, void* user_data);

/* Type-safe wrapper macros */

/**
 * \brief           Create a list for a specific type
 * \param[in]       T: Type of the list elements
 * \param[in]       capacity: Initial capacity
 * \return          Initialized list
 * \hideinitializer
 */
#define MVN_LIST_INIT(T, capacity)   mvn_list_init(sizeof(T), (capacity))

/**
 * \brief           Get the typed pointer to an element at index
 * \param[in]       T: Type of the list elements
 * \param[in]       list: List
 * \param[in]       index: Index of the element
 * \return          Typed pointer to the element
 * \hideinitializer
 */
#define MVN_LIST_GET(T, list, index) ((T*)mvn_list_get((list), (index)))

/**
 * \brief           Push an element to the list
 * \param[in]       list: List
 * \param[in]       T: Type of the list elements
 * \param[in]       value: Value to push
 * \return          true on success, false on failure
 * \hideinitializer
 */
#define MVN_LIST_PUSH(list, T, value)                                                              \
    do {                                                                                           \
        T _temp_value = (value);                                                                   \
        mvn_list_push((list), &_temp_value);                                                       \
    } while (0)

/**
 * \brief           Pop an element from the list
 * \param[in]       list: List
 * \param[in]       T: Type of the list elements
 * \param[out]      out_value: Pointer to store the popped value
 * \return          true if an element was popped, false otherwise
 * \hideinitializer
 */
#define MVN_LIST_POP(list, T, out_value) mvn_list_pop((list), (out_value))

/**
 * \brief           Unshift an element to the list
 * \param[in]       list: List
 * \param[in]       T: Type of the list elements
 * \param[in]       value: Value to unshift
 * \return          true on success, false on failure
 * \hideinitializer
 */
#define MVN_LIST_UNSHIFT(list, T, value)                                                           \
    do {                                                                                           \
        T _temp_value = (value);                                                                   \
        mvn_list_unshift((list), &_temp_value);                                                    \
    } while (0)

/**
 * \brief           Shift an element from the list
 * \param[in]       list: List
 * \param[in]       T: Type of the list elements
 * \param[out]      out_value: Pointer to store the shifted value
 * \return          true if an element was shifted, false otherwise
 * \hideinitializer
 */
#define MVN_LIST_SHIFT(list, T, out_value) mvn_list_shift((list), (out_value))

/**
 * \brief           Set an element at index
 * \param[in]       list: List
 * \param[in]       index: Index of the element
 * \param[in]       T: Type of the list elements
 * \param[in]       value: Value to set
 * \return          true on success, false on failure
 * \hideinitializer
 */
#define MVN_LIST_SET(list, index, T, value)                                                        \
    do {                                                                                           \
        T _temp_value = (value);                                                                   \
        mvn_list_set((list), (index), &_temp_value);                                               \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* MVN_LIST_H */
