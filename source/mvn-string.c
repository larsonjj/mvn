/**
 * \file            mvn-string.c
 * \brief           Implementation of dynamic string for MVN game framework
 */

#include <SDL3/SDL.h>
#include "mvn/mvn-list.h"
#include "mvn/mvn-logger.h"
#include "mvn/mvn-string.h"
#include "mvn/mvn-utils.h"

/* Default initial capacity if none is specified */
#define MVN_STRING_DEFAULT_CAPACITY 16

/* Growth factor when resizing */
#define MVN_STRING_GROWTH_FACTOR    2

/* Empty string constant for safety */
static const char EMPTY_STRING[] = "";

/* Helper function prototypes */
static bool mvn_string_ensure_capacity(mvn_string_t* str, size_t needed_capacity);
static bool mvn_string_resize(mvn_string_t* str, size_t new_capacity);
static bool is_whitespace(char character);

/**
 * \brief           Create a deep copy of a string
 * \param[in]       str: String to clone
 * \return          New string or NULL on failure
 */
mvn_string_t*
mvn_string_clone(const mvn_string_t* str) {
    if (!str) {
        return NULL;
    }

    mvn_string_t* clone = mvn_string_init(str->capacity);
    if (!clone) {
        return NULL;
    }

    SDL_memcpy(clone->data, str->data, str->length + 1); // +1 for null terminator
    clone->length = str->length;

    return clone;
}

/**
 * \brief           Helper function to free resources in a list of strings
 * \param[in]       list: List containing mvn_string_t* elements
 */
static void
mvn_free_string_list(mvn_list_t* list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < mvn_list_length(list); ++i) {
        mvn_string_t** item = MVN_LIST_GET(mvn_string_t*, list, i);
        if (item && *item) {
            mvn_string_free(*item);
        }
    }
    mvn_list_free(list);
}

/**
 * \brief           Initialize a new empty string
 * \param[in]       initial_capacity: Initial capacity for the string buffer
 * \return          Newly created string or NULL on failure
 */
mvn_string_t*
mvn_string_init(size_t initial_capacity) {
    /* Use default capacity if not specified */
    if (initial_capacity == 0) {
        initial_capacity = MVN_STRING_DEFAULT_CAPACITY;
    }

    /* Allocate string structure */
    mvn_string_t* str = MVN_MALLOC(sizeof(mvn_string_t));
    if (!str) {
        mvn_log_error("Failed to allocate memory for string");
        return NULL;
    }

    /* Allocate data array */
    str->data = MVN_MALLOC(initial_capacity);
    if (!str->data) {
        mvn_log_error("Failed to allocate memory for string data");
        MVN_FREE(str);
        return NULL;
    }

    /* Initialize with empty string */
    str->data[0] = '\0';
    str->length = 0;
    str->capacity = initial_capacity;

    mvn_log_debug("String initialized with capacity=%zu", initial_capacity);
    return str;
}

/**
 * \brief           Initialize a string from a C string
 * \param[in]       cstr: C string to copy
 * \return          Newly created string or NULL on failure
 */
mvn_string_t*
mvn_string_from_cstr(const char* cstr) {
    if (!cstr) {
        cstr = EMPTY_STRING;
    }

    size_t len = SDL_strlen(cstr);
    size_t needed_capacity = len + 1; /* +1 for null terminator */

    mvn_string_t* str = mvn_string_init(needed_capacity);
    if (!str) {
        return NULL;
    }

    SDL_memcpy(str->data, cstr, needed_capacity);
    str->length = len;

    return str;
}

/**
 * \brief           Free a string and all its resources
 * \param[in]       str: String to free
 */
void
mvn_string_free(mvn_string_t* str) {
    if (!str) {
        return;
    }

    if (str->data) {
        MVN_FREE(str->data);
    }

    MVN_FREE(str);
    mvn_log_debug("String freed");
}

/**
 * \brief           Ensure the string has enough capacity
 * \param[in]       str: String to check capacity for
 * \param[in]       needed_capacity: Minimum required capacity
 * \return          true on success, false on failure
 */
