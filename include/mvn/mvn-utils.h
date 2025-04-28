/**
 * \file            mvn-utils.h
 * \brief           MVN framework utility macros and helpers
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

#ifndef MVN_UTILS_H
#define MVN_UTILS_H

#include <SDL3/SDL.h>
#include "mvn/mvn-list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Helper macro for cross-language struct initialization
 * \hideinitializer
 *
 * This macro helps ensure that struct initialization works correctly
 * in both C and C++ code. C uses compound literals while C++
 * uses brace initialization.
 */
#ifdef __cplusplus
#define MVN_STRUCT(type, ...) type __VA_ARGS__
#else
#define MVN_STRUCT(type, ...) ((type)__VA_ARGS__)
#endif

// Allow custom memory allocators
// Defaults use SDL3 memory management functions
#ifndef MVN_MALLOC
#define MVN_MALLOC(sz) SDL_malloc(sz)
#endif
#ifndef MVN_CALLOC
#define MVN_CALLOC(n, sz) SDL_calloc(n, sz)
#endif
#ifndef MVN_REALLOC
#define MVN_REALLOC(ptr, sz) SDL_realloc(ptr, sz)
#endif
#ifndef MVN_FREE
#define MVN_FREE(ptr) SDL_free(ptr)
#endif

void mvn_set_random_seed(int32_t seed);
int32_t mvn_get_random_value(int32_t min, int32_t max);
void mvn_open_url(const char* url);
mvn_list_t* mvn_load_random_sequence(int32_t count, int32_t min, int32_t max);
void mvn_unload_random_sequence(mvn_list_t* sequence);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_UTILS_H */
