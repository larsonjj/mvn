#ifndef MVN_HASHMAP_H
#define MVN_HASHMAP_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // ---------- Internal structures and function declarations ----------

    typedef struct mvn__hm_entry
    {
        char *key;
        int key_len;
        uint32_t hash;
        bool is_present; // Used to mark deleted entries
        // Value is stored after this header
    } mvn__hm_entry;

    typedef struct mvn__hm_header
    {
        size_t capacity;
        size_t count;
        size_t value_size;
        float load_factor_threshold;
        mvn__hm_entry *entries;
    } mvn__hm_header;

#define MVN__HASH_INITIAL_CAPACITY 16
#define MVN__LOAD_FACTOR_THRESHOLD 0.75f
#define MVN__FNV_OFFSET 2166136261U
#define MVN__FNV_PRIME 16777619U

    // Function declarations - not definitions
    uint32_t mvn__hash_string(const char *str);
    mvn__hm_header *mvn__hm_header_of(void *hm);
    void *mvn__hm_get_value_ptr(mvn__hm_entry *entry);
    mvn__hm_entry *mvn__hm_find_entry(mvn__hm_header *header, const char *key, uint32_t hash);
    void mvn__hm_resize(void **hm_ptr, size_t new_capacity);
    void *mvn__hm_new(size_t value_size);
    void mvn__hm_set(void **hm_ptr, const char *key, void *value_ptr, size_t value_size);
    void *mvn__hm_get(void *hm, const char *key);
    bool mvn__hm_del(void **hm_ptr, const char *key);
    void mvn__hm_free(void *hm);
    void mvn__hm_clear(void **hm_ptr);

    // Iterator support
    typedef struct mvn__hm_iter
    {
        void *hm;
        size_t index;
    } mvn__hm_iter;

    mvn__hm_iter mvn__hm_iter_new(void *hm);
    bool mvn__hm_next(mvn__hm_iter *iter, char **key_ptr, void **value_ptr);

#ifdef __cplusplus
}
#endif

// ---------- Public API macros ----------

#ifdef __cplusplus
#define mvn_hmap_new(type) static_cast<type *>(mvn__hm_new(sizeof(type)))
#else
#define mvn_hmap_new(type) (type *)mvn__hm_new(sizeof(type))
#endif

#define mvn_hmap_set(hm, key, value)                                                               \
    do                                                                                             \
    {                                                                                              \
        void **_hm_void = (void **)&(hm);                                                          \
        mvn__hm_set(_hm_void, key, &(value), sizeof(value));                                       \
    } while (0)

#ifdef __cplusplus
#define mvn_hmap_get(hm, key) static_cast<typeof(hm)>(mvn__hm_get((void *)(hm), key))
#else
#define mvn_hmap_get(hm, key) mvn__hm_get(hm, key)
#endif

#define mvn_hmap_len(hm) ((hm) ? mvn__hm_header_of(hm)->count : 0)

#define mvn_hmap_del(hm, key) mvn__hm_del((void **)&(hm), key)

#define mvn_hmap_free(hm)                                                                          \
    do                                                                                             \
    {                                                                                              \
        if (hm)                                                                                    \
        {                                                                                          \
            mvn__hm_free(hm);                                                                      \
            (hm) = NULL;                                                                           \
        }                                                                                          \
    } while (0)

#define mvn_hmap_iter(hm) mvn__hm_iter_new(hm)

#define mvn_hmap_next(iter, key, value) mvn__hm_next(&(iter), (key), (void **)(value))

#define mvn_hmap_has(hm, key) (mvn_hmap_get(hm, key) != NULL)

#define mvn_hmap_clear(hm)                                                                         \
    do                                                                                             \
    {                                                                                              \
        void **_hm_void = (void **)&(hm);                                                          \
        mvn__hm_clear(_hm_void);                                                                   \
    } while (0)

#endif // MVN_HASHMAP_H