static bool
mvn_string_ensure_capacity(mvn_string_t* str, size_t needed_capacity) {
    if (!str) {
        mvn_log_error("NULL string provided to ensure_capacity");
        return false;
    }

    if (needed_capacity <= str->capacity) {
        return true; /* Already has enough capacity */
    }

    size_t new_capacity = str->capacity;
    while (new_capacity < needed_capacity) {
        new_capacity *= MVN_STRING_GROWTH_FACTOR;
    }

    return mvn_string_resize(str, new_capacity);
}

/**
 * \brief           Resize the string capacity
 * \param[in]       str: String to resize
 * \param[in]       new_capacity: New capacity for the string
 * \return          true on success, false on failure
 */
static bool
mvn_string_resize(mvn_string_t* str, size_t new_capacity) {
    if (!str) {
        mvn_log_error("NULL string provided to resize");
        return false;
    }

    /* Don't shrink below current length + null terminator */
    if (new_capacity < str->length + 1) {
        new_capacity = str->length + 1;
    }

    char* new_data = MVN_REALLOC(str->data, new_capacity);
    if (!new_data) {
        mvn_log_error("Failed to resize string to capacity %zu", new_capacity);
        return false;
    }

    str->data = new_data;
    str->capacity = new_capacity;

    mvn_log_debug("String resized to capacity %zu", new_capacity);
    return true;
}

/**
 * \brief           Get the length of the string
 * \param[in]       str: String to get length of
 * \return          Length of the string, 0 if str is NULL
 */
size_t
mvn_string_length(const mvn_string_t* str) {
    if (!str) {
        return 0;
    }
    return str->length;
}

/**
 * \brief           Get raw C string pointer from mvn_string
 * \param[in]       str: String to get C string from
 * \return          C string, empty string if str is NULL
 */
const char*
mvn_string_to_cstr(const mvn_string_t* str) {
    if (!str || !str->data) {
        return EMPTY_STRING;
    }
    return str->data;
}

/**
 * \brief           Concatenate two strings
 * \param[in]       str1: First string
 * \param[in]       str2: Second string
 * \return          New string with concatenated content or NULL on failure
 */
mvn_string_t*
mvn_string_concat(const mvn_string_t* str1, const mvn_string_t* str2) {
    if (!str1) {
        return str2 ? mvn_string_from_cstr(mvn_string_to_cstr(str2)) : NULL;
    }
    if (!str2) {
        return mvn_string_from_cstr(mvn_string_to_cstr(str1));
    }

    size_t len1 = str1->length;
    size_t len2 = str2->length;
    size_t total_len = len1 + len2;

    mvn_string_t* result = mvn_string_init(total_len + 1);
    if (!result) {
        return NULL;
    }

    SDL_memcpy(result->data, str1->data, len1);
    SDL_memcpy(result->data + len1, str2->data, len2);
    result->data[total_len] = '\0';
    result->length = total_len;

    return result;
}

/**
 * \brief           Append C string to mvn_string
 * \param[in,out]   str: String to append to
 * \param[in]       cstr: C string to append
 * \return          true on success, false on failure
 */
bool
mvn_string_append(mvn_string_t* str, const char* cstr) {
    if (!str || !cstr) {
        return false;
    }

    size_t cstr_len = SDL_strlen(cstr);
    if (cstr_len == 0) {
        return true; /* Nothing to append */
    }

    size_t new_length = str->length + cstr_len;
    if (!mvn_string_ensure_capacity(str, new_length + 1)) {
        return false;
    }

    SDL_memcpy(str->data + str->length, cstr, cstr_len + 1); /* Include null terminator */
    str->length = new_length;

    return true;
}

/**
 * \brief           Check if string ends with a suffix
 * \param[in]       str: String to check
 * \param[in]       suffix: Suffix to check for
 * \return          true if string ends with suffix, false otherwise
 */
