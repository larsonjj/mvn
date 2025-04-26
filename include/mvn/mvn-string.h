/**
 * \file            mvn-string.h
 * \brief           Dynamic string implementation for MVN game framework
 */

/*
 * Copyright (c) 2025 Jake Larson
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of MVN library.
 *
 * Author:          Jake Larson
 */

#ifndef MVN_STRING_H
#define MVN_STRING_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Dynamic string structure
 */
typedef struct mvn_string_t {
    char* data;      /*!< String data */
    size_t length;   /*!< String length (excluding null terminator) */
    size_t capacity; /*!< Allocated capacity (including null terminator) */
} mvn_string_t;

mvn_string_t* mvn_string_init(size_t initial_capacity);
mvn_string_t* mvn_string_from_cstr(const char* cstr);
void mvn_string_free(mvn_string_t* str);
size_t mvn_string_length(const mvn_string_t* str);
const char* mvn_string_to_cstr(const mvn_string_t* str);
mvn_string_t* mvn_string_concat(const mvn_string_t* str1, const mvn_string_t* str2);
bool mvn_string_append(mvn_string_t* str, const char* cstr);
bool mvn_string_ends_with(const mvn_string_t* str, const char* suffix);
bool mvn_string_starts_with(const mvn_string_t* str, const char* prefix);
bool mvn_string_includes(const mvn_string_t* str, const char* substr);
mvn_string_t* mvn_string_pad_end(const mvn_string_t* str, size_t target_length, char pad_char);
mvn_string_t* mvn_string_pad_start(const mvn_string_t* str, size_t target_length, char pad_char);
mvn_string_t* mvn_string_repeat(const mvn_string_t* str, size_t count);
mvn_string_t* mvn_string_replace(const mvn_string_t* str, const char* search,
                                 const char* replacement);
mvn_string_t* mvn_string_replace_all(const mvn_string_t* str, const char* search,
                                     const char* replacement);
struct mvn_list_t* mvn_string_split(const mvn_string_t* str, const char* delimiter);
mvn_string_t* mvn_string_to_lowercase(const mvn_string_t* str);
mvn_string_t* mvn_string_to_uppercase(const mvn_string_t* str);
mvn_string_t* mvn_string_trim(const mvn_string_t* str);
mvn_string_t* mvn_string_trim_end(const mvn_string_t* str);
mvn_string_t* mvn_string_trim_start(const mvn_string_t* str);
mvn_string_t* mvn_string_substring(const mvn_string_t* str, size_t start, size_t length);
bool mvn_string_compare(const mvn_string_t* str1, const mvn_string_t* str2);

#ifdef __cplusplus
}
#endif

#endif /* MVN_STRING_H */
