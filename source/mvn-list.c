/**
 * \file            mvn-list.c
 * \brief           Implementation of dynamic array list for MVN game framework
 */

#include "mvn/mvn-list.h"
#include <SDL3/SDL.h>
#include "mvn/mvn-logger.h"

/* Default initial capacity if none is specified */
#define MVN_LIST_DEFAULT_CAPACITY  8

/* Growth factor when resizing */
#define MVN_LIST_GROWTH_FACTOR     2

/* Stack buffer size for small item temporary storage */
#define MVN_LIST_STACK_BUFFER_SIZE 64

/**
 * \brief           Initialize a new list
 * \param[in]       item_size: Size of each item in bytes
 * \param[in]       initial_capacity: Initial capacity (0 for default)
 * \return          New list or NULL on failure
 */
mvn_list_t*
mvn_list_init(size_t item_size, size_t initial_capacity) {
    if (item_size == 0) {
        mvn_log_error("Cannot create list with item_size 0");
        return NULL;
    }

    /* Use default capacity if not specified */
    if (initial_capacity == 0) {
        initial_capacity = MVN_LIST_DEFAULT_CAPACITY;
    }

    /* Allocate list structure */
    mvn_list_t* list = MVN_MALLOC(sizeof(mvn_list_t));
    if (!list) {
        mvn_log_error("Failed to allocate memory for list");
        return NULL;
    }

    /* Allocate data array - use MVN_MALLOC instead of MVN_CALLOC for better performance
     * since we'll be overwriting all this memory anyway */
    list->data = MVN_MALLOC(initial_capacity * item_size);
    if (!list->data) {
        mvn_log_error("Failed to allocate memory for list data");
        MVN_FREE(list);
        return NULL;
    }

    list->item_size = item_size;
    list->length = 0;
    list->capacity = initial_capacity;

    mvn_log_debug("List initialized with item_size=%zu, capacity=%zu", item_size, initial_capacity);
    return list;
}

/**
 * \brief           Free a list and all its resources
 * \param[in]       list: List to free
 */
void
mvn_list_free(mvn_list_t* list) {
    if (!list) {
        return;
    }

    if (list->data) {
        MVN_FREE(list->data);
    }

    MVN_FREE(list);
    mvn_log_debug("List freed");
}

/**
 * \brief           Get the number of items in the list
 * \param[in]       list: List to query
 * \return          Number of items in the list
 */
size_t
mvn_list_length(const mvn_list_t* list) {
    if (!list) {
        return 0;
    }
    return list->length;
}

/**
 * \brief           Reserve capacity for future list growth
 * \param[in]       list: List to reserve capacity for
 * \param[in]       capacity: Desired minimum capacity
 * \return          true on success, false on failure
 */
bool
mvn_list_reserve(mvn_list_t* list, size_t capacity) {
    if (!list) {
        return false;
    }

    if (capacity <= list->capacity) {
        return true; /* Already have enough capacity */
    }

    return mvn_list_resize(list, capacity);
}

/**
 * \brief           Resize the list capacity
 * \param[in]       list: List to resize
 * \param[in]       new_capacity: New capacity
 * \return          true on success, false on failure
 */
bool
mvn_list_resize(mvn_list_t* list, size_t new_capacity) {
    if (!list) {
        mvn_log_error("Cannot resize NULL list");
        return false;
    }

    /* Don't allow resizing smaller than current length */
    if (new_capacity < list->length) {
        new_capacity = list->length;
    }

    if (new_capacity == list->capacity) {
        return true; /* No change needed */
    }

    /* For very small new capacities, use a minimum threshold to reduce future resizes */
    if (new_capacity > 0 && new_capacity < MVN_LIST_DEFAULT_CAPACITY) {
        new_capacity = MVN_LIST_DEFAULT_CAPACITY;
    }

    void* new_data = MVN_REALLOC(list->data, new_capacity * list->item_size);
    if (!new_data) {
        mvn_log_error("Failed to resize list to capacity %zu", new_capacity);
        return false;
    }

    list->data = new_data;
    list->capacity = new_capacity;

    mvn_log_debug("List resized to capacity %zu", new_capacity);
    return true;
}

/**
 * \brief           Ensure the list has enough capacity for one more item
 * \param[in]       list: List to check capacity for
 * \return          true if capacity is sufficient or was successfully increased, false on failure
 */