bool
mvn_string_ends_with(const mvn_string_t* str, const char* suffix) {
    if (!str || !suffix) {
        return false;
    }

    size_t suffix_len = SDL_strlen(suffix);
    if (suffix_len == 0) {
        return true; /* Empty suffix is always a match */
    }

    if (suffix_len > str->length) {
        return false;
    }

    return (SDL_memcmp(str->data + (str->length - suffix_len), suffix, suffix_len) == 0);
}

/**
 * \brief           Check if string starts with a prefix
 * \param[in]       str: String to check
 * \param[in]       prefix: Prefix to check for
 * \return          true if string starts with prefix, false otherwise
 */
bool
mvn_string_starts_with(const mvn_string_t* str, const char* prefix) {
    if (!str || !prefix) {
        return false;
    }

    size_t prefix_len = SDL_strlen(prefix);
    if (prefix_len == 0) {
        return true; /* Empty prefix is always a match */
    }

    if (prefix_len > str->length) {
        return false;
    }

    return (SDL_memcmp(str->data, prefix, prefix_len) == 0);
}

/**
 * \brief           Check if string includes a substring
 * \param[in]       str: String to check
 * \param[in]       substr: Substring to check for
 * \return          true if string includes substring, false otherwise
 */
bool
mvn_string_includes(const mvn_string_t* str, const char* substr) {
    if (!str || !substr) {
        return false;
    }

    if (substr[0] == '\0') {
        return true; /* Empty substring is always included */
    }

    return (SDL_strstr(str->data, substr) != NULL);
}

/**
 * \brief           Pad the end of string with a character to reach a target length
 * \param[in]       str: String to pad
 * \param[in]       target_length: Target length after padding
 * \param[in]       pad_char: Character to use for padding
 * \return          New padded string or NULL on failure
 */
mvn_string_t*
mvn_string_pad_end(const mvn_string_t* str, size_t target_length, char pad_char) {
    if (!str) {
        return NULL;
    }

    if (str->length >= target_length) {
        return mvn_string_clone(str); /* No padding needed */
    }

    mvn_string_t* result = mvn_string_init(target_length + 1);
    if (!result) {
        return NULL;
    }

    /* Copy original string */
    SDL_memcpy(result->data, str->data, str->length);

    /* Add padding */
    for (size_t i = str->length; i < target_length; i++) {
        result->data[i] = pad_char;
    }

    result->data[target_length] = '\0';
    result->length = target_length;

    return result;
}

/**
 * \brief           Pad the start of string with a character to reach a target length
 * \param[in]       str: String to pad
 * \param[in]       target_length: Target length after padding
 * \param[in]       pad_char: Character to use for padding
 * \return          New padded string or NULL on failure
 */
mvn_string_t*
mvn_string_pad_start(const mvn_string_t* str, size_t target_length, char pad_char) {
    if (!str) {
        return NULL;
    }

    if (str->length >= target_length) {
        return mvn_string_clone(str); /* No padding needed */
    }

    mvn_string_t* result = mvn_string_init(target_length + 1);
    if (!result) {
        return NULL;
    }

    size_t pad_length = target_length - str->length;

    /* Add padding */
    for (size_t i = 0; i < pad_length; i++) {
        result->data[i] = pad_char;
    }

    /* Copy original string */
    SDL_memcpy(result->data + pad_length, str->data, str->length + 1); /* Include null terminator */
    result->length = target_length;

    return result;
}

/**
 * \brief           Repeat a string n times
 * \param[in]       str: String to repeat
 * \param[in]       count: Number of times to repeat the string
 * \return          New repeated string or NULL on failure
 */
mvn_string_t*
mvn_string_repeat(const mvn_string_t* str, size_t count) {
    // Explicitly handle NULL input string first
    if (!str) {
        return NULL;
    }

    // Handle count == 0
    if (count == 0) {
        return mvn_string_init(0);
    }

    if (count == 1) {
        return mvn_string_from_cstr(str->data);
    }

    size_t result_len = str->length * count;
    mvn_string_t* result = mvn_string_init(result_len + 1);
    if (!result) {
        return NULL;
    }

    char* dest = result->data;
    for (size_t i = 0; i < count; i++) {
        SDL_memcpy(dest, str->data, str->length);
        dest += str->length;
    }

    *dest = '\0';
    result->length = result_len;

    return result;
}

