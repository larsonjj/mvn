/**
 * array_list.c - Implementation for dynamic array functions
 */

#include "array_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Core array growing function - reallocates memory to expand array capacity
     *
     * @param list Pointer to the array
     * @param params Structure containing element size and number of elements to add capacity for
     * @return Pointer to the resized array (may be different from input pointer)
     */
    void *mvn__list_growf_(void *list, MVN_List_GrowParams params)
    {
        size_t current_size = mvn__list_size_of(list);
        size_t current_capacity = mvn__list_capacity_of(list);
        size_t new_capacity;

        // Calculate new capacity with growth strategy
        if (current_capacity == 0)
        {
            new_capacity = (params.add_len > 4) ? params.add_len : 4;
        }
        else
        {
            new_capacity = current_capacity * 2;
            if (new_capacity < current_size + params.add_len)
            {
                new_capacity = current_size + params.add_len;
            }
        }

        // Allocate or reallocate memory
        MVN_List_Header *new_header;
        if (list == NULL)
        {
            // First allocation
            new_header = (MVN_List_Header *)malloc(
                sizeof(MVN_List_Header) + new_capacity * params.elem_size
            );
            if (!new_header)
            {
                return NULL;
            }
            new_header->size = 0;
        }
        else
        {
            // Reallocate existing array with larger capacity
            new_header = (MVN_List_Header *)realloc(
                mvn__list_header(list), sizeof(MVN_List_Header) + new_capacity * params.elem_size
            );
            if (!new_header)
            {
                return NULL;
            }
        }

        new_header->capacity = new_capacity;
        return new_header->data;
    }

    /**
     * Joins elements into a string with separator
     *
     * @param arr Pointer to the array
     * @param elem_size Size of each array element
     * @param separator String to insert between elements
     * @param to_string Function that converts an element to a string
     * @return Newly allocated string with joined elements (caller must free)
     */
    char *mvn_list_join(
        const void *arr, size_t elem_size, const char *separator,
        char *(*to_string)(const void *elem)
    )
    {
        if (!arr || !to_string)
        {
            return NULL;
        }

        // Special case for empty array
        size_t size = mvn__list_size_of(arr);
        if (size == 0)
        {
            return strdup("");
        }

        // Calculate needed space for the final string
        size_t sep_len = separator ? strlen(separator) : 0;
        size_t total_len = 0;
        char **elem_strs = (char **)malloc(size * sizeof(char *));
        if (!elem_strs)
        {
            return NULL;
        }

        // Convert each element to string and calculate total length
        for (size_t i = 0; i < size; i++)
        {
            const void *elem_ptr = (const char *)arr + i * elem_size;
            elem_strs[i] = to_string(elem_ptr);
            if (!elem_strs[i])
            {
                // Clean up on error
                for (size_t j = 0; j < i; j++)
                {
                    free(elem_strs[j]);
                }
                free(elem_strs);
                return NULL;
            }
            total_len += strlen(elem_strs[i]);
            if (i < size - 1)
            {
                total_len += sep_len;
            }
        }

        // Allocate and construct the result string
        char *result = (char *)malloc(total_len + 1);
        if (!result)
        {
            for (size_t i = 0; i < size; i++)
            {
                free(elem_strs[i]);
            }
            free(elem_strs);
            return NULL;
        }

        // Copy strings and separators
        char *cursor = result;
        for (size_t i = 0; i < size; i++)
        {
            size_t len = strlen(elem_strs[i]);
            memcpy(cursor, elem_strs[i], len);
            cursor += len;

            if (i < size - 1 && separator)
            {
                memcpy(cursor, separator, sep_len);
                cursor += sep_len;
            }

            free(elem_strs[i]);
        }
        *cursor = '\0';
        free(elem_strs);

        return result;
    }

    /**
     * Convert a list of integers to a string with the given separator
     *
     * @param list Array of integers
     * @param separator String to insert between elements
     * @return Newly allocated string with joined integers (caller must free)
     */
    char *mvn_list_join_int(int *list, const char *separator)
    {
        if (!list)
        {
            return NULL;
        }

        size_t size = mvn_list_size(list);
        if (size == 0)
        {
            return strdup("");
        }

        // Calculate max needed space (20 chars per int + separators)
        size_t sep_len = separator ? strlen(separator) : 0;
        size_t buffer_size = size * 21 + (size - 1) * sep_len + 1;
        char *result = (char *)malloc(buffer_size);
        if (!result)
        {
            return NULL;
        }

        char *cursor = result;
        for (size_t i = 0; i < size; i++)
        {
            cursor += sprintf(cursor, "%d", list[i]);

            if (i < size - 1 && separator)
            {
                memcpy(cursor, separator, sep_len);
                cursor += sep_len;
            }
        }

        return result;
    }

    /**
     * Convert a list of doubles to a string with the given separator
     *
     * @param list Array of doubles
     * @param separator String to insert between elements
     * @param precision Number of decimal places to show
     * @return Newly allocated string with joined doubles (caller must free)
     */
    char *mvn_list_join_double(double *list, const char *separator, int precision)
    {
        if (!list)
        {
            return NULL;
        }

        size_t size = mvn_list_size(list);
        if (size == 0)
        {
            return strdup("");
        }

        // Format string for each double
        char format[32];
        sprintf(format, "%%.%df", precision);

        // Calculate max needed space (30 chars per double + separators)
        size_t sep_len = separator ? strlen(separator) : 0;
        size_t buffer_size = size * 31 + (size - 1) * sep_len + 1;
        char *result = (char *)malloc(buffer_size);
        if (!result)
        {
            return NULL;
        }

        char *cursor = result;
        for (size_t i = 0; i < size; i++)
        {
            cursor += sprintf(cursor, format, list[i]);

            if (i < size - 1 && separator)
            {
                memcpy(cursor, separator, sep_len);
                cursor += sep_len;
            }
        }

        return result;
    }

    /**
     * Convert a list of floats to a string with the given separator
     *
     * @param list Array of floats
     * @param separator String to insert between elements
     * @param precision Number of decimal places to show
     * @return Newly allocated string with joined floats (caller must free)
     */
    char *mvn_list_join_float(float *list, const char *separator, int precision)
    {
        if (!list)
        {
            return NULL;
        }

        size_t size = mvn_list_size(list);
        if (size == 0)
        {
            return strdup("");
        }

        // Format string for each float
        char format[32];
        sprintf(format, "%%.%df", precision);

        // Calculate max needed space (20 chars per float + separators)
        size_t sep_len = separator ? strlen(separator) : 0;
        size_t buffer_size = size * 21 + (size - 1) * sep_len + 1;
        char *result = (char *)malloc(buffer_size);
        if (!result)
        {
            return NULL;
        }

        char *cursor = result;
        for (size_t i = 0; i < size; i++)
        {
            cursor += sprintf(cursor, format, list[i]);

            if (i < size - 1 && separator)
            {
                memcpy(cursor, separator, sep_len);
                cursor += sep_len;
            }
        }

        return result;
    }

    /**
     * Convert a list of strings to a single string with the given separator
     *
     * @param list Array of strings (char*)
     * @param separator String to insert between elements
     * @return Newly allocated string with joined strings (caller must free)
     */
    char *mvn_list_join_str(char **list, const char *separator)
    {
        if (!list)
        {
            return NULL;
        }

        size_t size = mvn_list_size(list);
        if (size == 0)
        {
            return strdup("");
        }

        size_t sep_len = separator ? strlen(separator) : 0;
        size_t total_len = 0;

        // Calculate total length needed
        for (size_t i = 0; i < size; i++)
        {
            if (list[i])
            {
                total_len += strlen(list[i]);
            }
            if (i < size - 1)
            {
                total_len += sep_len;
            }
        }

        // Allocate and build result string
        char *result = (char *)malloc(total_len + 1);
        if (!result)
        {
            return NULL;
        }

        char *cursor = result;
        for (size_t i = 0; i < size; i++)
        {
            if (list[i])
            {
                size_t len = strlen(list[i]);
                memcpy(cursor, list[i], len);
                cursor += len;
            }

            if (i < size - 1 && separator)
            {
                memcpy(cursor, separator, sep_len);
                cursor += sep_len;
            }
        }
        *cursor = '\0';

        return result;
    }

    /**
     * Creates a deep copy of a string array
     *
     * @param src Source string array
     * @return New array with copies of all strings (caller must free both array and strings)
     */
    char **mvn_list_clone_strings(char **src)
    {
        if (!src)
        {
            return NULL;
        }

        size_t size = mvn_list_size(src);
        char **result = NULL;

        for (size_t i = 0; i < size; i++)
        {
            char *copy = src[i] ? strdup(src[i]) : NULL;
            mvn_list_push(result, copy);
        }

        return result;
    }

    /**
     * Frees all strings in an array and the array itself
     *
     * @param arr Array of strings to free
     */
    void mvn_list_free_strings(char **arr)
    {
        if (!arr)
        {
            return;
        }

        size_t size = mvn_list_size(arr);
        for (size_t i = 0; i < size; i++)
        {
            free(arr[i]);
        }
        mvn_list_free(arr);
    }

#ifdef __cplusplus
}
#endif
