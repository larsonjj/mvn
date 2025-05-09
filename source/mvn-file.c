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

#include "mvn/mvn-error.h" // Added error module
#include "mvn/mvn-logger.h"
#include "mvn/mvn-string.h"
#include "mvn/mvn-utils.h"

#include <SDL3/SDL.h>

/**
 * \brief           Check if a file exists
 * \param[in]       fileName: Path to the file
 * \return          true if file exists, false otherwise
 */
bool mvn_file_exists(const char *fileName)
{
    if (fileName == NULL) {
        mvn_set_error("Cannot check if file exists: NULL filename");
        return false;
    }

    if (fileName[0] == '\0') {
        return mvn_set_error("Cannot check if file exists: Empty filename");
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(fileName, &info)) {
        return info.type == SDL_PATHTYPE_FILE;
    }

    return mvn_set_error("Failed to get path info: %s", SDL_GetError());
}

/**
 * \brief           Check if a directory exists
 * \param[in]       dirPath: Path to the directory
 * \return          true if directory exists, false otherwise
 */
bool mvn_directory_exists(const char *dirPath)
{
    if (dirPath == NULL) {
        mvn_set_error("Cannot check if directory exists: NULL directory path");
        return false;
    }

    if (dirPath[0] == '\0') {
        return mvn_set_error("Cannot check if directory exists: Empty directory path");
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(dirPath, &info)) {
        return info.type == SDL_PATHTYPE_DIRECTORY;
    }

    return mvn_set_error("Failed to get path info: %s", SDL_GetError());
}

/**
 * \brief           Check if a file has a specific extension
 * \param[in]       fileName: Path to the file
 * \param[in]       ext: Extension to check for (including dot, e.g. ".png")
 * \return          true if file has the extension, false otherwise
 */
