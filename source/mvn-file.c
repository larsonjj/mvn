/**
 * \file            mvn-file.c
 * \brief           MVN filesystem utility functions implementation
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

#include "mvn/mvn-file.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mvn/mvn-logger.h"
#include "mvn/mvn-string.h"

/**
 * \brief           Check if a file exists
 * \param[in]       fileName: Path to the file
 * \return          true if file exists, false otherwise
 */
bool
mvn_file_exists(const char* fileName) {
    if (fileName == NULL || fileName[0] == '\0') {
        return false;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(fileName, &info)) {
        return info.type == SDL_PATHTYPE_FILE;
    }

    return false;
}

/**
 * \brief           Check if a directory exists
 * \param[in]       dirPath: Path to the directory
 * \return          true if directory exists, false otherwise
 */
bool
mvn_directory_exists(const char* dirPath) {
    if (dirPath == NULL || dirPath[0] == '\0') {
        return false;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(dirPath, &info)) {
        return info.type == SDL_PATHTYPE_DIRECTORY;
    }

    return false;
}

/**
 * \brief           Check if a file has a specific extension
 * \param[in]       fileName: Path to the file
 * \param[in]       ext: Extension to check for (including dot, e.g. ".png")
 * \return          true if file has the extension, false otherwise
 */
bool
mvn_is_file_extension(const char* fileName, const char* ext) {
    if (fileName == NULL || ext == NULL) {
        return false;
    }

    mvn_string_t* fileExt = mvn_get_file_extension(fileName);
    if (!fileExt) {
        return false;
    }

    bool result = false;
    if (fileExt->length == 0) {
        result = (ext[0] == '\0'); // No extension matches empty extension
    } else {
        // Case-insensitive comparison
        result = (SDL_strcasecmp(mvn_string_to_cstr(fileExt), ext) == 0);
    }

    mvn_string_free(fileExt);
    return result;
}

/**
 * \brief           Get file length in bytes
 * \param[in]       fileName: Path to the file
 * \return          File size in bytes, -1 on failure
 */
int32_t
mvn_get_file_length(const char* fileName) {
    if (fileName == NULL) {
        return -1;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(fileName, &info) && info.type == SDL_PATHTYPE_FILE) {
        if (info.size > INT32_MAX) {
            mvn_log_warn("File size exceeds int32_t limit: %s", fileName);
            return INT32_MAX;
        }
        return (int32_t)info.size;
    }

    return -1;
}

/**
 * \brief           Get file extension
 * \param[in]       fileName: Path to the file
 * \return          File extension as mvn_string_t including dot (e.g. ".png"), empty string if no extension
 */
mvn_string_t*
mvn_get_file_extension(const char* fileName) {
    if (fileName == NULL) {
        return mvn_string_from_cstr("");
    }

    const char* dot = SDL_strrchr(fileName, '.');
    if (dot == NULL || dot == fileName) {
        return mvn_string_from_cstr(""); // No extension or filename starts with dot
    }

    // Make sure there's no directory separator after the last dot
    const char* slash = SDL_max(SDL_strrchr(fileName, '/'), SDL_strrchr(fileName, '\\'));
    if (slash != NULL && slash > dot) {
        return mvn_string_from_cstr(""); // Last dot is in a directory component
    }

    return mvn_string_from_cstr(dot);
}

/**
 * \brief           Get filename from a path
 * \param[in]       filePath: Path to the file
 * \return          Filename part of the path as mvn_string_t
 */
mvn_string_t*
mvn_get_file_name(const char* filePath) {
    if (filePath == NULL) {
        return mvn_string_from_cstr("");
    }

    const char* slash1 = SDL_strrchr(filePath, '/');
    const char* slash2 = SDL_strrchr(filePath, '\\');
    const char* lastSlash = SDL_max(slash1, slash2);

    if (lastSlash != NULL) {
        return mvn_string_from_cstr(lastSlash + 1); // Skip the slash character
    }

    return mvn_string_from_cstr(filePath); // No directory component
}

/**
 * \brief           Get filename without extension
 * \param[in]       filePath: Path to the file
 * \return          Filename without extension as a mvn_string_t
 */
mvn_string_t*
mvn_get_file_name_without_ext(const char* filePath) {
    if (filePath == NULL) {
        return mvn_string_from_cstr("");
    }

    mvn_string_t* fileName = mvn_get_file_name(filePath);
    if (!fileName) {
        return mvn_string_from_cstr("");
    }

    mvn_string_t* extStr = mvn_get_file_extension(mvn_string_to_cstr(fileName));
    if (!extStr) {
        mvn_string_free(fileName);
        return mvn_string_from_cstr("");
    }

    const char* name = mvn_string_to_cstr(fileName);
    const char* ext = mvn_string_to_cstr(extStr);

    // Calculate the length of the filename without extension
    size_t length = (ext[0] != '\0') ? (size_t)(ext - name) : fileName->length;

    // Create a new string with the calculated length
    mvn_string_t* result = mvn_string_init(length + 1);
    if (result == NULL) {
        mvn_log_error("Failed to create string for filename without extension");
        mvn_string_free(fileName);
        mvn_string_free(extStr);
        return mvn_string_from_cstr("");
    }

    // Copy the filename without the extension
    SDL_memcpy(result->data, name, length);
    result->data[length] = '\0';
    result->length = length;

    mvn_string_free(fileName);
    mvn_string_free(extStr);
    return result;
}

