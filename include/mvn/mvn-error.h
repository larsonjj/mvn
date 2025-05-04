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

bool        mvn_set_error(const char *fmt, ...);
const char *mvn_get_error(void);
void        mvn_clear_error(void);

#ifdef __cplusplus
}
#endif

#endif /* MVN_ERROR_H */