static bool
mvn_list_ensure_capacity(mvn_list_t* list) {
    if (list->length >= list->capacity) {
        size_t new_capacity = list->capacity * MVN_LIST_GROWTH_FACTOR;
        if (new_capacity == 0) {
            new_capacity = MVN_LIST_DEFAULT_CAPACITY;
        }
        return mvn_list_resize(list, new_capacity);
    }
    return true;
}

/**
 * \brief           Get item at a specific index
 * \param[in]       list: List to get item from
 * \param[in]       index: Index of the item to get
 * \return          Pointer to the item or NULL if index is out of bounds
 */
void*
mvn_list_get(const mvn_list_t* list, size_t index) {
    if (!list || index >= list->length) {
        return NULL;
    }

    return (char*)list->data + (index * list->item_size);
}

/**
 * \brief           Set an item at a specific index
 * \param[in]       list: List to modify
 * \param[in]       index: Index where to set the item
 * \param[in]       item: Item to set
 * \return          true on success, false on failure
 */
bool
mvn_list_set(mvn_list_t* list, size_t index, const void* item) {
    if (!list || !item || index >= list->length) {
        return false;
    }

    void* dest = (char*)list->data + (index * list->item_size);
    SDL_memcpy(dest, item, list->item_size);
    return true;
}

/**
 * \brief           Add an item to the end of a list
 * \param[in]       list: List to add item to
 * \param[in]       item: Item to add
 * \return          true on success, false on failure
 */
bool
mvn_list_push(mvn_list_t* list, const void* item) {
    if (!list || !item) {
        mvn_log_error("Invalid parameters for list push");
        return false;
    }

    if (!mvn_list_ensure_capacity(list)) {
        return false;
    }

    void* dest = (char*)list->data + (list->length * list->item_size);
    SDL_memcpy(dest, item, list->item_size);
    list->length++;

    return true;
}

/**
 * \brief           Add multiple items to the end of a list in one operation
 * \param[in]       list: List to add items to
 * \param[in]       items: Array of items to add
 * \param[in]       count: Number of items to add
 * \return          true on success, false on failure
 */
bool
mvn_list_push_batch(mvn_list_t* list, const void* items, size_t count) {
    if (!list || !items || count == 0) {
        return false;
    }

    /* Ensure we have enough space */
    if (list->length + count > list->capacity) {
        size_t new_capacity = list->length + count;
        /* Round up to the next power of MVN_LIST_GROWTH_FACTOR */
        new_capacity = new_capacity * MVN_LIST_GROWTH_FACTOR;

        if (!mvn_list_resize(list, new_capacity)) {
            return false;
        }
    }

    /* Copy all items at once */
    void* dest = (char*)list->data + (list->length * list->item_size);
    SDL_memcpy(dest, items, count * list->item_size);
    list->length += count;

    return true;
}

/**
 * \brief           Remove and return the last item from a list
 * \param[in]       list: List to pop from
 * \param[out]      out_item: Output buffer for the popped item (can be NULL)
 * \return          true on success, false on failure
 */
bool
mvn_list_pop(mvn_list_t* list, void* out_item) {
    if (!list || list->length == 0) {
        return false;
    }

    list->length--;

    if (out_item) {
        void* src = (char*)list->data + (list->length * list->item_size);
        SDL_memcpy(out_item, src, list->item_size);
    }

    return true;
}

/**
 * \brief           Add an item to the beginning of a list
 * \param[in]       list: List to add item to
 * \param[in]       item: Item to add
 * \return          true on success, false on failure
 */
bool
mvn_list_unshift(mvn_list_t* list, const void* item) {
    if (!list || !item) {
        mvn_log_error("Invalid parameters for list unshift");
        return false;
    }

    if (!mvn_list_ensure_capacity(list)) {
        return false;
    }

    /* Shift existing items right by one position */
    if (list->length > 0) {
        void* src = list->data;
        void* dest = (char*)list->data + list->item_size;

        /* For small lists or large items, directly use memmove */
        SDL_memmove(dest, src, list->length * list->item_size);
    }

    /* Insert new item at the beginning */
    SDL_memcpy(list->data, item, list->item_size);
    list->length++;

    return true;
}

