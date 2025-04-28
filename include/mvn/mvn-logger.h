/**
 * \file            mvn-logger.h
 * \brief           MVN logging functionality
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

#ifndef MVN_LOGGER_H
#define MVN_LOGGER_H

#include <SDL3/SDL.h>
#include "mvn/mvn-types.h" /* IWYU pragma: keep */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           MVN log priority levels
 */
typedef enum {
    MVN_LOG_VERBOSE = SDL_LOG_PRIORITY_VERBOSE,  /*!< Verbose logging for debugging */
    MVN_LOG_DEBUG = SDL_LOG_PRIORITY_DEBUG,      /*!< Debug log level */
    MVN_LOG_INFO = SDL_LOG_PRIORITY_INFO,        /*!< Information log level */
    MVN_LOG_WARN = SDL_LOG_PRIORITY_WARN,        /*!< Warning log level */
    MVN_LOG_ERROR = SDL_LOG_PRIORITY_ERROR,      /*!< Error log level */
    MVN_LOG_CRITICAL = SDL_LOG_PRIORITY_CRITICAL /*!< Critical log level */
} mvn_log_level_t;

/**
* \brief           MVN log categories
*/
typedef enum {
    MVN_LOG_CATEGORY_DEFAULT = SDL_LOG_CATEGORY_APPLICATION, /*!< Default application category */
    MVN_LOG_CATEGORY_ERROR = SDL_LOG_CATEGORY_ERROR,         /*!< Error category */
    MVN_LOG_CATEGORY_SYSTEM = SDL_LOG_CATEGORY_SYSTEM,       /*!< System category */
    MVN_LOG_CATEGORY_AUDIO = SDL_LOG_CATEGORY_AUDIO,         /*!< Audio category */
    MVN_LOG_CATEGORY_VIDEO = SDL_LOG_CATEGORY_VIDEO,         /*!< Video category */
    MVN_LOG_CATEGORY_RENDER = SDL_LOG_CATEGORY_RENDER,       /*!< Render category */
    MVN_LOG_CATEGORY_INPUT = SDL_LOG_CATEGORY_INPUT,         /*!< Input category */
    MVN_LOG_CATEGORY_CUSTOM = SDL_LOG_CATEGORY_CUSTOM        /*!< Custom category */
} mvn_log_category_t;

bool mvn_logger_init(void);
void mvn_logger_set_level(mvn_log_category_t category, mvn_log_level_t level);
void mvn_logger_set_all_levels(mvn_log_level_t level);
void mvn_log(mvn_log_category_t category, mvn_log_level_t priority, const char* fmt, ...);
void mvn_log_info(const char* fmt, ...);
void mvn_log_debug(const char* fmt, ...);
void mvn_log_warn(const char* fmt, ...);
void mvn_log_error(const char* fmt, ...);
void mvn_log_critical(const char* fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_LOGGER_H */