/**
 * \brief           Replace first occurrence of a substring with another
 * \param[in]       str: String to process
 * \param[in]       search: Substring to search for
 * \param[in]       replacement: Replacement string
 * \return          New string with replacement or NULL on failure
 */
mvn_string_t*
mvn_string_replace(const mvn_string_t* str, const char* search, const char* replacement) {
    if (!str || !search || !replacement) {
        return NULL;
    }

    const char* pos = SDL_strstr(str->data, search);
    if (!pos) {
        return mvn_string_clone(str); /* No match found */
    }

    size_t search_len = SDL_strlen(search);
    size_t replace_len = SDL_strlen(replacement);
    size_t prefix_len = pos - str->data;
    size_t suffix_len = str->length - prefix_len - search_len;
    size_t result_len = prefix_len + replace_len + suffix_len;

    mvn_string_t* result = mvn_string_init(result_len + 1);
    if (!result) {
        return NULL;
    }

    /* Copy prefix */
    SDL_memcpy(result->data, str->data, prefix_len);

    /* Copy replacement */
    SDL_memcpy(result->data + prefix_len, replacement, replace_len);

    /* Copy suffix */
    SDL_memcpy(result->data + prefix_len + replace_len, pos + search_len,
               suffix_len + 1); /* Include null terminator */

    result->length = result_len;

    return result;
}

/**
 * \brief           Replace all occurrences of a substring with another
 * \param[in]       str: String to process
 * \param[in]       search: Substring to search for
 * \param[in]       replacement: Replacement string
 * \return          New string with all replacements or NULL on failure
 */
mvn_string_t*
mvn_string_replace_all(const mvn_string_t* str, const char* search, const char* replacement) {
    if (!str || !search || !replacement) {
        return NULL;
    }

    size_t search_len = SDL_strlen(search);
    if (search_len == 0) {
        return mvn_string_clone(str); /* Empty search string */
    }

    size_t replace_len = SDL_strlen(replacement);

    /* First, count occurrences to calculate final size */
    size_t count = 0;
    const char* pos = str->data;

    while ((pos = SDL_strstr(pos, search)) != NULL) {
        count++;
        pos += search_len;
    }

    if (count == 0) {
        return mvn_string_from_cstr(str->data); /* No matches */
    }

    size_t result_len = str->length + ((replace_len - search_len) * count);
    mvn_string_t* result = mvn_string_init(result_len + 1);
    if (!result) {
        return NULL;
    }

    /* Perform replacements */
    char* dest = result->data;
    const char* src = str->data;
    const char* next_pos;

    while ((next_pos = SDL_strstr(src, search)) != NULL) {
        size_t chunk_len = next_pos - src;

        /* Copy chunk before match */
        SDL_memcpy(dest, src, chunk_len);
        dest += chunk_len;

        /* Copy replacement */
        SDL_memcpy(dest, replacement, replace_len);
        dest += replace_len;

        /* Move source past this occurrence */
        src = next_pos + search_len;
    }

    /* Copy remaining part */
    SDL_strlcpy(dest, src, result->capacity - (dest - result->data));
    result->length = result_len;

    return result;
}

/**
 * \brief           Split string into a list of strings by a delimiter
 * \param[in]       str: String to split
 * \param[in]       delimiter: Delimiter to split by
 * \return          List of strings or NULL on failure
 */
