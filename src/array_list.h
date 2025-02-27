/**
 * array_list.h - Dynamic array implementation in C using stb_ds style macros
 *
 * This header provides a complete dynamic array implementation that works with any type.
 * The API is inspired by JavaScript arrays and the stb_ds.h library.
 */

#ifndef MVN_ARRAY_LIST_H
#define MVN_ARRAY_LIST_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h> /* For size_t */
#include <stdlib.h>
#include <string.h>

/* Type compatibility macros for C99/C++ */
#ifdef __cplusplus
#define MVN_TYPEOF(x) decltype(x)
#else
#define MVN_TYPEOF(x) __typeof__(x)
#endif

/* Statement expression compatibility */
#ifdef _MSC_VER
/* MSVC doesn't support statement expressions */
#define MVN_EXPR_BEGIN do {
#define MVN_EXPR_END(result)                                                                       \
    result;                                                                                        \
    }                                                                                              \
    while (0)
#define MVN_EXPR_RETURN(x) x
#else
/* GCC/Clang support statement expressions */
#define MVN_EXPR_BEGIN ({
#define MVN_EXPR_END(result)                                                                       \
    result;                                                                                        \
    })
#define MVN_EXPR_RETURN(x) x
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Internal structure definitions
 */

/* Header structure that's stored before the array pointer */
typedef struct {
    size_t size;
    size_t capacity;
#ifdef __cplusplus
    char data[1]; /* C++ compatible approach */
#else
    char data[]; /* C99 flexible array member */
#endif
} MVN_List_Header;

/* Structure for growth parameters */
typedef struct {
    size_t elem_size; /* Size of each element in bytes */
    size_t add_len;   /* Number of elements to add capacity for */
} MVN_List_GrowParams;

/* Helper for compound literals in C++ */
#ifdef __cplusplus
static inline MVN_List_GrowParams mvn__make_grow_params(size_t elem_size, size_t add_len)
{
    MVN_List_GrowParams params;
    params.elem_size = elem_size;
    params.add_len = add_len;
    return params;
}
#define MVN_MAKE_GROW_PARAMS(size, len) mvn__make_grow_params(size, len)
#else
#define MVN_MAKE_GROW_PARAMS(size, len)                                                            \
    ((MVN_List_GrowParams){.elem_size = (size), .add_len = (len)})
#endif

/* Core internal functions */
#define mvn__list_header(a) ((MVN_List_Header *)((char *)(a) - sizeof(MVN_List_Header)))
#define mvn__list_size_of(a) ((a) ? mvn__list_header(a)->size : 0)
#define mvn__list_capacity_of(a) ((a) ? mvn__list_header(a)->capacity : 0)
#define mvn__list_grow(a, need)                                                                    \
    ((a) = mvn__list_growf_((a), MVN_MAKE_GROW_PARAMS(sizeof(*(a)), (need))))
#define mvn__list_maybe_grow(a, n)                                                                 \
    (mvn__list_size_of(a) + (n) > mvn__list_capacity_of(a) ? mvn__list_grow((a), (n)) : 0)

/* Function declaration - implementation is in array_list.c */
void *mvn__list_growf_(void *list, MVN_List_GrowParams params);

/*
 * Basic operations
 */

#define mvn_list_new() NULL

#define mvn_list_free(a) ((a) ? (free(mvn__list_header(a)), (a) = NULL) : 0)

#define mvn_list_size(a) mvn__list_size_of(a)

#define mvn_list_capacity(a) mvn__list_capacity_of(a)

#define mvn_list_empty(a) (mvn_list_size(a) == 0)

#define mvn_list_reserve(a, n)                                                                     \
    ((n) > mvn__list_capacity_of(a) ? mvn__list_grow((a), (n)-mvn_list_size(a)) : 0)

#define mvn_list_clear(a) ((a) ? (mvn__list_header(a)->size = 0) : 0)

#define mvn_list_resize(a, n)                                                                      \
    ((a) ? (mvn__list_maybe_grow((a), (n) > mvn_list_size(a) ? (n)-mvn_list_size(a) : 0),          \
            mvn__list_header(a)->size = (n))                                                       \
         : ((n) > 0 ? (mvn__list_grow((a), (n)), mvn__list_header(a)->size = (n), 0) : 0))

#define mvn_list_push(a, v) (mvn__list_maybe_grow((a), 1), (a)[mvn__list_header(a)->size++] = (v))

#define mvn_list_pop(a)                                                                            \
    ((a) && mvn_list_size(a) > 0 ? (a)[--mvn__list_header(a)->size]                                \
                                 : (assert(!"Cannot pop from empty list"), (a)[0]))

#define mvn_list_get(a, i) ((a)[i])

#define mvn_list_set(a, i, v) ((i) < mvn_list_size(a) ? (a)[i] = (v), true : false)

#define mvn_list_first(a)                                                                          \
    ((a) && mvn_list_size(a) > 0 ? (a)[0] : (assert(!"List is empty"), (a)[0]))

