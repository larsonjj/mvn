/**
 * \file            mvn-file.h
 * \brief           MVN filesystem utility functions
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

#ifndef MVN_FILE_H
#define MVN_FILE_H

#include "mvn/mvn-string.h"

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool          mvn_file_exists(const char *fileName);
bool          mvn_directory_exists(const char *dirPath);
bool          mvn_is_file_extension(const char *fileName, const char *ext);
int32_t       mvn_get_file_length(const char *fileName);
mvn_string_t *mvn_get_file_extension(const char *fileName);
mvn_string_t *mvn_get_file_name(const char *filePath);
mvn_string_t *mvn_get_file_name_without_ext(const char *filePath);
mvn_string_t *mvn_get_directory_path(const char *filePath);
mvn_string_t *mvn_get_parent_directory_path(const char *dirPath);
mvn_string_t *mvn_get_application_directory(void);
bool          mvn_is_path_file(const char *path);
bool          mvn_is_path_directory(const char *path);
long          mvn_get_file_mod_time(const char *fileName);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MVN_FILE_H */