struct mvn_list_t*
mvn_string_split(const mvn_string_t* str, const char* delimiter) {
    if (!str || !delimiter) {
        return NULL;
    }

    struct mvn_list_t* list = mvn_list_init(sizeof(mvn_string_t*), 0);
    if (!list) {
        mvn_log_error("Failed to create list for string split");
        return NULL;
    }

    size_t delim_len = SDL_strlen(delimiter);
    if (delim_len == 0 || str->length == 0) {
        /* Empty delimiter or empty string - return list with just the original string */
        mvn_string_t* copy = mvn_string_from_cstr(str->data);
        if (!copy || !mvn_list_push(list, (const void*)&copy)) {
            mvn_log_error("Failed to add string to split list");
            if (copy) {
                mvn_string_free(copy);
            }
            mvn_list_free(list);
            return NULL;
        }
        return list;
    }

    const char* start = str->data;
    const char* end = NULL;

    while ((end = SDL_strstr(start, delimiter)) != NULL) {
        size_t part_len = end - start;

        /* Create string for this part */
        mvn_string_t* part = mvn_string_init(part_len + 1);
        if (!part) {
            mvn_free_string_list(list);
            return NULL;
        }
        SDL_memcpy(part->data, start, part_len);
        part->data[part_len] = '\0';
        part->length = part_len;

        if (!mvn_list_push(list, (const void*)&part)) {
            mvn_string_free(part);
            mvn_free_string_list(list);
            return NULL;
        }

        start = end + delim_len;
    }

    /* Add final part (always add, even if empty) */
    mvn_string_t* part = mvn_string_from_cstr(start);
    if (!part || !mvn_list_push(list, (const void*)&part)) {
        if (part) {
            mvn_string_free(part);
        }
        mvn_free_string_list(list);
        return NULL;
    }

    return list;
}

/**
 * \brief           Convert string to lowercase
 * \param[in]       str: String to convert
 * \return          New string in lowercase or NULL on failure
 */
mvn_string_t*
mvn_string_to_lowercase(const mvn_string_t* str) {
    if (!str) {
        return NULL;
    }

    mvn_string_t* result = mvn_string_init(str->length + 1);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < str->length; i++) {
        result->data[i] = (char)SDL_tolower((unsigned char)str->data[i]);
    }

    result->data[str->length] = '\0';
    result->length = str->length;

    return result;
}

/**
 * \brief           Convert string to uppercase
 * \param[in]       str: String to convert
 * \return          New string in uppercase or NULL on failure
 */
mvn_string_t*
mvn_string_to_uppercase(const mvn_string_t* str) {
    if (!str) {
        return NULL;
    }

    mvn_string_t* result = mvn_string_init(str->length + 1);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < str->length; i++) {
        result->data[i] = (char)SDL_toupper((unsigned char)str->data[i]);
    }

    result->data[str->length] = '\0';
    result->length = str->length;

    return result;
}

/**
 * \brief           Check if a character is whitespace
 * \param[in]       character: Character to check
 * \return          true if character is whitespace, false otherwise
 */
static bool
is_whitespace(char character) {
    return character == ' ' || character == '\t' || character == '\n' || character == '\r'
           || character == '\f' || character == '\v';
}

/**
 * \brief           Trim whitespace from both ends of the string
 * \param[in]       str: String to trim
 * \return          New trimmed string or NULL on failure
 */
mvn_string_t*
mvn_string_trim(const mvn_string_t* str) {
    if (!str) {
        return NULL;
    }

    if (str->length == 0) {
        return mvn_string_init(0);
    }

    /* Find start position (first non-whitespace) */
    size_t start = 0;
    while (start < str->length && is_whitespace(str->data[start])) {
        start++;
    }

    /* Find end position (last non-whitespace) */
    size_t end = str->length - 1;
    while (end > start && is_whitespace(str->data[end])) {
        end--;
    }

    /* If the whole string is whitespace */
    if (start > end) {
        return mvn_string_init(0);
    }

    /* Create result with trimmed substring */
    size_t result_len = end - start + 1;
    mvn_string_t* result = mvn_string_init(result_len + 1);
    if (!result) {
        return NULL;
    }

    SDL_memcpy(result->data, str->data + start, result_len);
    result->data[result_len] = '\0';
    result->length = result_len;

    return result;
}

