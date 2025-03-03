#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FNV-1a hash implementation for strings
uint32_t mvn__hash_string(const char *str)
{
    uint32_t hash = MVN__FNV_OFFSET;
    while (*str)
    {
        hash ^= (uint8_t)*str++;
        hash *= MVN__FNV_PRIME;
    }
    return hash;
}

// Get the header from a hashmap pointer
mvn__hm_header *mvn__hm_header_of(void *hm)
{
    return hm ? (mvn__hm_header *)((char *)hm - sizeof(mvn__hm_header)) : NULL;
}

// Get pointer to the value in an entry
void *mvn__hm_get_value_ptr(mvn__hm_entry *entry)
{
    return (void *)((char *)entry + sizeof(mvn__hm_entry));
}

// Find an entry for a key (or an empty slot to insert it)
mvn__hm_entry *mvn__hm_find_entry(mvn__hm_header *header, const char *key, uint32_t hash)
{
    size_t mask = header->capacity - 1;
    size_t index = hash & mask;
    mvn__hm_entry *tombstone = NULL;

    for (;;)
    {
        mvn__hm_entry *entry = &header->entries[index];

        if (!entry->key && !entry->is_present)
        {
            // Empty entry
            return tombstone ? tombstone : entry;
        }
        else if (!entry->key && entry->is_present)
        {
            // We found a tombstone
            if (!tombstone)
                tombstone = entry;
        }
        else if (entry->hash == hash && strcmp(entry->key, key) == 0)
        {
            // We found the key
            return entry;
        }

        // Linear probing
        index = (index + 1) & mask;
    }
}

// Create a new hashmap
void *mvn__hm_new(size_t value_size)
{
    size_t allocation_size = sizeof(mvn__hm_header) +
                             MVN__HASH_INITIAL_CAPACITY * sizeof(mvn__hm_entry) +
                             MVN__HASH_INITIAL_CAPACITY * value_size;

    mvn__hm_header *header = (mvn__hm_header *)malloc(allocation_size);
    if (!header)
        return NULL;

    header->capacity = MVN__HASH_INITIAL_CAPACITY;
    header->count = 0;
    header->value_size = value_size;
    header->load_factor_threshold = MVN__LOAD_FACTOR_THRESHOLD;

    // Initialize entries
    header->entries = (mvn__hm_entry *)((char *)header + sizeof(mvn__hm_header));
    memset(header->entries, 0, header->capacity * sizeof(mvn__hm_entry));

    // Return pointer to the first value slot, which is after all the entries
    return (void *)((char *)header + sizeof(mvn__hm_header));
}

// Resize the hashmap
void mvn__hm_resize(void **hm_ptr, size_t new_capacity)
{
    mvn__hm_header *old_header = mvn__hm_header_of(*hm_ptr);
    if (!old_header)
        return;

    size_t allocation_size = sizeof(mvn__hm_header) + new_capacity * sizeof(mvn__hm_entry) +
                             new_capacity * old_header->value_size;

    mvn__hm_header *new_header = (mvn__hm_header *)malloc(allocation_size);
    if (!new_header)
        return; // Failed to allocate

    new_header->capacity = new_capacity;
    new_header->count = 0; // Will be incremented during rehash
    new_header->value_size = old_header->value_size;
    new_header->load_factor_threshold = old_header->load_factor_threshold;

    // Initialize entries
    new_header->entries = (mvn__hm_entry *)((char *)new_header + sizeof(mvn__hm_header));
    memset(new_header->entries, 0, new_capacity * sizeof(mvn__hm_entry));

    // Create new map pointer
    void *new_map = (void *)((char *)new_header + sizeof(mvn__hm_header));

    // Rehash all existing entries
    for (size_t i = 0; i < old_header->capacity; i++)
    {
        mvn__hm_entry *entry = &old_header->entries[i];
        if (entry->key && entry->is_present)
        {
            mvn__hm_entry *dest = mvn__hm_find_entry(new_header, entry->key, entry->hash);

            // Copy the entry
            dest->key = entry->key; // Transfer ownership of the key string
            dest->key_len = entry->key_len;
            dest->hash = entry->hash;
            dest->is_present = true;

            // Copy the value
            memcpy(
                mvn__hm_get_value_ptr(dest), mvn__hm_get_value_ptr(entry), old_header->value_size
            );

            new_header->count++;
        }
    }

    // Free the old header but not the keys as they were transferred
    free(old_header);
    *hm_ptr = new_map;
}

