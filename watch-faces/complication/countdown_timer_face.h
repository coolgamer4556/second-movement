#ifndef COUNTDOWN_TIMER_FACE_H_
#define COUNTDOWN_TIMER_FACE_H_

#include <stdbool.h>
#include <stdint.h>
#include "movement.h"

typedef struct {
    uint32_t target_ts;          // Hardware RTC unix timestamp for countdown completion
    uint32_t remaining_seconds;   // Exact seconds left (preserved when paused or setting)
    uint8_t set_minutes;         // Base setting in minutes (0 to 60)
    bool is_running;             // True when actively counting down
    bool is_paused;              // True when paused midway
    bool repeat_mode;            // True: Auto-repeats timer; False: Restarts to 0
    bool is_active;              // True when currently visible on screen
    uint8_t watch_face_index;    // Assigned index in watch_faces[]
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
