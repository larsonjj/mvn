/**
 * \file            mvn-utils.c
 * \brief           MVN utilities functions
 */

/*
 * Copyright (c) 2024 Jake Larson
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
 * This file is part of MVN.
 *
 * Author:          Jake Larson
 */

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mvn/mvn-utils.h"

/**
 * \brief           Set the seed for the random number generator
 * \param[in]       seed: Seed value to set
 */
void
mvn_set_random_seed(int32_t seed) {
    SDL_srand((unsigned int)seed);
}

/**
 * \brief           Get a random value between min and max (both included)
 * \param[in]       min: Minimum value (inclusive)
 * \param[in]       max: Maximum value (inclusive)
 * \return          Random value in the specified range
 */
int32_t
mvn_get_random_value(int32_t min, int32_t max) {
    if (min > max) {
        int32_t tmp = max;
        max = min;
        min = tmp;
    }

    uint32_t random_value = SDL_rand(max);
    return min + (int32_t)(random_value % (max - min + 1));
}

/**
 * \brief           Open URL with default system browser (if available)
 * \param[in]       url: URL to open
 */
void
mvn_open_url(const char* url) {
    if (!SDL_OpenURL(url)) {
        SDL_Log("Failed to open URL %s: %s", url, SDL_GetError());
    }
}

/**
 * \brief           Load random values sequence, no values repeated
 * \param[in]       count: Number of values to generate
 * \param[in]       min: Minimum value (inclusive)
 * \param[in]       max: Maximum value (inclusive)
 * \return          MVN list containing unique random values or NULL on failure
 */
mvn_list_t*
mvn_load_random_sequence(int32_t count, int32_t min, int32_t max) {
    if (count <= 0 || min > max) {
        return NULL;
    }

    /* Make sure the range is large enough to accommodate 'count' unique values */
    if ((max - min + 1) < count) {
        return NULL;
    }

    /* Create a list to store the sequence */
    mvn_list_t* list = MVN_LIST_INIT(int32_t, count);
    if (list == NULL) {
        return NULL;
    }

    /* Generate the required number of unique random values */
    while (mvn_list_length(list) < (size_t)count) {
        int32_t value = mvn_get_random_value(min, max);

        /* Check if value already exists in the list */
        bool exists = false;
        for (size_t i = 0; i < mvn_list_length(list); i++) {
            int32_t* existing = MVN_LIST_GET(int32_t, list, i);
            if (existing && *existing == value) {
                exists = true;
                break;
            }
        }

        /* Add unique value to list */
        if (!exists) {
            MVN_LIST_PUSH(list, int32_t, value);
        }
    }

    return list;
}

/**
 * \brief           Unload random values sequence
 * \param[in]       sequence: Pointer to sequence list to free
 */
void
mvn_unload_random_sequence(mvn_list_t* sequence) {
    if (sequence != NULL) {
        mvn_list_free(sequence);
    }
}
