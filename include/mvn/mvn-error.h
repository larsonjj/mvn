/**
 * \file            mvn-error.h
 * \brief           Error handling for MVN game framework
 */

#ifndef MVN_ERROR_H
#define MVN_ERROR_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Set an error message
 * \param[in]       fmt: Format string for the error message
 * \param[in]       ...: Additional arguments for format string
 * \return          Always returns false for convenient usage in return statements
 */
bool mvn_set_error(const char *fmt, ...);

/**
 * \brief           Get the last error message
 * \return          Pointer to the error message string
 */
const char *mvn_get_error(void);

/**
 * \brief           Clear the error message
 */
void mvn_clear_error(void);

/* Convenience macro for error checking and logging */
#define MVN_CHECK_ERROR(condition, ...)                                                            \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            mvn_set_error(__VA_ARGS__);                                                            \
            mvn_log_error(__VA_ARGS__);                                                            \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

/* Convenience macro for null checks */
#define MVN_CHECK_NULL(ptr, ...)                                                                   \
    do {                                                                                           \
        if ((ptr) == NULL) {                                                                       \
            mvn_set_error(__VA_ARGS__);                                                            \
            mvn_log_error(__VA_ARGS__);                                                            \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* MVN_ERROR_H */