#define mvn_list_last(a)                                                                           \
    ((a) && mvn_list_size(a) > 0 ? (a)[mvn_list_size(a) - 1] : (assert(!"List is empty"), (a)[0]))

#define mvn_list_insert_at(a, i, v)                                                                \
    ((i) <= mvn_list_size(a)                                                                       \
         ? (mvn__list_maybe_grow((a), 1),                                                          \
            memmove(&(a)[(i) + 1], &(a)[i], (mvn_list_size(a) - (i)) * sizeof(*(a))),              \
            (a)[i] = (v), ++mvn__list_header(a)->size, true)                                       \
         : false)

#define mvn_list_remove_at(a, i)                                                                   \
    ((a) && (i) < mvn_list_size(a)                                                                 \
         ? (memmove(&(a)[i], &(a)[(i) + 1], (mvn_list_size(a) - (i)-1) * sizeof(*(a))),            \
            --mvn__list_header(a)->size, true)                                                     \
         : false)

#define mvn_list_shift(a)                                                                          \
    ((a) && mvn_list_size(a) > 0                                                                   \
         ? (memmove(&(a)[0], &(a)[1], (mvn_list_size(a) - 1) * sizeof(*(a))),                      \
            --mvn__list_header(a)->size, true)                                                     \
         : false)

#define mvn_list_unshift(a, v)                                                                     \
    ((a) || true ? (mvn__list_maybe_grow((a), 1),                                                  \
                    memmove(&(a)[1], &(a)[0], mvn_list_size(a) * sizeof(*(a))), (a)[0] = (v),      \
                    ++mvn__list_header(a)->size, true)                                             \
                 : false)

#ifdef __cplusplus
#define mvn_list_swap(a, i, j)                                                                     \
    ((a) && (i) < mvn_list_size(a) && (j) < mvn_list_size(a) && (i) != (j)                         \
         ? (std::swap((a)[i], (a)[j]), true)                                                       \
         : false)
#else
#define mvn_list_swap(a, i, j)                                                                     \
    MVN_EXPR_BEGIN                                                                                 \
    bool result = false;                                                                           \
    if ((a) && (i) < mvn_list_size(a) && (j) < mvn_list_size(a) && (i) != (j)) {                   \
        MVN_TYPEOF((a)[0]) tmp = (a)[i];                                                           \
        (a)[i] = (a)[j];                                                                           \
        (a)[j] = tmp;                                                                              \
        result = true;                                                                             \
    }                                                                                              \
    MVN_EXPR_END(result)
#endif

/*
 * Search operations
 */