bool mvn_is_file_extension(const char *fileName, const char *ext)
{
    if (fileName == NULL) {
        mvn_set_error("Cannot check file extension: NULL filename");
        return false;
    }

    if (ext == NULL) {
        mvn_set_error("Cannot check file extension: NULL extension");
        return false;
    }

    mvn_string_t *fileExt = mvn_get_file_extension(fileName);
    if (!fileExt) {
        return mvn_set_error("Failed to get file extension");
    }

    bool result = false;
    if (fileExt->length == 0) {
        result = false;
    } else {
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
int32_t mvn_get_file_length(const char *fileName)
{
    if (fileName == NULL) {
        mvn_set_error("Cannot get file length: NULL filename");
        return -1;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(fileName, &info) && info.type == SDL_PATHTYPE_FILE) {
        if (info.size > INT32_MAX) {
            mvn_set_error("File size exceeds int32_t limit: %s", fileName);
            return INT32_MAX;
        }
        return (int32_t)info.size;
    }

    mvn_set_error("Failed to get file length for '%s': %s", fileName, SDL_GetError());
    return -1;
}

/**
 * \brief           Get file extension
 * \param[in]       fileName: Path to the file
 * \return          File extension as mvn_string_t including dot (e.g. ".png"), empty string if no
 * extension
 */
mvn_string_t *mvn_get_file_extension(const char *fileName)
{
    if (fileName == NULL) {
        mvn_set_error("Cannot get file extension: NULL filename");
        return mvn_string_from_cstr("");
    }

    const char *dot = SDL_strrchr(fileName, '.');
    if (dot == NULL || dot == fileName) {
        return mvn_string_from_cstr(""); // No extension or filename starts with dot
    }

    // Make sure there's no directory separator after the last dot
    const char *slash = SDL_max(SDL_strrchr(fileName, '/'), SDL_strrchr(fileName, '\\'));
    if (slash != NULL && slash > dot) {
        return mvn_string_from_cstr(""); // Last dot is in a directory component
    }

    mvn_string_t *result = mvn_string_from_cstr(dot);
    if (!result) {
        mvn_set_error("Failed to create string for file extension");
    }
    return result;
}

/**
 * \brief           Get filename from a path
 * \param[in]       filePath: Path to the file
 * \return          Filename part of the path as mvn_string_t
 */
mvn_string_t *mvn_get_file_name(const char *filePath)
{
    if (filePath == NULL) {
        mvn_set_error("Cannot get file name: NULL file path");
        return mvn_string_from_cstr("");
    }

    const char *slash1    = SDL_strrchr(filePath, '/');
    const char *slash2    = SDL_strrchr(filePath, '\\');
    const char *lastSlash = SDL_max(slash1, slash2);

    const char   *name   = (lastSlash != NULL) ? lastSlash + 1 : filePath;
    mvn_string_t *result = mvn_string_from_cstr(name);

    if (!result) {
        mvn_set_error("Failed to create string for file name");
        return mvn_string_from_cstr("");
    }

    return result;
}

/**
 * \brief           Get filename without extension
 * \param[in]       filePath: Path to the file
 * \return          Filename without extension as a mvn_string_t
 */
mvn_string_t *mvn_get_file_name_without_ext(const char *filePath)
{
    if (filePath == NULL) {
        mvn_set_error("Cannot get file name without extension: NULL file path");
        return mvn_string_from_cstr("");
    }

    mvn_string_t *fileName = mvn_get_file_name(filePath);
    if (!fileName) {
        return mvn_string_from_cstr("");
    }

    mvn_string_t *extStr = mvn_get_file_extension(mvn_string_to_cstr(fileName));
    if (!extStr) {
        mvn_string_free(fileName);
        return mvn_string_from_cstr("");
    }

    const char *name = mvn_string_to_cstr(fileName);
    const char *ext  = mvn_string_to_cstr(extStr);

    // Calculate the length of the filename without extension
    size_t length = (ext[0] != '\0') ? (size_t)(ext - name) : fileName->length;

    // Create a new string with the calculated length
    mvn_string_t *result = mvn_string_init(length + 1);
    if (result == NULL) {
        mvn_set_error("Failed to create string for filename without extension");
        mvn_string_free(fileName);
        mvn_string_free(extStr);
        return mvn_string_from_cstr("");
    }

    // Copy the filename without the extension
    SDL_memcpy(result->data, name, length);
    result->data[length] = '\0';
    result->length       = length;

    mvn_string_free(fileName);
    mvn_string_free(extStr);
    return result;
}

/**
 * \brief           Get directory path from a file path
 * \param[in]       filePath: Path to the file
 * \return          Directory path as a mvn_string_t
 */
mvn_string_t *mvn_get_directory_path(const char *filePath)
{
    if (filePath == NULL) {
        mvn_set_error("Cannot get directory path: NULL file path");
        return mvn_string_from_cstr("");
    }

    // Find the last slash
    const char *lastSlash1 = SDL_strrchr(filePath, '/');
    const char *lastSlash2 = SDL_strrchr(filePath, '\\');
    const char *lastSlash  = SDL_max(lastSlash1, lastSlash2);

    mvn_string_t *result;

    if (lastSlash == NULL) {
        // No directory component, return "."
        result = mvn_string_from_cstr(".");
    } else {
        // Calculate the length of the directory path
        size_t length = (size_t)(lastSlash - filePath);

        // Create a new string with the calculated length
        result = mvn_string_init(length + 1);
        if (result == NULL) {
            mvn_set_error("Failed to create string for directory path");
            return mvn_string_from_cstr("");
        }

        // Copy the directory path
        SDL_memcpy(result->data, filePath, length);
        result->data[length] = '\0';
        result->length       = length;
    }

    return result;
}

/**
 * \brief           Get previous directory path
 * \param[in]       dirPath: Current directory path
 * \return          Parent directory path as a mvn_string_t
 */
mvn_string_t *mvn_get_parent_directory_path(const char *dirPath)
{
    if (dirPath == NULL) {
        mvn_set_error("Cannot get parent directory: NULL directory path");
        return mvn_string_from_cstr("");
    }

    if (dirPath[0] == '\0') {
        mvn_set_error("Cannot get parent directory: Empty directory path");
        return mvn_string_from_cstr("");
    }

    // Make a copy of the path that we can modify
    mvn_string_t *path = mvn_string_from_cstr(dirPath);
    if (path == NULL) {
        mvn_set_error("Failed to create string for parent directory path");
        return mvn_string_from_cstr("");
    }

    // Remove trailing slashes
    while (path->length > 0 &&
           (path->data[path->length - 1] == '/' || path->data[path->length - 1] == '\\')) {
        path->data[path->length - 1] = '\0';
        path->length--;
    }

    if (path->length == 0) {
        mvn_string_free(path);
        return mvn_string_from_cstr("/"); // Root directory
    }

    // Find the last slash
    char *lastSlash1 = SDL_strrchr(path->data, '/');
    char *lastSlash2 = SDL_strrchr(path->data, '\\');
    char *lastSlash  = SDL_max(lastSlash1, lastSlash2);

    if (lastSlash == NULL) {
        mvn_string_free(path);
        return mvn_string_from_cstr("."); // No parent directory
    }

    // Terminate the string at the last slash
    *lastSlash   = '\0';
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
mvn_string_t *mvn_get_application_directory(void)
{
    const char *basePath = SDL_GetBasePath();
    if (basePath == NULL) {
        return mvn_set_error("Failed to get application directory: %s", SDL_GetError()),
               mvn_string_from_cstr("");
    }

    mvn_string_t *result = mvn_string_from_cstr(basePath);
    MVN_FREE((void *)basePath);

    if (result == NULL) {
        mvn_set_error("Failed to create string for application directory");
        return mvn_string_from_cstr("");
    }

    return result;
}

/**
 * \brief           Check if a given path is a file
 * \param[in]       path: Path to check
 * \return          true if path is a file, false otherwise
 */
bool mvn_is_path_file(const char *path)
{
    if (path == NULL) {
        mvn_set_error("Cannot check if path is a file: NULL path");
        return false;
    }

    if (path[0] == '\0') {
        return mvn_set_error("Cannot check if path is a file: Empty path");
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(path, &info)) {
        return info.type == SDL_PATHTYPE_FILE;
    }

    return mvn_set_error("Failed to get path info: %s", SDL_GetError());
}

/**
 * \brief           Check if a given path is a directory
 * \param[in]       path: Path to check
 * \return          true if path is a directory, false otherwise
 */
bool mvn_is_path_directory(const char *path)
{
    if (path == NULL) {
        mvn_set_error("Cannot check if path is a directory: NULL path");
        return false;
    }

    if (path[0] == '\0') {
        return mvn_set_error("Cannot check if path is a directory: Empty path");
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(path, &info)) {
        return info.type == SDL_PATHTYPE_DIRECTORY;
    }

    return mvn_set_error("Failed to get path info: %s", SDL_GetError());
}

/**
 * \brief           Get file modification time (last write time)
 * \param[in]       fileName: Path to the file
 * \return          Modification time as a Unix timestamp, -1 on failure
 */
int64_t mvn_get_file_mod_time(const char *fileName)
{
    if (fileName == NULL) {
        mvn_set_error("Cannot get file modification time: NULL filename");
        return -1;
    }

    if (fileName[0] == '\0') {
        mvn_set_error("Cannot get file modification time: Empty filename");
        return -1;
    }

    SDL_PathInfo info;
    if (SDL_GetPathInfo(fileName, &info)) {
        if (info.type == SDL_PATHTYPE_FILE) {
            return info.modify_time;
        }
        mvn_set_error("Path is not a file: %s", fileName);
        return -1;
    }

    mvn_set_error("Failed to get path info: %s", SDL_GetError());
    return -1;
}
