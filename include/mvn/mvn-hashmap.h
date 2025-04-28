/**
 * \file            mvn-hashmap.h
 * \brief           Unordered hashmap implementation for MVN game framework
 */

#ifndef MVN_HASHMAP_H
#define MVN_HASHMAP_H

#include <SDL3/SDL.h>
#include "mvn/mvn-list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Hashmap entry structure (key-value pair)
 */
typedef struct mvn_hmap_entry_t {
    char* key;                     /*!< String key (owned by the hashmap) */
    void* value;                   /*!< Pointer to value (owned by the hashmap) */
    struct mvn_hmap_entry_t* next; /*!< Next entry in collision chain */
} mvn_hmap_entry_t;

/**
* \brief           Unordered hashmap structure
*/
typedef struct mvn_hmap_t {
    mvn_hmap_entry_t** buckets; /*!< Array of buckets */
    size_t bucket_count;        /*!< Number of buckets */
    size_t length;              /*!< Number of entries */
    size_t item_size;           /*!< Size of each item in bytes */
} mvn_hmap_t;

mvn_hmap_t* mvn_hmap_init(size_t item_size, size_t initial_capacity);
void mvn_hmap_free(mvn_hmap_t* hmap);
size_t mvn_hmap_length(const mvn_hmap_t* hmap);
bool mvn_hmap_set(mvn_hmap_t* hmap, const char* key, const void* value);
void* mvn_hmap_get(const mvn_hmap_t* hmap, const char* key);
bool mvn_hmap_delete(mvn_hmap_t* hmap, const char* key);
mvn_list_t* mvn_hmap_keys(const mvn_hmap_t* hmap);
mvn_list_t* mvn_hmap_values(const mvn_hmap_t* hmap);

/**
 * \brief           Create a hashmap for a specific type
 * \param[in]       T: Type of the hashmap elements
 * \param[in]       capacity: Initial capacity
 * \return          Initialized hashmap
 * \hideinitializer
 */
#define MVN_HMAP_INIT(T, capacity) mvn_hmap_init(sizeof(T), (capacity))

/**
 * \brief           Get the typed pointer to a value at key
 * \param[in]       T: Type of the hashmap elements
 * \param[in]       hmap: Hashmap
 * \param[in]       key: String key to look up
 * \return          Typed pointer to the value or NULL if not found
 * \hideinitializer
 */
#define MVN_HMAP_GET(T, hmap, key) ((T*)mvn_hmap_get((hmap), (key)))

/**
 * \brief           Set a value in the hashmap
 * \param[in]       hmap: Hashmap
 * \param[in]       key: String key
 * \param[in]       T: Type of the hashmap elements
 * \param[in]       value: Value to set
 * \return          true on success, false on failure
 * \hideinitializer
 */
#define MVN_HMAP_SET(hmap, key, T, value)                                                          \
    do {                                                                                           \
        T _temp_value = (value);                                                                   \
        mvn_hmap_set((hmap), (key), &_temp_value);                                                 \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* MVN_HASHMAP_H */
