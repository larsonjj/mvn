/**
 * \file            mvn-logger.c
 * \brief           MVN logging functionality implementation
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

#include "mvn/mvn-logger.h"
#include <SDL3/SDL.h>

/**
  * \brief           Initialize the MVN logger
  * \return          true on success, false on failure
  */
bool
mvn_logger_init(void) {
    // Set default log levels for each category
    mvn_logger_set_level(MVN_LOG_CATEGORY_DEFAULT, MVN_LOG_INFO);
    mvn_logger_set_level(MVN_LOG_CATEGORY_ERROR, MVN_LOG_ERROR);
    mvn_logger_set_level(MVN_LOG_CATEGORY_SYSTEM, MVN_LOG_INFO);
    mvn_logger_set_level(MVN_LOG_CATEGORY_AUDIO, MVN_LOG_INFO);
    mvn_logger_set_level(MVN_LOG_CATEGORY_VIDEO, MVN_LOG_INFO);
    mvn_logger_set_level(MVN_LOG_CATEGORY_RENDER, MVN_LOG_INFO);
    mvn_logger_set_level(MVN_LOG_CATEGORY_INPUT, MVN_LOG_INFO);

    // Set a custom prefix format if needed
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_VERBOSE, "[VERBOSE] ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG, "[DEBUG] ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO, "[INFO] ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN, "[WARN] ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR, "[ERROR] ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_CRITICAL, "[CRITICAL] ");

    return true;
}

/**
  * \brief           Set minimum log level for a specific category
  * \param[in]       category: Log category to set level for
  * \param[in]       level: Minimum priority level to log
  */
void
mvn_logger_set_level(mvn_log_category_t category, mvn_log_level_t level) {
    SDL_SetLogPriority((SDL_LogCategory)category, (SDL_LogPriority)level);
}

/**
  * \brief           Set minimum log level for all categories
  * \param[in]       level: Minimum priority level to log
  */
void
mvn_logger_set_all_levels(mvn_log_level_t level) {
    SDL_SetLogPriorities((SDL_LogPriority)level);
}

/**
  * \brief           Log a message with specified category and priority
  * \param[in]       category: Category for the log message
  * \param[in]       priority: Priority level for the message
  * \param[in]       fmt: Formatting string for the log message
  * \param[in]       ...: Variable arguments for the format string
  */
void
mvn_log(mvn_log_category_t category, mvn_log_level_t priority, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV((SDL_LogCategory)category, (SDL_LogPriority)priority, fmt, args);
    va_end(args);
}

/**
  * \brief           Log an information message (INFO level)
  * \param[in]       fmt: Formatting string for the log message
  * \param[in]       ...: Variable arguments for the format string
  */
void
mvn_log_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
    va_end(args);
}

/**
  * \brief           Log a debug message (DEBUG level)
  * \param[in]       fmt: Formatting string for the log message
  * \param[in]       ...: Variable arguments for the format string
  */
void
mvn_log_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, fmt, args);
    va_end(args);
}

/**
  * \brief           Log a warning message (WARN level)
  * \param[in]       fmt: Formatting string for the log message
  * \param[in]       ...: Variable arguments for the format string
  */
void
mvn_log_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, fmt, args);
    va_end(args);
}

/**
  * \brief           Log an error message (ERROR level)
  * \param[in]       fmt: Formatting string for the log message
  * \param[in]       ...: Variable arguments for the format string
  */
void
mvn_log_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, fmt, args);
    va_end(args);
}

/**
  * \brief           Log a critical message (CRITICAL level)
  * \param[in]       fmt: Formatting string for the log message
  * \param[in]       ...: Variable arguments for the format string
  */
void
mvn_log_critical(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, fmt, args);
    va_end(args);
}
