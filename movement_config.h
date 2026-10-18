#ifndef MOVEMENT_CONFIG_H_
#define MOVEMENT_CONFIG_H_

#include "movement_faces.h"

const watch_face_t watch_faces[] = {
    // Primary Faces (Cycled with a short press of MODE)
    clock_face,
    stopwatch_face,
    tomato_face,

    // Secondary Faces (Accessed by long-pressing MODE)
    set_time_face,
    alarm_face
};

#define MOVEMENT_NUM_FACES (sizeof(watch_faces) / sizeof(watch_face_t))

/* 
 * Faces at and beyond this index are excluded from the main rotation 
 * and accessed via long press of the Mode button.
 * Since we have 2 secondary faces at the end, we subtract 2.
 */
#define MOVEMENT_SECONDARY_FACE_INDEX (MOVEMENT_NUM_FACES - 2)

/* Custom hourly chime tune. Check movement_custom_signal_tunes.h for options. */
#define SIGNAL_TUNE_DEFAULT

/* LED Colors and intensity (0x0 = off, 0xF = max) */
#define MOVEMENT_DEFAULT_RED_COLOR 0x0
#define MOVEMENT_DEFAULT_GREEN_COLOR 0xF
#define MOVEMENT_DEFAULT_BLUE_COLOR 0x0

/* Set to true for 24h mode or false for 12h mode */
#define MOVEMENT_DEFAULT_24H_MODE false

/* Enable or disable the sound on mode button press */
#define MOVEMENT_DEFAULT_BUTTON_SOUND true
#define MOVEMENT_DEFAULT_BUTTON_VOLUME WATCH_BUZZER_VOLUME_SOFT

/* 
 * Timeout before returning to the main clock face 
 * 0: 60 seconds, 1: 2 minutes, 2: 5 minutes, 3: 30 minutes
 */
#define MOVEMENT_DEFAULT_TIMEOUT_INTERVAL 0

/* 
 * Timeout before switching to low energy mode 
 * 0: Never, 1: 10m, 2: 1h, 3: 2h, 4: 6h, 5: 12h, 6: 1 day, 7: 7 days
 */
#define MOVEMENT_DEFAULT_LOW_ENERGY_INTERVAL 2

/* LED duration in seconds (0: off, 1: 1s, 2: 3s, 3: 5s) */
#define MOVEMENT_DEFAULT_LED_DURATION 1

#endif // MOVEMENT_CONFIG_H_#ifndef MOVEMENT_CONFIG_H_
#define MOVEMENT_CONFIG_H_

#include "movement_faces.h"

// PRIMARY FACES (Cycled with a short press of the MODE button)
#define PRIMARY_FACES(F) \
    F(clock) \
    F(stopwatch) \
    F(tomato)

// SECONDARY FACES (Accessed by holding down the MODE button)
#define SECONDARY_FACES(F) \
    F(set_time) \
    F(alarm)

// TERTIARY FACES
#define TERTIARY_FACES(F)

#define MOVEMENT_DEBOUNCE_TICKS 0

#endif // MOVEMENT_CONFIG_H_