/**
 * \brief           Trim whitespace from the end of the string
 * \param[in]       str: String to trim
 * \return          New trimmed string or NULL on failure
 */
mvn_string_t*
mvn_string_trim_end(const mvn_string_t* str) {
    if (!str) {
        return NULL;
    }

    if (str->length == 0) {
        return mvn_string_init(0);
    }

    /* Find end position (last non-whitespace) */
    size_t end = str->length - 1;
    while (end > 0 && is_whitespace(str->data[end])) {
        end--;
    }

    if (end == 0 && is_whitespace(str->data[0])) {
        return mvn_string_init(0);
    }

    /* Create result with trimmed substring */
    size_t result_len = end + 1;
    mvn_string_t* result = mvn_string_init(result_len + 1);
    if (!result) {
        return NULL;
    }

    SDL_memcpy(result->data, str->data, result_len);
    result->data[result_len] = '\0';
    result->length = result_len;

    return result;
}

/**
 * \brief           Trim whitespace from the start of the string
 * \param[in]       str: String to trim
 * \return          New trimmed string or NULL on failure
 */
mvn_string_t*
mvn_string_trim_start(const mvn_string_t* str) {
    if (!str) {
        return NULL;
    }

    if (str->length == 0) {
        return mvn_string_init(0);
    }

    /* Find start position (first non-whitespace) */
    size_t start = 0;
    while (start < str->length && is_whitespace(str->data[start])) {
        start++;
    }

    if (start == str->length) {
        return mvn_string_init(0);
    }

    /* Create result with trimmed substring */
    size_t result_len = str->length - start;
    mvn_string_t* result = mvn_string_init(result_len + 1);
    if (!result) {
        return NULL;
    }

    SDL_memcpy(result->data, str->data + start, result_len);
    result->data[result_len] = '\0';
    result->length = result_len;

    return result;
}

/**
 * \brief           Extract a substring
 * \param[in]       str: String to get substring from
 * \param[in]       start: Start index of substring
 * \param[in]       length: Length of substring
 * \return          New substring or NULL on failure
 */
mvn_string_t*
mvn_string_substring(const mvn_string_t* str, size_t start, size_t length) {
    if (!str) {
        return NULL;
    }

    /* Validate indices */
    if (start > str->length) {
        mvn_log_error("Invalid substring start index: start=%zu, string_length=%zu", start,
                      str->length);
        // Return empty string for invalid start past the end
        return mvn_string_init(0);
    }

    // Clamp length if it goes past the end of the string
    if (start + length > str->length) {
        length = str->length - start;
    }

    // Handle zero length request explicitly
    if (length == 0) {
        return mvn_string_init(0);
    }

    mvn_string_t* result = mvn_string_init(length + 1);
    if (!result) {
        return NULL;
    }

    SDL_memcpy(result->data, str->data + start, length);
    result->data[length] = '\0';
    result->length = length;

    return result;
}

/**
 * \brief           Compare two mvn_string_t strings for equality
 * \param[in]       str1: First string to compare
 * \param[in]       str2: Second string to compare
 * \return          true if strings are equal, false otherwise
 */
bool
mvn_string_compare(const mvn_string_t* str1, const mvn_string_t* str2) {
    if (str1 == NULL || str2 == NULL) {
        return false;
    }
    if (str1->length != str2->length) {
        return false;
    }
    return SDL_memcmp(str1->data, str2->data, str1->length) == 0;
}

/**
 * \brief           Get the current capacity of the string
 * \param[in]       str: String to get capacity of
 * \return          Capacity of the string, or 0 on NULL input
 */
size_t
mvn_string_capacity(const mvn_string_t* str) {
    if (!str) {
        return 0;
    }
    return str->capacity;
}

/**
 * \brief           Clear the string (set length to 0)
 * \param[in]       str: String to clear
 */
void
mvn_string_clear(mvn_string_t* str) {
    if (!str || !str->data) {
        return;
    }
    
    str->length = 0;
    str->data[0] = '\0';
}
