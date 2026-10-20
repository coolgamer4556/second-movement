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

static uint8_t get_length(tomato_state_t *state) {
    if (state->kind == tomato_focus) {
        return state->focus_min;
    } else {
        return state->break_min;
    }
}

static void tomato_start(tomato_state_t *state) {
    uint8_t length = get_length(state);

    state->mode = tomato_run;
    state->now_ts = movement_get_utc_timestamp();
    state->target_ts = watch_utility_offset_timestamp(state->now_ts, 0, length, 0);
    watch_date_time_t target_dt = watch_utility_date_time_from_unix_time(state->target_ts, 0);
    movement_schedule_background_task_for_face(state->watch_face_index, target_dt);
    watch_set_indicator(WATCH_INDICATOR_BELL);
}

static void tomato_pause_timer(tomato_state_t *state) {
    state->mode = tomato_pause;
    movement_cancel_background_task_for_face(state->watch_face_index);
    watch_clear_indicator(WATCH_INDICATOR_BELL);

    uint32_t now = movement_get_utc_timestamp();
    if (state->target_ts > now) {
        state->time_left = state->target_ts - now;
    } else {
        state->time_left = 0;
    }
}

static void tomato_resume_timer(tomato_state_t *state) {
    state->mode = tomato_run;
    state->now_ts = movement_get_utc_timestamp();
    state->target_ts = state->now_ts + state->time_left;
    watch_date_time_t target_dt = watch_utility_date_time_from_unix_time(state->target_ts, 0);
    movement_schedule_background_task_for_face(state->watch_face_index, target_dt);
    watch_set_indicator(WATCH_INDICATOR_BELL);
}

static void tomato_draw(tomato_state_t *state, uint8_t subsecond) {
    char buf[16];

    uint32_t delta;
    div_t result;
    uint8_t min = 0;
    uint8_t sec = 0;
    char kind;

    if (state->kind == tomato_break) {
        kind = 'b';
    } else {
        kind = 'f';
    }

    switch (state->mode) {
        case tomato_run:
            if (state->target_ts <= state->now_ts)
                delta = 0;
            else
                delta = state->target_ts - state->now_ts;
            result = div(delta, 60);
            min = result.quot;
            sec = result.rem;
            break;
        case tomato_pause:
            result = div(state->time_left, 60);
            min = result.quot;
            sec = result.rem;
            break;
        case tomato_ready:
        case tomato_setting:
            min = get_length(state);
            sec = 0;
            break;
    }

    sprintf(buf, " %c", kind);
    watch_display_text(WATCH_POSITION_TOP_RIGHT, buf);

    // If in setting mode, blink the minutes digits
    if (state->mode == tomato_setting && (subsecond % 2)) {
        sprintf(buf, "  %02d%2d", sec, state->done_count);
    } else {
        sprintf(buf, "%2d%02d%2d", min, sec, state->done_count);
    }
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

static void tomato_reset(tomato_state_t *state) {
    state->mode = tomato_ready;
    state->time_left = 0;
    movement_cancel_background_task_for_face(state->watch_face_index);
    watch_clear_indicator(WATCH_INDICATOR_BELL);
}

static void tomato_ring(tomato_state_t *state) {
    movement_play_signal();
    tomato_reset(state);
    if (state->kind == tomato_focus) {
        state->kind = tomato_break;
        state->done_count++;
    } else {
        state->kind = tomato_focus;
    }
}

void tomato_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(tomato_state_t));
        tomato_state_t *state = (tomato_state_t *)*context_ptr;
        memset(*context_ptr, 0, sizeof(tomato_state_t));
        state->mode = tomato_ready;
        state->kind = tomato_focus;
        state->done_count = 0;
        state->watch_face_index = watch_face_index;
        state->focus_min = TOMATO_DEFAULT_FOCUS_MIN;
        state->break_min = TOMATO_DEFAULT_BREAK_MIN;
    }
}

void tomato_face_activate(void *context) {
    tomato_state_t *state = (tomato_state_t *)context;
    if (state->mode == tomato_run) {
        state->now_ts = movement_get_utc_timestamp();
        watch_set_indicator(WATCH_INDICATOR_BELL);
    } else if (state->mode == tomato_setting) {
        movement_request_tick_frequency(4);
    }
    watch_set_colon();
}

bool tomato_face_loop(movement_event_t event, void *context) {
    tomato_state_t *state = (tomato_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            watch_display_text_with_fallback(WATCH_POSITION_TOP, "TOMATO", "TO");
            tomato_draw(state, event.subsecond);
            break;
        case EVENT_TICK:
            if (state->mode == tomato_run) {
                state->now_ts++;
            }
            tomato_draw(state, event.subsecond);
            break;
        case EVENT_LIGHT_BUTTON_DOWN:
            if (state->mode != tomato_setting) {
                movement_illuminate_led();
            }
            break;
        case EVENT_LIGHT_BUTTON_UP:
            if (state->mode == tomato_ready) {
                // Toggle between focus and break
                state->kind = (state->kind == tomato_break) ? tomato_focus : tomato_break;
            } else if (state->mode == tomato_setting) {
                // Cycle setting: focus -> break -> finish & exit
                if (state->kind == tomato_focus) {
                    state->kind = tomato_break;
                } else {
                    state->kind = tomato_focus;
                    state->mode = tomato_ready;
                    movement_request_tick_frequency(1);
                }
            }
            tomato_draw(state, event.subsecond);
            break;
        case EVENT_LIGHT_LONG_PRESS:
            // Long press LIGHT to enter or exit setting mode
            if (state->mode == tomato_ready) {
                state->mode = tomato_setting;
                state->kind = tomato_focus;
                movement_request_tick_frequency(4);
            } else if (state->mode == tomato_setting) {
                state->mode = tomato_ready;
                movement_request_tick_frequency(1);
            }
            tomato_draw(state, event.subsecond);
            break;
        case EVENT_ALARM_BUTTON_UP:
            switch (state->mode) {
                case tomato_run:
                    tomato_pause_timer(state);
                    break;
                case tomato_pause:
                    tomato_resume_timer(state);
                    break;
                case tomato_ready:
                    tomato_start(state);
                    break;
                case tomato_setting:
                    // Increment the selected timer (1 to 60 minutes)
                    if (state->kind == tomato_focus) {
                        state->focus_min = (state->focus_min >= 60) ? 1 : state->focus_min + 1;
                    } else {
                        state->break_min = (state->break_min >= 60) ? 1 : state->break_min + 1;
                    }
                    break;
            }
            tomato_draw(state, event.subsecond);
            break;
        case EVENT_ALARM_LONG_PRESS:
            if (state->mode == tomato_run || state->mode == tomato_pause) {
                tomato_reset(state);
            } else if (state->mode == tomato_ready) {
                state->done_count = 0;
            } else if (state->mode == tomato_setting) {
                // Long press ALARM in setting mode resets back to defaults
                if (state->kind == tomato_focus) {
                    state->focus_min = TOMATO_DEFAULT_FOCUS_MIN;
                } else {
                    state->break_min = TOMATO_DEFAULT_BREAK_MIN;
                }
            }
            tomato_draw(state, event.subsecond);
            break;
        case EVENT_BACKGROUND_TASK:
            tomato_ring(state);
            tomato_draw(state, event.subsecond);
            break;
        case EVENT_TIMEOUT:
            if (state->mode == tomato_setting) {
                state->mode = tomato_ready;
                movement_request_tick_frequency(1);
            } else if (state->mode != tomato_run) {
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
    if (state->mode == tomato_setting) {
        state->mode = tomato_ready;
        movement_request_tick_frequency(1);
    }
}
