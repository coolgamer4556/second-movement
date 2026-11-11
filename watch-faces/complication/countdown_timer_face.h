#ifndef COUNTDOWN_TIMER_FACE_H_
#define COUNTDOWN_TIMER_FACE_H_

#include <stdbool.h>
#include <stdint.h>
#include "movement.h"

typedef enum {
    CD_IDLE = 0,
    CD_RUNNING,
    CD_PAUSED
} cd_mode_t;

typedef struct {
    uint32_t target_ts;          // Target Unix timestamp for current countdown
    uint32_t remaining_seconds;  // Stored seconds when paused
    uint8_t set_minutes;         // Selected interval in minutes (0 - 60)
    uint8_t repeat_count;        // Number of completed repeats (0 - 99)
    bool repeat_enabled;         // True: auto-repeat; False: reset to 0 on finish
    cd_mode_t mode;              // IDLE, RUNNING, or PAUSED
    uint8_t watch_face_index;    // Assigned index in watch_faces[]
    bool is_active;              // True when visible on screen
} countdown_timer_state_t;

void countdown_timer_face_setup(uint8_t watch_face_index, void ** context_ptr);
void countdown_timer_face_activate(void *context);
bool countdown_timer_face_loop(movement_event_t event, void *context);
void countdown_timer_face_resign(void *context);

#define countdown_timer_face ((const watch_face_t){ \
    countdown_timer_face_setup, \
    countdown_timer_face_activate, \
    countdown_timer_face_loop, \
    countdown_timer_face_resign, \
    NULL, \
})

#endif // COUNTDOWN_TIMER_FACE_H_
