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

typedef enum {
    TOMATO_READY = 0,
    TOMATO_RUN,
    TOMATO_PAUSE
} tomato_mode_t;

typedef enum {
    TOMATO_WARMUP = 0,
    TOMATO_FOCUS,
    TOMATO_REST
} tomato_kind_t;

typedef struct {
    uint32_t target_ts;          // Unix timestamp for scheduled completion
    uint32_t remaining_seconds;   // Preserved remaining seconds while paused/ready
    tomato_mode_t mode;          // READY, RUN, or PAUSE
    tomato_kind_t kind;          // WARMUP, FOCUS, or REST
    uint8_t done_count;          // Number of completed focus blocks (0 to 6)
    uint8_t watch_face_index;    // Assigned index in watch_faces[]
    bool is_active;              // True only when currently visible in foreground
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