/**
 * \brief           Remove and return the first item from a list
 * \param[in]       list: List to shift from
 * \param[out]      out_item: Output buffer for the shifted item (can be NULL)
 * \return          true on success, false on failure
 */
bool
mvn_list_shift(mvn_list_t* list, void* out_item) {
    if (!list || list->length == 0) {
        return false;
    }

    /* Copy first item to output if requested */
    if (out_item) {
        SDL_memcpy(out_item, list->data, list->item_size);
    }

    /* Shift remaining items left by one position */
    if (list->length > 1) {
        void* dest = list->data;
        void* src = (char*)list->data + list->item_size;
        SDL_memmove(dest, src, (list->length - 1) * list->item_size);
    }

    list->length--;
    return true;
}

/**
 * \brief           Create a new list containing a slice of the original list
 * \param[in]       list: List to slice
 * \param[in]       start: Start index (inclusive)
 * \param[in]       end: End index (exclusive), use -1 for end of list
 * \return          New list with sliced items or NULL on failure
 */
mvn_list_t*
mvn_list_slice(const mvn_list_t* list, size_t start, size_t end) {
    if (!list) {
        mvn_log_error("Cannot slice NULL list");
        return NULL;
    }

    /* Handle end == -1 (slice to end of list) */
    if (end == (size_t)-1 || end > list->length) {
        end = list->length;
    }

    /* Validate indices */
    if (start > list->length || start > end) {
        mvn_log_error("Invalid slice indices: start=%zu, end=%zu, length=%zu", start, end,
                      list->length);
        return NULL;
    }

    size_t slice_length = end - start;

    /* Create new list with exact capacity needed */
    mvn_list_t* result = mvn_list_init(list->item_size,
                                       slice_length > 0 ? slice_length : MVN_LIST_DEFAULT_CAPACITY);
    if (!result) {
        return NULL;
    }

    /* Copy slice data */
    if (slice_length > 0) {
        void* src = (char*)list->data + (start * list->item_size);
        SDL_memcpy(result->data, src, slice_length * list->item_size);
        result->length = slice_length;
    }

    return result;
}

/**
 * \brief           Concatenate two lists into a new list
 * \param[in]       list1: First list
 * \param[in]       list2: Second list
 * \return          New list containing items from both lists or NULL on failure
 */
mvn_list_t*
mvn_list_concat(const mvn_list_t* list1, const mvn_list_t* list2) {
    if (!list1 || !list2) {
        mvn_log_error("Cannot concatenate NULL lists");
        return NULL;
    }

    if (list1->item_size != list2->item_size) {
        mvn_log_error("Cannot concatenate lists with different item sizes");
        return NULL;
    }

    size_t total_length = list1->length + list2->length;

    /* Create new list with exact capacity needed */
    mvn_list_t* result = mvn_list_init(list1->item_size,
                                       total_length > 0 ? total_length : MVN_LIST_DEFAULT_CAPACITY);
    if (!result) {
        return NULL;
    }

    /* Copy data from first list */
    if (list1->length > 0) {
        SDL_memcpy(result->data, list1->data, list1->length * list1->item_size);
        result->length = list1->length;
    }

    /* Copy data from second list */
    if (list2->length > 0) {
        void* dest = (char*)result->data + (list1->length * list1->item_size);
        SDL_memcpy(dest, list2->data, list2->length * list2->item_size);
        result->length += list2->length;
    }

    return result;
}

/**
 * \brief           Create a deep copy of a list
 * \param[in]       list: List to clone
 * \return          New copy of the list or NULL on failure
 */
mvn_list_t*
mvn_list_clone(const mvn_list_t* list) {
    if (!list) {
        mvn_log_error("Cannot clone NULL list");
        return NULL;
    }

    /* Create new list with same capacity */
    mvn_list_t* result = mvn_list_init(list->item_size, list->capacity);
    if (!result) {
        return NULL;
    }

    /* Copy data */
    if (list->length > 0) {
        SDL_memcpy(result->data, list->data, list->length * list->item_size);
        result->length = list->length;
    }

    return result;
}

/**
 * \brief           Reverse the order of items in the list
 * \param[in]       list: List to reverse
 * \return          true on success, false on failure
 */