#define mvn_list_includes(a, v)                                                                    \
    MVN_EXPR_BEGIN                                                                                 \
    bool found = false;                                                                            \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        if ((a)[i] == (v)) {                                                                       \
            found = true;                                                                          \
            break;                                                                                 \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(found)

#define mvn_list_index_of(a, v)                                                                    \
    MVN_EXPR_BEGIN                                                                                 \
    int idx = -1;                                                                                  \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        if ((a)[i] == (v)) {                                                                       \
            idx = (int)i;                                                                          \
            break;                                                                                 \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(idx)

#define mvn_list_last_index_of(a, v)                                                               \
    MVN_EXPR_BEGIN                                                                                 \
    int idx = -1;                                                                                  \
    for (size_t i = mvn_list_size(a); i-- > 0;) {                                                  \
        if ((a)[i] == (v)) {                                                                       \
            idx = (int)i;                                                                          \
            break;                                                                                 \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(idx)

#define mvn_list_find(a, predicate)                                                                \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF((a)[0]) *result = NULL;                                                             \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        if (predicate((a)[i])) {                                                                   \
            result = &(a)[i];                                                                      \
            break;                                                                                 \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_find_index(a, predicate)                                                          \
    MVN_EXPR_BEGIN                                                                                 \
    int idx = -1;                                                                                  \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        if (predicate((a)[i])) {                                                                   \
            idx = (int)i;                                                                          \
            break;                                                                                 \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(idx)

#define mvn_list_binary_search(a, v, comparator)                                                   \
    MVN_EXPR_BEGIN                                                                                 \
    int left = 0;                                                                                  \
    int right = (int)mvn_list_size(a) - 1;                                                         \
    int idx = -1;                                                                                  \
    while (left <= right) {                                                                        \
        int mid = left + (right - left) / 2;                                                       \
        int cmp = comparator(&(v), &(a)[mid]);                                                     \
        if (cmp == 0) {                                                                            \
            idx = mid;                                                                             \
            break;                                                                                 \
        }                                                                                          \
        if (cmp < 0)                                                                               \
            right = mid - 1;                                                                       \
        else                                                                                       \
            left = mid + 1;                                                                        \
    }                                                                                              \
    MVN_EXPR_END(idx)

#define mvn_list_reverse(a)                                                                        \
    MVN_EXPR_BEGIN                                                                                 \
    if ((a) && mvn_list_size(a) > 1) {                                                             \
        size_t i = 0, j = mvn_list_size(a) - 1;                                                    \
        while (i < j) {                                                                            \
            MVN_TYPEOF((a)[0]) tmp = (a)[i];                                                       \
            (a)[i] = (a)[j];                                                                       \
            (a)[j] = tmp;                                                                          \
            i++;                                                                                   \
            j--;                                                                                   \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END((void)0)

#define mvn_list_foreach(a, func)                                                                  \
    MVN_EXPR_BEGIN                                                                                 \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        func(&(a)[i]);                                                                             \
    }                                                                                              \
    MVN_EXPR_END((void)0)

#define mvn_list_sort(a, comparator)                                                               \
    ((a) && mvn_list_size(a) > 1 ? qsort((a), mvn_list_size(a), sizeof(*(a)), comparator) : 0)

#define mvn_list_clone(a)                                                                          \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF(a) result = NULL;                                                                   \
    if (a) {                                                                                       \
        mvn_list_reserve(result, mvn_list_size(a));                                                \
        for (size_t i = 0; i < mvn_list_size(a); ++i) {                                            \
            mvn_list_push(result, (a)[i]);                                                         \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_unique(a)                                                                         \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF(a) result = NULL;                                                                   \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        bool duplicate = false;                                                                    \
        for (size_t j = 0; j < mvn_list_size(result); ++j) {                                       \
            if ((result)[j] == (a)[i]) {                                                           \
                duplicate = true;                                                                  \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
        if (!duplicate)                                                                            \
            mvn_list_push(result, (a)[i]);                                                         \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_map(a, func)                                                                      \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF(a) result = NULL;                                                                   \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        mvn_list_push(result, func((a)[i]));                                                       \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_filter(a, predicate)                                                              \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF(a) result = NULL;                                                                   \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        if (predicate((a)[i])) {                                                                   \
            mvn_list_push(result, (a)[i]);                                                         \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_concat(a, b)                                                                      \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF(a) result = mvn_list_clone(a);                                                      \
    for (size_t i = 0; i < mvn_list_size(b); ++i) {                                                \
        mvn_list_push(result, (b)[i]);                                                             \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_slice(a, start, end)                                                              \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF(a) result = NULL;                                                                   \
    size_t s = (start);                                                                            \
    size_t e = (end);                                                                              \
    size_t a_size = mvn_list_size(a);                                                              \
    if (s > a_size)                                                                                \
        s = a_size;                                                                                \
    if (e > a_size)                                                                                \
        e = a_size;                                                                                \
    if (s < e) {                                                                                   \
        mvn_list_reserve(result, e - s);                                                           \
        for (size_t i = s; i < e; ++i) {                                                           \
            mvn_list_push(result, (a)[i]);                                                         \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_fill(a, v, start, count)                                                          \
    MVN_EXPR_BEGIN                                                                                 \
    size_t s = (start);                                                                            \
    size_t c = (count);                                                                            \
    size_t end = s + c;                                                                            \
    if (end > mvn_list_capacity(a)) {                                                              \
        mvn_list_reserve(a, end);                                                                  \
    }                                                                                              \
    if (end > mvn_list_size(a)) {                                                                  \
        mvn__list_header(a)->size = end;                                                           \
    }                                                                                              \
    for (size_t i = s; i < end; ++i) {                                                             \
        (a)[i] = (v);                                                                              \
    }                                                                                              \
    MVN_EXPR_END((void)0)

#define mvn_list_reduce(a, initial, reducer)                                                       \
    MVN_EXPR_BEGIN                                                                                 \
    MVN_TYPEOF(initial) result = (initial);                                                        \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        result = reducer(result, (a)[i]);                                                          \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_every(a, predicate)                                                               \
    MVN_EXPR_BEGIN                                                                                 \
    bool result = true;                                                                            \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        if (!predicate((a)[i])) {                                                                  \
            result = false;                                                                        \
            break;                                                                                 \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(result)

#define mvn_list_some(a, predicate)                                                                \
    MVN_EXPR_BEGIN                                                                                 \
    bool result = false;                                                                           \
    for (size_t i = 0; i < mvn_list_size(a); ++i) {                                                \
        if (predicate((a)[i])) {                                                                   \
            result = true;                                                                         \
            break;                                                                                 \
        }                                                                                          \
    }                                                                                              \
    MVN_EXPR_END(result)

/* Function declarations for utility functions */
char *mvn_list_join_str(
    const void *arr, size_t elem_size, const char *separator, char *(*to_string)(const void *elem)
);
char *mvn_list_join_int(int *list, const char *separator);
char *mvn_list_join_double(double *list, const char *separator, int precision);
char *mvn_list_join_str_array(char **list, const char *separator);
char **mvn_list_clone_strings(char **src);
void mvn_list_free_strings(char **arr);

#ifdef __cplusplus
}
#endif

#endif /* MVN_ARRAY_LIST_H */
