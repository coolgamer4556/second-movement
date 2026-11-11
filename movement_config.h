#ifndef MOVEMENT_CONFIG_H_
#define MOVEMENT_CONFIG_H_

#include "movement_faces.h"
#include "countdown_timer_face.h"

// 1. Declare active watch faces
const watch_face_t watch_faces[] = {
    clock_face,             // Index 0 (Primary)
    tomato_face,            // Index 1 (Primary)
    countdown_timer_face,   // Index 2 (Primary)
    set_time_face,          // Index 3 (Secondary)
};

#define MOVEMENT_NUM_FACES (sizeof(watch_faces) / sizeof(watch_face_t))

// 2. Isolate secondary face (set_time_face at Index 3):
// Short press MODE cycles: clock_face (0) -> tomato_face (1) -> countdown_timer_face (2) -> loops to 0
// Long press MODE from clock_face jumps directly to: set_time_face (3)
#define MOVEMENT_SECONDARY_FACE_INDEX (MOVEMENT_NUM_FACES - 1)

/* Sound & Buzzer Defaults */
#define SIGNAL_TUNE_DEFAULT
#define MOVEMENT_DEFAULT_BUTTON_SOUND true
#define MOVEMENT_DEFAULT_BUTTON_VOLUME WATCH_BUZZER_VOLUME_SOFT
#define MOVEMENT_DEFAULT_SIGNAL_VOLUME WATCH_BUZZER_VOLUME_LOUD
#define MOVEMENT_DEFAULT_ALARM_VOLUME WATCH_BUZZER_VOLUME_LOUD

/* Display & Backlight Defaults */
#define MOVEMENT_DEFAULT_24H_MODE false
#define MOVEMENT_DEFAULT_RED_COLOR 0x0
#define MOVEMENT_DEFAULT_GREEN_COLOR 0xF
#define MOVEMENT_DEFAULT_BLUE_COLOR 0x0
#define MOVEMENT_DEFAULT_LED_DURATION 2  // 3 seconds backlight duration

#define MOVEMENT_DEFAULT_TIMEOUT_INTERVAL 0
#define MOVEMENT_DEFAULT_LOW_ENERGY_INTERVAL 2
#define MOVEMENT_DEBOUNCE_TICKS 0

#endif // MOVEMENT_CONFIG_H_
