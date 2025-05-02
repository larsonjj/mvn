/**
 * \file            mvn-hashmap.c
 * \brief           Implementation of unordered hashmap for MVN game framework
 */

#include "mvn/mvn-hashmap.h"

#include "mvn/mvn-logger.h"
#include "mvn/mvn-utils.h"

/* Default initial capacity if none is specified */
#define MVN_HMAP_DEFAULT_CAPACITY 16

/* Maximum load factor before resizing */
#define MVN_HMAP_LOAD_FACTOR 0.75

/* Growth factor when resizing */
#define MVN_HMAP_GROWTH_FACTOR 2

/**
 * \brief           Calculate hash for a string key
 * \param[in]       key: String key to hash
 * \return          Hash value for the key
 *
 * Uses FNV-1a hash algorithm
 */
static size_t hash_string(const char *key)
{
    if (!key) {
        return 0;
    }

    /* FNV-1a hash parameters */
    const size_t FNV_PRIME        = 16777619;
    const size_t FNV_OFFSET_BASIS = 2166136261;

    size_t hash = FNV_OFFSET_BASIS;
    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= FNV_PRIME;
    }

    return hash;
}

/**
 * \brief           Create a new entry
 * \param[in]       key: String key for the entry
 * \param[in]       value: Pointer to value data
 * \param[in]       value_size: Size of the value in bytes
 * \return          Newly created entry or NULL on failure
 */
static mvn_hmap_entry_t *create_entry(const char *key, const void *value, size_t value_size)
{
    if (!key || !value) {
        return NULL;
    }

    mvn_hmap_entry_t *entry = MVN_MALLOC(sizeof(mvn_hmap_entry_t));
    if (!entry) {
        mvn_log_error("Failed to allocate memory for hashmap entry");
        return NULL;
    }

    /* Copy key */
    size_t key_len = SDL_strlen(key) + 1;
    entry->key     = MVN_MALLOC(key_len);
    if (!entry->key) {
        mvn_log_error("Failed to allocate memory for hashmap entry key");
        MVN_FREE(entry);
        return NULL;
    }
    SDL_memcpy(entry->key, key, key_len);

    /* Copy value */
    entry->value = MVN_MALLOC(value_size);
    if (!entry->value) {
        mvn_log_error("Failed to allocate memory for hashmap entry value");
        MVN_FREE(entry->key);
        MVN_FREE(entry);
        return NULL;
    }
    SDL_memcpy(entry->value, value, value_size);

    entry->next = NULL;
    return entry;
}

/**
 * \brief           Free a hashmap entry and its resources
 * \param[in]       entry: Hashmap entry to free
 */
static void free_entry(mvn_hmap_entry_t *entry)
{
    if (!entry) {
        return;
    }

    MVN_FREE(entry->key);
    MVN_FREE(entry->value);
    MVN_FREE(entry);
}

/**
 * \brief           Resize the hashmap to a new capacity
 * \param[in]       hmap: Hashmap to resize
 * \param[in]       new_capacity: New capacity for the hashmap
 * \return          true on success, false on failure
 */
static bool resize_hashmap(mvn_hmap_t *hmap, size_t new_capacity)
{
    if (!hmap || new_capacity < hmap->length) {
        return false;
    }

    /* Check for potential integer overflow before allocation */
    if (new_capacity > SIZE_MAX / sizeof(mvn_hmap_entry_t *)) {
        mvn_log_error("Integer overflow detected when calculating hashmap resize capacity");
        return false;
    }

    /* Allocate new buckets array */
    mvn_hmap_entry_t **new_buckets =
        (mvn_hmap_entry_t **)MVN_CALLOC(new_capacity, sizeof(mvn_hmap_entry_t *));
    if (!new_buckets) {
        mvn_log_error("Failed to allocate memory for hashmap resizing");
        return false;
    }

    /* Rehash all entries into new buckets */
    for (size_t i = 0; i < hmap->bucket_count; i++) {
        mvn_hmap_entry_t *entry = hmap->buckets[i];
        while (entry) {
            /* Get the next entry before we modify this one */
            mvn_hmap_entry_t *next = entry->next;

            /* Insert into new buckets */
            size_t new_index       = hash_string(entry->key) % new_capacity;
            entry->next            = new_buckets[new_index];
            new_buckets[new_index] = entry;

            entry = next;
        }
    }

    /* Free old buckets array */
    MVN_FREE((void *)hmap->buckets);

    /* Update hashmap */
    hmap->buckets      = new_buckets;
    hmap->bucket_count = new_capacity;

    mvn_log_debug("Hashmap resized to capacity %zu", new_capacity);
    return true;
}