bool
mvn_list_reverse(mvn_list_t* list) {
    if (!list) {
        mvn_log_error("Cannot reverse NULL list");
        return false;
    }

    if (list->length <= 1) {
        return true; /* Nothing to do */
    }

    char temp_storage[MVN_LIST_STACK_BUFFER_SIZE]; /* Stack buffer for small items */
    void* temp = temp_storage;

    /* Only allocate heap memory if item size is too large for stack */
    if (list->item_size > sizeof(temp_storage)) {
        temp = MVN_MALLOC(list->item_size);
        if (!temp) {
            mvn_log_error("Failed to allocate memory for list reverse operation");
            return false;
        }
    }

    /* Swap elements from both ends moving toward the center */
    for (size_t i = 0; i < list->length / 2; i++) {
        void* a = (char*)list->data + (i * list->item_size);
        void* b = (char*)list->data + ((list->length - 1 - i) * list->item_size);

        SDL_memcpy(temp, a, list->item_size);
        SDL_memcpy(a, b, list->item_size);
        SDL_memcpy(b, temp, list->item_size);
    }

    if (temp != temp_storage) {
        MVN_FREE(temp);
    }
    return true;
}

/**
 * \brief           Sort the list using a comparison function
 * \param[in]       list: List to sort
 * \param[in]       compare: Comparison function
 * \return          true on success, false on failure
 */
bool
mvn_list_sort(mvn_list_t* list, mvn_list_compare_fn compare) {
    if (!list || !compare) {
        mvn_log_error("Invalid parameters for list sort");
        return false;
    }

    if (list->length <= 1) {
        return true; /* Nothing to do */
    }

    /* Use SDL_qsort to sort the list */
    SDL_qsort(list->data, list->length, list->item_size, compare);
    return true;
}

/**
 * \brief           Create a new list with filtered elements
 * \param[in]       list: List to filter
 * \param[in]       filter: Filter function to apply to each element
 * \param[in]       user_data: User data to pass to the filter function
 * \return          New list with filtered items or NULL on failure
 */
mvn_list_t*
mvn_list_filter(const mvn_list_t* list, mvn_list_filter_fn filter, void* user_data) {
    if (!list || !filter) {
        mvn_log_error("Invalid parameters for list filter");
        return NULL;
    }

    /* First, count matching elements to optimize allocation */
    size_t match_count = 0;
    for (size_t i = 0; i < list->length; i++) {
        void* item = (char*)list->data + (i * list->item_size);
        if (filter(item, user_data)) {
            match_count++;
        }
    }

    /* Create new list with exact needed capacity */
    mvn_list_t* result = mvn_list_init(list->item_size,
                                       match_count > 0 ? match_count : MVN_LIST_DEFAULT_CAPACITY);
    if (!result) {
        return NULL;
    }

    /* Apply filter to each element and add matches in batch if possible */
    if (match_count > 0) {
        /* If all items match, we can do a single memcpy */
        if (match_count == list->length) {
            SDL_memcpy(result->data, list->data, list->length * list->item_size);
            result->length = list->length;
        } else {
            /* Otherwise add items one by one */
            for (size_t i = 0; i < list->length; i++) {
                void* item = (char*)list->data + (i * list->item_size);
                if (filter(item, user_data)) {
                    void* dest = (char*)result->data + (result->length * list->item_size);
                    SDL_memcpy(dest, item, list->item_size);
                    result->length++;
                }
            }
        }
    }

    return result;
}

/**
 * \brief           Clear all items from the list without changing capacity
 * \param[in]       list: List to clear
 * \return          true on success, false on failure
 */
bool
mvn_list_clear(mvn_list_t* list) {
    if (!list) {
        return false;
    }

    list->length = 0;
    return true;
}

/**
 * \brief           Trim excess capacity from the list
 * \param[in]       list: List to trim
 * \return          true on success, false on failure
 */
bool
mvn_list_trim(mvn_list_t* list) {
    if (!list) {
        return false;
    }

    if (list->length == 0) {
        /* Special case: if empty, reset to default capacity */
        return mvn_list_resize(list, MVN_LIST_DEFAULT_CAPACITY);
    }

    if (list->capacity > list->length) {
        /* Trim to exact length */
        return mvn_list_resize(list, list->length);
    }

    return true; /* Nothing to trim */
}
