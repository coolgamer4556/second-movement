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

#include <stdlib.h>
#include <string.h>
#include "tomato_face.h"
#include "watch.h"
#include "watch_utility.h"

#define WARMUP_SECONDS  (5  * 60)
#define FOCUS_SECONDS   (25 * 60)
#define REST_SECONDS    (7  * 60)
#define TOTAL_SESSIONS  6

static void tomato_schedule_target(tomato_state_t *state, uint32_t seconds) {
    state->target_ts = movement_get_utc_timestamp() + seconds;
    watch_date_time_t target_dt = watch_utility_date_time_from_unix_time(state->target_ts, 0);
    movement_schedule_background_task_for_face(state->watch_face_index, target_dt);
    if (state->is_active) {
        watch_set_indicator(WATCH_INDICATOR_BELL);
    }
}

static void tomato_full_reset(tomato_state_t *state) {
    state->mode = TOMATO_READY;
    state->kind = TOMATO_WARMUP;
    state->remaining_seconds = WARMUP_SECONDS;
    state->done_count = 0;
    movement_cancel_background_task_for_face(state->watch_face_index);
    if (state->is_active) {
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    }
}

static void tomato_advance_interval(tomato_state_t *state) {
    switch (state->kind) {
        case TOMATO_WARMUP:
            // Step 1 -> Step 2: Warm-up ends, first Focus session begins
            state->kind = TOMATO_FOCUS;
            state->remaining_seconds = FOCUS_SECONDS;
            break;

        case TOMATO_FOCUS:
            state->done_count++;
            if (state->done_count >= TOTAL_SESSIONS) {
                // Target met: Exactly 6 focus sessions (2.5 hours) complete!
                movement_play_alarm();
                tomato_full_reset(state);
                return;
            }
            // Step 3: Transition into interspersed 7m rest block
            state->kind = TOMATO_REST;
            state->remaining_seconds = REST_SECONDS;
            break;

        case TOMATO_REST:
            // Rest ends, auto-chain into next Focus session
            state->kind = TOMATO_FOCUS;
            state->remaining_seconds = FOCUS_SECONDS;
            break;
    }

    // Chime to notify interval change
    movement_play_signal();

    // Auto-chain: if in RUN mode, schedule the next interval immediately
    if (state->mode == TOMATO_RUN) {
        tomato_schedule_target(state, state->remaining_seconds);
    }
}

static void tomato_skip(tomato_state_t *state) {
    if (state->mode == TOMATO_RUN) {
        movement_cancel_background_task_for_face(state->watch_face_index);
        tomato_advance_interval(state);
    } else {
        // Skip while Paused or Ready advances interval without starting countdown
        switch (state->kind) {
            case TOMATO_WARMUP:
                state->kind = TOMATO_FOCUS;
                state->remaining_seconds = FOCUS_SECONDS;
                break;
            case TOMATO_FOCUS:
                state->done_count++;
                if (state->done_count >= TOTAL_SESSIONS) {
                    state->kind = TOMATO_WARMUP;
                    state->done_count = 0;
                    state->remaining_seconds = WARMUP_SECONDS;
                } else {
                    state->kind = TOMATO_REST;
                    state->remaining_seconds = REST_SECONDS;
                }
                break;
            case TOMATO_REST:
                state->kind = TOMATO_FOCUS;
                state->remaining_seconds = FOCUS_SECONDS;
                break;
        }
        state->mode = TOMATO_READY;
        movement_cancel_background_task_for_face(state->watch_face_index);
        if (state->is_active) {
            watch_clear_indicator(WATCH_INDICATOR_BELL);
        }
    }
}