/**
 * \brief           Initialize a new empty hashmap
 * \param[in]       item_size: Size of each item in bytes
 * \param[in]       initial_capacity: Initial capacity (0 for default)
 * \return          New hashmap or NULL on failure
 */
mvn_hmap_t *mvn_hmap_init(size_t item_size, size_t initial_capacity)
{
    if (item_size == 0) {
        mvn_log_error("Cannot create hashmap with item_size 0");
        return NULL;
    }

    /* Use default capacity if not specified */
    if (initial_capacity == 0) {
        initial_capacity = MVN_HMAP_DEFAULT_CAPACITY;
    }

    /* Check for potential integer overflow before allocating buckets */
    if (initial_capacity > SIZE_MAX / sizeof(mvn_hmap_entry_t *)) {
        mvn_log_error("Integer overflow detected when calculating initial hashmap capacity");
        return NULL;
    }

    /* Allocate hashmap structure */
    mvn_hmap_t *hmap = MVN_MALLOC(sizeof(mvn_hmap_t));
    if (!hmap) {
        mvn_log_error("Failed to allocate memory for hashmap");
        return NULL;
    }

    /* Allocate buckets array (initialized to NULL) */
    hmap->buckets = (mvn_hmap_entry_t **)MVN_CALLOC(initial_capacity, sizeof(mvn_hmap_entry_t *));
    if (!hmap->buckets) {
        mvn_log_error("Failed to allocate memory for hashmap buckets");
        MVN_FREE(hmap);
        return NULL;
    }

    hmap->bucket_count = initial_capacity;
    hmap->length       = 0;
    hmap->item_size    = item_size;

    mvn_log_debug(
        "Hashmap initialized with item_size=%zu, capacity=%zu", item_size, initial_capacity);
    return hmap;
}

/**
 * \brief           Free a hashmap and all its resources
 * \param[in]       hmap: Hashmap to free
 */
void mvn_hmap_free(mvn_hmap_t *hmap)
{
    if (!hmap) {
        return;
    }

    /* Free all entries */
    for (size_t i = 0; i < hmap->bucket_count; i++) {
        mvn_hmap_entry_t *entry = hmap->buckets[i];
        while (entry) {
            mvn_hmap_entry_t *next = entry->next;
            free_entry(entry);
            entry = next;
        }
    }

    /* Free buckets array and hashmap structure */
    MVN_FREE((void *)hmap->buckets);
    MVN_FREE(hmap);

    mvn_log_debug("Hashmap freed");
}

/**
 * \brief           Get the number of entries in the hashmap
 * \param[in]       hmap: Hashmap to query
 * \return          Number of entries, 0 if hmap is NULL
 */
size_t mvn_hmap_length(const mvn_hmap_t *hmap)
{
    if (!hmap) {
        return 0;
    }
    return hmap->length;
}

/**
 * \brief           Set a value in the hashmap
 * \param[in]       hmap: Hashmap to modify
 * \param[in]       key: String key
 * \param[in]       value: Pointer to value (will be copied)
 * \return          true if successful, false otherwise
 */
bool mvn_hmap_set(mvn_hmap_t *hmap, const char *key, const void *value)
{
    if (!hmap || !key || !value) {
        mvn_log_error("Invalid parameters for hashmap set operation");
        return false;
    }

    /* Check if we need to resize */
    if (hmap->length > (size_t)((double)hmap->bucket_count * MVN_HMAP_LOAD_FACTOR)) {
        size_t new_capacity = hmap->bucket_count * MVN_HMAP_GROWTH_FACTOR;
        if (!resize_hashmap(hmap, new_capacity)) {
            mvn_log_error("Failed to resize hashmap during set operation");
            return false;
        }
    }

    /* Calculate bucket index */
    size_t index = hash_string(key) % hmap->bucket_count;

    /* Check if key already exists */
    mvn_hmap_entry_t *entry = hmap->buckets[index];
    while (entry) {
        if (SDL_strcmp(entry->key, key) == 0) {
            /* Update existing entry */
            SDL_memcpy(entry->value, value, hmap->item_size);
            return true;
        }
        entry = entry->next;
    }

    /* Create new entry */
    mvn_hmap_entry_t *new_entry = create_entry(key, value, hmap->item_size);
    if (!new_entry) {
        return false;
    }

    /* Insert at the beginning of the linked list */
    new_entry->next      = hmap->buckets[index];
    hmap->buckets[index] = new_entry;
    hmap->length++;

    return true;
}

