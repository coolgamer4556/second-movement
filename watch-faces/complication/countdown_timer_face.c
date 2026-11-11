#include <stdlib.h>
#include <string.h>
#include "countdown_timer_face.h"
#include "watch.h"
#include "watch_utility.h"

static void countdown_schedule(countdown_timer_state_t *state) {
    state->target_ts = movement_get_utc_timestamp() + state->remaining_seconds;
    watch_date_time_t target_dt = watch_utility_date_time_from_unix_time(state->target_ts, 0);
    movement_schedule_background_task_for_face(state->watch_face_index, target_dt);
    if (state->is_active) {
        watch_set_indicator(WATCH_INDICATOR_BELL);
    }
}

static void countdown_reset(countdown_timer_state_t *state) {
    state->is_running = false;
    state->is_paused = false;
    state->set_minutes = 0;
    state->remaining_seconds = 0;
    movement_cancel_background_task_for_face(state->watch_face_index);
    if (state->is_active) {
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    }
}

static void countdown_finish(countdown_timer_state_t *state) {
    // Play the classic Casio double pip chime
    movement_play_signal();

    if (state->repeat_mode && state->set_minutes > 0) {
        // Auto-repeat: Reload set minutes and keep counting down
        state->remaining_seconds = state->set_minutes * 60;
        countdown_schedule(state);
    } else {
        // Single run: Reset to 0 idle state
        countdown_reset(state);
    }
}

static void countdown_draw(countdown_timer_state_t *state) {
    if (!state->is_active) return;

    char buf[16];

    // Show Repeat status in the top complication: "RP" (Repeat) or "1X" (Single run)
    if (state->repeat_mode) {
        watch_display_text(WATCH_POSITION_TOP_RIGHT, "RP");
        watch_set_indicator(WATCH_INDICATOR_LAP);
    } else {
        watch_display_text(WATCH_POSITION_TOP_RIGHT, "1X");
        watch_clear_indicator(WATCH_INDICATOR_LAP);
    }

    if (state->is_running) {
        // RUNNING DISPLAY (Inverse/Active Countdown mode):
        // Shows remaining Minutes and Seconds counting down: [ MM SS 00 ]
        uint32_t now = movement_get_utc_timestamp();
        uint32_t rem = (state->target_ts > now) ? (state->target_ts - now) : 0;
        div_t d = div(rem, 60);
        sprintf(buf, "%02d%02d  ", d.quot, d.rem);
        watch_set_indicator(WATCH_INDICATOR_BELL);
    } else if (state->is_paused) {
        // PAUSED DISPLAY: Frozen remaining Minutes and Seconds
        div_t d = div(state->remaining_seconds, 60);
        sprintf(buf, "%02d%02dPA", d.quot, d.rem);
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    } else {
        // IDLE / SETTING DISPLAY:
        // Shows preset minutes clearly: "CD  XX 00" or " 0" when idle
        if (state->set_minutes == 0) {
            sprintf(buf, "   0  ");
        } else {
            sprintf(buf, "%2d00  ", state->set_minutes);
        }
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    }

    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void countdown_timer_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(countdown_timer_state_t));
        memset(*context_ptr, 0, sizeof(countdown_timer_state_t));
        countdown_timer_state_t *state = (countdown_timer_state_t *)*context_ptr;
        state->set_minutes = 0;
        state->remaining_seconds = 0;
        state->is_running = false;
        state->is_paused = false;
        state->repeat_mode = false;
        state->is_active = false;
        state->watch_face_index = watch_face_index;
    }
}

void countdown_timer_face_activate(void *context) {
    countdown_timer_state_t *state = (countdown_timer_state_t *)context;
    state->is_active = true;

    watch_set_colon();
    watch_display_text_with_fallback(WATCH_POSITION_TOP, "TIMER", "CD");
    countdown_draw(state);
}

bool countdown_timer_face_loop(movement_event_t event, void *context) {
    countdown_timer_state_t *state = (countdown_timer_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            countdown_draw(state);
            break;

        case EVENT_TICK:
            if (state->is_running) {
                uint32_t now = movement_get_utc_timestamp();
                if (now >= state->target_ts) {
                    countdown_finish(state);
                }
            }
            countdown_draw(state);
            break;

        case EVENT_LIGHT_BUTTON_UP:
            // Short press LIGHT: Increment minutes (1-60)
            if (!state->is_running) {
                state->is_paused = false;
                state->set_minutes++;
                if (state->set_minutes > 60) {
                    state->set_minutes = 1; // loops back
                }
                state->remaining_seconds = state->set_minutes * 60;
                countdown_draw(state);
            }
            break;

        case EVENT_LIGHT_LONG_PRESS:
            // Long press LIGHT: Turn on Backlight LED
            movement_illuminate_led();
            break;

        case EVENT_ALARM_BUTTON_UP:
            // Short press ALARM: Confirm / Start / Pause / Resume
            if (state->is_running) {
                // Pause countdown
                state->is_running = false;
                state->is_paused = true;
                uint32_t now = movement_get_utc_timestamp();
                state->remaining_seconds = (state->target_ts > now) ? (state->target_ts - now) : 0;
                movement_cancel_background_task_for_face(state->watch_face_index);
            } else if (state->is_paused) {
                // Resume countdown
                state->is_running = true;
                state->is_paused = false;
                countdown_schedule(state);
            } else {
                // In Ready/Idle state
                if (state->set_minutes > 0) {
                    // Start countdown
                    state->is_running = true;
                    state->remaining_seconds = state->set_minutes * 60;
                    countdown_schedule(state);
                } else {
                    // If at 0, pressing ALARM toggles Repeat Mode ON / OFF!
                    state->repeat_mode = !state->repeat_mode;
                }
            }
            countdown_draw(state);
            break;

        case EVENT_ALARM_LONG_PRESS:
            // Long press ALARM: Reset back to 0
            countdown_reset(state);
            countdown_draw(state);
            break;

        case EVENT_BACKGROUND_TASK:
            // RTC alarm woke up watch in sleep or from another face
            if (state->is_running) {
                uint32_t now = movement_get_utc_timestamp();
                if (now >= state->target_ts) {
                    countdown_finish(state);
                }
            }
            if (state->is_active) {
                countdown_draw(state);
            }
            break;

        case EVENT_TIMEOUT:
            // Inactivity timeout (60s): Return to clock face if stopped
            if (!state->is_running) {
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