// Set a key-value pair in the hashmap
void mvn__hm_set(void **hm_ptr, const char *key, void *value_ptr, size_t value_size)
{
    mvn__hm_header *header = mvn__hm_header_of(*hm_ptr);
    if (!header)
    {
        *hm_ptr = mvn__hm_new(value_size);
        header = mvn__hm_header_of(*hm_ptr);
        if (!header)
            return; // Failed to allocate
    }

    // Check if we need to resize
    if ((float)(header->count + 1) > (float)(header->capacity) * header->load_factor_threshold)
    {
        mvn__hm_resize(hm_ptr, header->capacity * 2);
        header = mvn__hm_header_of(*hm_ptr);
    }

    uint32_t hash = mvn__hash_string(key);
    mvn__hm_entry *entry = mvn__hm_find_entry(header, key, hash);

    bool is_new_key = !entry->key;
    if (is_new_key)
    {
        // Copy the key
        int key_len = (int32_t)strlen(key) + 1;
        entry->key = (char *)malloc((size_t)key_len);
        if (!entry->key)
            return; // Failed to allocate
        strcpy(entry->key, key);
        entry->key_len = key_len - 1;
        entry->hash = hash;
        entry->is_present = true;
        header->count++;
    }

    // Copy the value
    memcpy(mvn__hm_get_value_ptr(entry), value_ptr, header->value_size);
}

// Get a value from the hashmap by key
void *mvn__hm_get(void *hm, const char *key)
{
    mvn__hm_header *header = mvn__hm_header_of(hm);
    if (!header || header->count == 0)
        return NULL;

    uint32_t hash = mvn__hash_string(key);
    size_t mask = header->capacity - 1;
    size_t index = hash & mask;

    for (;;)
    {
        mvn__hm_entry *entry = &header->entries[index];

        if (!entry->key && !entry->is_present)
        {
            // Empty slot, key doesn't exist
            return NULL;
        }

        if (entry->key && entry->is_present && entry->hash == hash && strcmp(entry->key, key) == 0)
        {
            // Found the key
            return mvn__hm_get_value_ptr(entry);
        }

        // Linear probing
        index = (index + 1) & mask;
    }
}

// Delete a key from the hashmap
bool mvn__hm_del(void **hm_ptr, const char *key)
{
    mvn__hm_header *header = mvn__hm_header_of(*hm_ptr);
    if (!header || header->count == 0)
        return false;

    uint32_t hash = mvn__hash_string(key);
    size_t mask = header->capacity - 1;
    size_t index = hash & mask;

    for (;;)
    {
        mvn__hm_entry *entry = &header->entries[index];

        if (!entry->key && !entry->is_present)
        {
            // Empty slot, key doesn't exist
            return false;
        }

        if (entry->key && entry->is_present && entry->hash == hash && strcmp(entry->key, key) == 0)
        {
            // Found the key, mark as deleted
            free(entry->key);
            entry->key = NULL;
            entry->is_present = true; // Mark as tombstone
            header->count--;
            return true;
        }

        // Linear probing
        index = (index + 1) & mask;
    }
}

// Free the hashmap
void mvn__hm_free(void *hm)
{
    mvn__hm_header *header = mvn__hm_header_of(hm);
    if (!header)
    {
        return;
    }

    // Free all keys (with safety check)
    for (size_t i = 0; i < header->capacity; i++)
    {
        if (header->entries[i].key)
        {
            // Safety check - validate this is a pointer, not a small integer
            if ((uintptr_t)header->entries[i].key > 1000)
            {
                free(header->entries[i].key);
            }
            else
            {
                printf("Warning: Invalid key pointer detected: %p\n", header->entries[i].key);
            }
        }
    }

    // Free the header (which includes entries and values)
    free(header);
}

// Create a new iterator
mvn__hm_iter mvn__hm_iter_new(void *hm)
{
    mvn__hm_iter iter;
    iter.hm = hm;
    iter.index = 0;
    return iter;
}

// Get the next key-value pair from an iterator
bool mvn__hm_next(mvn__hm_iter *iter, char **key_ptr, void **value_ptr)
{
    mvn__hm_header *header = mvn__hm_header_of(iter->hm);
    if (!header)
        return false;

    // Find the next valid entry
    while (iter->index < header->capacity)
    {
        mvn__hm_entry *entry = &header->entries[iter->index];
        iter->index++;

        if (entry->key && entry->is_present)
        {
            if (key_ptr)
                *key_ptr = entry->key;
            if (value_ptr)
                *value_ptr = mvn__hm_get_value_ptr(entry);
            return true;
        }
    }

    return false; // No more entries
}

// Clear all entries from the hashmap
void mvn__hm_clear(void **hm_ptr)
{
    mvn__hm_header *header = mvn__hm_header_of(*hm_ptr);
    if (!header)
        return;

    // Free all keys
    for (size_t i = 0; i < header->capacity; i++)
    {
        if (header->entries[i].key)
        {
            free(header->entries[i].key);
            header->entries[i].key = NULL;
            header->entries[i].is_present = false;
        }
    }

    header->count = 0;
}
