/**
 * \file            empty.c
 * \brief           Basic example showing MVN greeting functionality
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

#include "mvn/mvn.h" // IWYU pragma: keep

/**
 * \brief           Main application entry point
 */
int
main(void) {
    /* Initialize SDL */
    if (!mvn_init(800, 600, "MVN Hello World Example", 0)) {
        return 1;
    }

    mvn_logger_init();

    /* Display Engine Version */
    mvn_string_t* version = mvn_get_engine_version();
    mvn_log_info("MVN Engine Version: %s", mvn_string_to_cstr(version));

    while (!mvn_window_should_close()) {
        // Begin drawing
        mvn_begin_drawing();

        // Clear background with white color
        mvn_clear_background(MVN_BLACK);

        // End drawing and present
        mvn_end_drawing();
    }

    // Clean up all resources
    mvn_string_free(version);
    mvn_quit();
    return 0;
}