static void tomato_draw(tomato_state_t *state) {
    if (!state->is_active) return;

    char buf[16];
    uint32_t remaining;

    if (state->mode == TOMATO_RUN) {
        uint32_t now = movement_get_utc_timestamp();
        remaining = (state->target_ts > now) ? (state->target_ts - now) : 0;
    } else {
        remaining = state->remaining_seconds;
    }

    div_t result = div(remaining, 60);
    uint8_t min = (uint8_t)result.quot;
    uint8_t sec = (uint8_t)result.rem;

    // Use full 14-segment top display so 'R' displays with a true diagonal uppercase leg!
    switch (state->kind) {
        case TOMATO_WARMUP:
            watch_display_text_with_fallback(WATCH_POSITION_TOP, "TOM WU", "WU");
            break;
        case TOMATO_FOCUS:
            watch_display_text_with_fallback(WATCH_POSITION_TOP, "TOM FC", "FC");
            break;
        case TOMATO_REST:
            watch_display_text_with_fallback(WATCH_POSITION_TOP, "TOM RS", "RS");
            break;
    }

    // Format: MM:SS and Completed Count (e.g., "2500 0")
    sprintf(buf, "%2d%02d%2d", min, sec, state->done_count);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void tomato_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(tomato_state_t));
        memset(*context_ptr, 0, sizeof(tomato_state_t));
        tomato_state_t *state = (tomato_state_t *)*context_ptr;
        state->mode = TOMATO_READY;
        state->kind = TOMATO_WARMUP;
        state->remaining_seconds = WARMUP_SECONDS;
        state->done_count = 0;
        state->watch_face_index = watch_face_index;
        state->is_active = false;
    }
}

void tomato_face_activate(void *context) {
    tomato_state_t *state = (tomato_state_t *)context;
    state->is_active = true;

    if (state->mode == TOMATO_RUN) {
        watch_set_indicator(WATCH_INDICATOR_BELL);
    } else {
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    }

    watch_set_colon();
}

bool tomato_face_loop(movement_event_t event, void *context) {
    tomato_state_t *state = (tomato_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            tomato_draw(state);
            break;

        case EVENT_TICK:
            if (state->mode == TOMATO_RUN) {
                uint32_t now = movement_get_utc_timestamp();
                if (now >= state->target_ts) {
                    tomato_advance_interval(state);
                }
            }
            tomato_draw(state);
            break;

        case EVENT_ALARM_BUTTON_UP:
            // Short Press: Toggle Start -> Pause -> Resume
            switch (state->mode) {
                case TOMATO_READY:
                    state->mode = TOMATO_RUN;
                    tomato_schedule_target(state, state->remaining_seconds);
                    break;
                case TOMATO_RUN:
                    state->mode = TOMATO_PAUSE;
                    {
                        uint32_t now = movement_get_utc_timestamp();
                        state->remaining_seconds = (state->target_ts > now) ? (state->target_ts - now) : 0;
                    }
                    movement_cancel_background_task_for_face(state->watch_face_index);
                    watch_clear_indicator(WATCH_INDICATOR_BELL);
                    break;
                case TOMATO_PAUSE:
                    state->mode = TOMATO_RUN;
                    tomato_schedule_target(state, state->remaining_seconds);
                    break;
            }
            tomato_draw(state);
            break;

        case EVENT_ALARM_LONG_PRESS:
            // Long Press: Full session reset back to Warm-up 05:00
            tomato_full_reset(state);
            tomato_draw(state);
            break;

        case EVENT_LIGHT_BUTTON_UP:
            // Short Press: Skip ahead to next interval
            tomato_skip(state);
            tomato_draw(state);
            break;

        case EVENT_LIGHT_LONG_PRESS:
            // Long Press: Illuminate backlight
            movement_illuminate_led();
            break;

        case EVENT_BACKGROUND_TASK:
            // Fired by RTC interrupt when an interval expires in background/sleep
            if (state->mode == TOMATO_RUN) {
                uint32_t now = movement_get_utc_timestamp();
                if (now >= state->target_ts) {
                    tomato_advance_interval(state);
                }
            }
            if (state->is_active) {
                tomato_draw(state);
            }
            break;

        case EVENT_TIMEOUT:
            // Return to clock face after 60s inactivity if paused or ready
            if (state->mode != TOMATO_RUN) {
                movement_move_to_face(0);
            }
            break;

        default:
            movement_default_loop_handler(event);
            break;
    }

    return true;
}

void tomato_face_resign(void *context) {
    tomato_state_t *state = (tomato_state_t *)context;
    state->is_active = false;
}
