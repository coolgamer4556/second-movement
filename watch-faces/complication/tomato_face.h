/*
 * MIT License
 *
 * Copyright (c) 2026 Wesley Ellis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TOMATO_FACE_H_
#define TOMATO_FACE_H_

#include "movement.h"

// Default durations in minutes (can be overridden in movement_config.h)
#ifndef TOMATO_DEFAULT_FOCUS_MIN
#define TOMATO_DEFAULT_FOCUS_MIN 25
#endif

#ifndef TOMATO_DEFAULT_BREAK_MIN
#define TOMATO_DEFAULT_BREAK_MIN 5
#endif

typedef enum {
    tomato_ready,
    tomato_run,
    tomato_pause,
    tomato_setting, // Setting mode for adjusting durations
} tomato_mode;

typedef enum {
    tomato_break,
    tomato_focus,
} tomato_kind;

typedef struct {
    uint32_t target_ts;
    uint32_t now_ts;
    uint32_t time_left;
    tomato_mode mode;
    tomato_kind kind;
    uint8_t done_count;
    uint8_t watch_face_index;
    uint8_t focus_min; // Configurable focus length
    uint8_t break_min; // Configurable break length
} tomato_state_t;

void tomato_face_setup(uint8_t watch_face_index, void ** context_ptr);
void tomato_face_activate(void *context);
bool tomato_face_loop(movement_event_t event, void *context);
void tomato_face_resign(void *context);

#define tomato_face ((const watch_face_t){ \
    tomato_face_setup, \
    tomato_face_activate, \
    tomato_face_loop, \
    tomato_face_resign, \
    NULL, \
})

#endif // TOMATO_FACE_H_
