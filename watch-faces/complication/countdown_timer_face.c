#include <stdlib.h>
#include <string.h>
#include "countdown_timer_face.h"
#include "watch.h"
#include "watch_utility.h"

static void cd_schedule_target(countdown_timer_state_t *state, uint32_t seconds) {
    state->target_ts = movement_get_utc_timestamp() + seconds;
    watch_date_time_t target_dt = watch_utility_date_time_from_unix_time(state->target_ts, 0);
    movement_schedule_background_task_for_face(state->watch_face_index, target_dt);
    if (state->is_active) {
        watch_set_indicator(WATCH_INDICATOR_BELL);
    }
}

static void cd_full_reset(countdown_timer_state_t *state) {
    state->mode = CD_IDLE;
    state->set_minutes = 0;
    state->remaining_seconds = 0;
    state->repeat_count = 0;
    movement_cancel_background_task_for_face(state->watch_face_index);
    if (state->is_active) {
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    }
}

static void cd_handle_expiration(countdown_timer_state_t *state) {
    // Classic 4-pair Casio alarm: beep-beep, beep-beep, beep-beep, beep-beep
    movement_play_alarm();

    if (state->repeat_enabled && state->set_minutes > 0) {
        // Auto-repeat path: increment repeat counter and chain next run
        if (state->repeat_count < 99) {
            state->repeat_count++;
        }
        cd_schedule_target(state, state->set_minutes * 60);
    } else {
        // Single-run path: reset back to 0
        cd_full_reset(state);
    }
}

static void cd_draw(countdown_timer_state_t *state) {
    if (!state->is_active) return;

    char buf[16];
    uint32_t remaining;

    if (state->mode == CD_RUNNING) {
        uint32_t now = movement_get_utc_timestamp();
        remaining = (state->target_ts > now) ? (state->target_ts - now) : 0;
    } else if (state->mode == CD_PAUSED) {
        remaining = state->remaining_seconds;
    } else {
        // IDLE: Show selected setting minutes
        remaining = state->set_minutes * 60;
    }

    div_t result = div(remaining, 60);
    uint8_t min = (uint8_t)result.quot;
    uint8_t sec = (uint8_t)result.rem;

    // Top row display:
    // "CUT  S" when Repeat is OFF (Single run)
    // "CUT RP" when Repeat is ON (Auto-repeat)
    if (state->repeat_enabled) {
        watch_display_text_with_fallback(WATCH_POSITION_TOP, "CUT RP", "RP");
    } else {
        watch_display_text_with_fallback(WATCH_POSITION_TOP, "CUT  S", " S");
    }

    // Bottom row: MM:SS and Repeat Tally (e.g., " 5:00  0")
    sprintf(buf, "%2d%02d%2d", min, sec, state->repeat_count);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void countdown_timer_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(countdown_timer_state_t));
        memset(*context_ptr, 0, sizeof(countdown_timer_state_t));
        countdown_timer_state_t *state = (countdown_timer_state_t *)*context_ptr;
        state->mode = CD_IDLE;
        state->set_minutes = 0;
        state->remaining_seconds = 0;
        state->repeat_count = 0;
        state->repeat_enabled = false;
        state->watch_face_index = watch_face_index;
        state->is_active = false;
    }
}

void countdown_timer_face_activate(void *context) {
    countdown_timer_state_t *state = (countdown_timer_state_t *)context;
    state->is_active = true;

    if (state->mode == CD_RUNNING) {
        watch_set_indicator(WATCH_INDICATOR_BELL);
    } else {
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    }

    watch_set_colon();
}

bool countdown_timer_face_loop(movement_event_t event, void *context) {
    countdown_timer_state_t *state = (countdown_timer_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            cd_draw(state);
            break;

        case EVENT_TICK:
            if (state->mode == CD_RUNNING) {
                uint32_t now = movement_get_utc_timestamp();
                if (now >= state->target_ts) {
                    cd_handle_expiration(state);
                }
            }
            cd_draw(state);
            break;

        case EVENT_LIGHT_BUTTON_UP:
            // Short press LIGHT:
            if (state->mode == CD_IDLE || state->mode == CD_PAUSED) {
                // Increment minutes: 1 -> 60 -> 0
                state->set_minutes = (state->set_minutes + 1) % 61;
                state->remaining_seconds = state->set_minutes * 60;
            } else if (state->mode == CD_RUNNING) {
                // While running, toggle Repeat mode on the fly
                state->repeat_enabled = !state->repeat_enabled;
            }
            cd_draw(state);
            break;

        case EVENT_LIGHT_LONG_PRESS:
            // Long press LIGHT: Illuminate backlight
            movement_illuminate_led();
            break;

        case EVENT_ALARM_BUTTON_UP:
            // Short press ALARM:
            if (state->mode == CD_IDLE) {
                if (state->set_minutes == 0) {
                    // At 0 minutes, toggle Repeat mode
                    state->repeat_enabled = !state->repeat_enabled;
                } else {
                    // Start countdown
                    state->mode = CD_RUNNING;
                    state->remaining_seconds = state->set_minutes * 60;
                    cd_schedule_target(state, state->remaining_seconds);
                }
            } else if (state->mode == CD_RUNNING) {
                // Pause countdown
                state->mode = CD_PAUSED;
                uint32_t now = movement_get_utc_timestamp();
                state->remaining_seconds = (state->target_ts > now) ? (state->target_ts - now) : 0;
                movement_cancel_background_task_for_face(state->watch_face_index);
                watch_clear_indicator(WATCH_INDICATOR_BELL);
            } else if (state->mode == CD_PAUSED) {
                // Resume countdown
                if (state->remaining_seconds > 0) {
                    state->mode = CD_RUNNING;
                    cd_schedule_target(state, state->remaining_seconds);
                }
            }
            cd_draw(state);
            break;

        case EVENT_ALARM_LONG_PRESS:
            // Long press ALARM: Full reset back to 0
            cd_full_reset(state);
            cd_draw(state);
            break;

        case EVENT_BACKGROUND_TASK:
            // RTC wake-up while in background/sleep
            if (state->mode == CD_RUNNING) {
                uint32_t now = movement_get_utc_timestamp();
                if (now >= state->target_ts) {
                    cd_handle_expiration(state);
                }
            }
            if (state->is_active) {
                cd_draw(state);
            }
            break;

        case EVENT_TIMEOUT:
            // Return to main clock if left paused or idle for 60s
            if (state->mode != CD_RUNNING) {
                movement_move_to_face(0);
            }
            break;

        default:
            movement_default_loop_handler(event);
            break;
    }

    return true;
}

void countdown_timer_face_resign(void *context) {
    countdown_timer_state_t *state = (countdown_timer_state_t *)context;
    state->is_active = false;
}