/**
 * \brief           Get directory path from a file path
 * \param[in]       filePath: Path to the file
 * \return          Directory path as a mvn_string_t
 */
mvn_string_t*
mvn_get_directory_path(const char* filePath) {
    if (filePath == NULL) {
        return mvn_string_from_cstr("");
    }

    // Find the last slash
    const char* lastSlash1 = SDL_strrchr(filePath, '/');
    const char* lastSlash2 = SDL_strrchr(filePath, '\\');
    const char* lastSlash = SDL_max(lastSlash1, lastSlash2);

    mvn_string_t* result;

    if (lastSlash == NULL) {
        // No directory component, return "."
        result = mvn_string_from_cstr(".");
    } else {
        // Calculate the length of the directory path
        size_t length = (size_t)(lastSlash - filePath);

        // Create a new string with the calculated length
        result = mvn_string_init(length + 1);
        if (result == NULL) {
            mvn_log_error("Failed to create string for directory path");
            return mvn_string_from_cstr("");
        }

        // Copy the directory path
        SDL_memcpy(result->data, filePath, length);
        result->data[length] = '\0';
        result->length = length;
    }

    return result;
}

/**
 * \brief           Get previous directory path
 * \param[in]       dirPath: Current directory path
 * \return          Parent directory path as a mvn_string_t
 */
mvn_string_t*
mvn_get_parent_directory_path(const char* dirPath) {
    if (dirPath == NULL || dirPath[0] == '\0') {
        return mvn_string_from_cstr("");
    }

    // Make a copy of the path that we can modify
    mvn_string_t* path = mvn_string_from_cstr(dirPath);
    if (path == NULL) {
        mvn_log_error("Failed to create string for parent directory path");
        return mvn_string_from_cstr("");
    }

    // Remove trailing slashes
    while (path->length > 0
           && (path->data[path->length - 1] == '/' || path->data[path->length - 1] == '\\')) {
        path->data[path->length - 1] = '\0';
        path->length--;
    }

    if (path->length == 0) {
        mvn_string_free(path);
        return mvn_string_from_cstr("/"); // Root directory
    }

    // Find the last slash
    char* lastSlash1 = SDL_strrchr(path->data, '/');
    char* lastSlash2 = SDL_strrchr(path->data, '\\');
    char* lastSlash = SDL_max(lastSlash1, lastSlash2);

    if (lastSlash == NULL) {
        mvn_string_free(path);
        return mvn_string_from_cstr("."); // No parent directory
    }

    // Terminate the string at the last slash
    *lastSlash = '\0';
    path->length = (size_t)(lastSlash - path->data);

    if (path->length == 0) {
        mvn_string_free(path);
        return mvn_string_from_cstr("/"); // Root directory
    }

    return path;
}

/**
 * \brief           Get the directory of the running application
 * \return          Application directory as a mvn_string_t
 */
mvn_string_t*
mvn_get_application_directory(void) {
    const char* basePath = SDL_GetBasePath();
    if (basePath == NULL) {
        mvn_log_error("Failed to get application directory: %s", SDL_GetError());
        return mvn_string_from_cstr("");
    }

    mvn_string_t* result = mvn_string_from_cstr(basePath);
    MVN_FREE((void*)basePath);

    if (result == NULL) {
        mvn_log_error("Failed to create string for application directory");
        return mvn_string_from_cstr("");
    }

    return result;
}

/**
 * \brief           Check if a given path is a file
 * \param[in]       path: Path to check
 * \return          true if path is a file, false otherwise
 */
bool
mvn_is_path_file(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(path, &info)) {
        return info.type == SDL_PATHTYPE_FILE;
    }

    return false;
}

/**
 * \brief           Check if a given path is a directory
 * \param[in]       path: Path to check
 * \return          true if path is a directory, false otherwise
 */
bool
mvn_is_path_directory(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(path, &info)) {
        return info.type == SDL_PATHTYPE_DIRECTORY;
    }

    return false;
}

/**
 * \brief           Get file modification time (last write time)
 * \param[in]       fileName: Path to the file
 * \return          Modification time as a Unix timestamp, -1 on failure
 */
long
mvn_get_file_mod_time(const char* fileName) {
    if (fileName == NULL || fileName[0] == '\0') {
        return -1;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(fileName, &info)) {
        if (info.type == SDL_PATHTYPE_FILE) {
            return (long)info.modify_time;
        }
    }

    return -1;
}
