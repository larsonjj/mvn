/**
 * \file            mvn-error.c
 * \brief           Error handling for MVN game framework
 */

#include "mvn/mvn-error.h"

#include "mvn/mvn-logger.h"

#include <SDL3/SDL.h>
#include <stdarg.h>

/* Thread local storage for error messages */
static SDL_TLSID error_tls_id;
static char      error_buffer[1024]; // Fixed size buffer for error messages

bool mvn_set_error(const char *fmt, ...)
{
    va_list ap;
    char   *error_copy;

    va_start(ap, fmt);
    SDL_vsnprintf(error_buffer, sizeof(error_buffer), fmt, ap);
    va_end(ap);

    // Make a copy of the error message for TLS storage
    error_copy = SDL_strdup(error_buffer);
    if (!error_copy) {
        return false;
    }

    // SDL3 initializes the TLS ID automatically if needed
    if (!SDL_SetTLS(&error_tls_id, error_copy, SDL_free)) {
        SDL_free(error_copy);
        return false;
    }

    // Also log the error at DEBUG level
    mvn_log_debug("Error set: %s", error_buffer);

    return false; // Return false for convenient usage in return statements
}

const char *mvn_get_error(void)
{
    const char *error = (const char *)SDL_GetTLS(&error_tls_id);
    return error ? error : "";
}

void mvn_clear_error(void)
{
    SDL_SetTLS(&error_tls_id, NULL, NULL);
}