/**
 * \brief           Get a value from the hashmap
 * \param[in]       hmap: Hashmap to query
 * \param[in]       key: String key
 * \return          Pointer to value or NULL if not found
 */
void *mvn_hmap_get(const mvn_hmap_t *hmap, const char *key)
{
    if (!hmap || !key) {
        return NULL;
    }

    /* Calculate bucket index */
    size_t index = hash_string(key) % hmap->bucket_count;

    /* Search for key in the bucket */
    mvn_hmap_entry_t *entry = hmap->buckets[index];
    while (entry) {
        if (SDL_strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    /* Key not found */
    return NULL;
}

/**
 * \brief           Delete an entry from the hashmap
 * \param[in]       hmap: Hashmap to modify
 * \param[in]       key: String key
 * \return          true if entry was deleted, false if not found or error
 */
bool mvn_hmap_delete(mvn_hmap_t *hmap, const char *key)
{
    if (!hmap || !key) {
        return false;
    }

    /* Calculate bucket index */
    size_t index = hash_string(key) % hmap->bucket_count;

    /* Handle first entry in bucket */
    mvn_hmap_entry_t *entry = hmap->buckets[index];
    if (!entry) {
        return false; /* Bucket is empty */
    }

    if (SDL_strcmp(entry->key, key) == 0) {
        /* First entry matches */
        hmap->buckets[index] = entry->next;
        free_entry(entry);
        hmap->length--;
        return true;
    }

    /* Search rest of the bucket */
    mvn_hmap_entry_t *prev = entry;
    entry                  = entry->next;
    while (entry) {
        if (SDL_strcmp(entry->key, key) == 0) {
            /* Found the entry */
            prev->next = entry->next;
            free_entry(entry);
            hmap->length--;
            return true;
        }
        prev  = entry;
        entry = entry->next;
    }

    /* Key not found */
    return false;
}

/**
 * \brief           Get a list of keys in the hashmap
 * \param[in]       hmap: Hashmap to query
 * \return          List of string keys or NULL on failure
 */
mvn_list_t *mvn_hmap_keys(const mvn_hmap_t *hmap)
{
    if (!hmap) {
        return NULL;
    }

    /* Create a list to hold all keys */
    mvn_list_t *keys = mvn_list_init(sizeof(char *), hmap->length);
    if (!keys) {
        mvn_log_error("Failed to create list for hashmap keys");
        return NULL;
    }

    /* Iterate through all buckets and entries */
    for (size_t i = 0; i < hmap->bucket_count; i++) {
        mvn_hmap_entry_t *entry = hmap->buckets[i];
        while (entry) {
            /* Copy the key string */
            char *key_copy = SDL_strdup(entry->key);
            if (!key_copy) {
                mvn_log_error("Failed to copy key for hashmap keys list");
                // Free previously added keys before freeing the list
                for (size_t j = 0; j < mvn_list_length(keys); ++j) {
                    char **item = (char **)mvn_list_get(keys, j);
                    if (item && *item) {
                        MVN_FREE(*item);
                    }
                }
                mvn_list_free(keys);
                return NULL;
            }

            /* Add to list */
            if (!mvn_list_push(keys, (const void *)&key_copy)) {
                mvn_log_error("Failed to add key to hashmap keys list");
                MVN_FREE(key_copy); // Free the key that failed to be added
                // Free previously added keys before freeing the list
                for (size_t j = 0; j < mvn_list_length(keys); ++j) {
                    char **item = (char **)mvn_list_get(keys, j);
                    if (item && *item) {
                        MVN_FREE(*item);
                    }
                }
                mvn_list_free(keys);
                return NULL;
            }

            entry = entry->next;
        }
    }

    return keys;
}

/**
 * \brief           Get a list of values in the hashmap
 * \param[in]       hmap: Hashmap to query
 * \return          List of values or NULL on failure
 */
mvn_list_t *mvn_hmap_values(const mvn_hmap_t *hmap)
{
    if (!hmap) {
        return NULL;
    }

    /* Create a list to hold all values */
    mvn_list_t *values = mvn_list_init(hmap->item_size, hmap->length);
    if (!values) {
        mvn_log_error("Failed to create list for hashmap values");
        return NULL;
    }

    /* Iterate through all buckets and entries */
    for (size_t i = 0; i < hmap->bucket_count; i++) {
        mvn_hmap_entry_t *entry = hmap->buckets[i];
        while (entry) {
            /* Add value to list */
            if (!mvn_list_push(values, entry->value)) {
                mvn_log_error("Failed to add value to hashmap values list");
                mvn_list_free(values);
                return NULL;
            }

            entry = entry->next;
        }
    }

    return values;
}
