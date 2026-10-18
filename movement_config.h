#ifndef MOVEMENT_CONFIG_H_
#define MOVEMENT_CONFIG_H_

#include "movement_faces.h"

const watch_face_t watch_faces[] = {
    // --- PRIMARY FACES (Short press MODE) ---
    clock_face,          // 1. Time / Date
    stopwatch_face,      // 2. Stopwatch (standard 1-hr)
    tomato_face,         // 3. Pomodoro Timer (tomato_face)

    // --- SECONDARY FACES (Long press MODE from Clock) ---
    set_time_face,       // 1. Adjust time/date
    alarm_face,          // 2. Alarms
};

#define MOVEMENT_NUM_FACES (sizeof(watch_faces) / sizeof(watch_face_t))

/* 
 * Faces at this index and higher are excluded from the main rotation 
 * and only accessible by holding down the MODE button.
 * Since there are 2 secondary faces, we subtract 2.
 */
#define MOVEMENT_SECONDARY_FACE_INDEX (MOVEMENT_NUM_FACES - 2)

/* Custom hourly chime tune. Check movement_custom_signal_tunes.h for options. */
#define SIGNAL_TUNE_DEFAULT

/* Determines the intensity of the led colors
 * Set a hex value 0-15 with 0x0 being off and 0xF being max intensity
 */
#define MOVEMENT_DEFAULT_RED_COLOR 0x0
#define MOVEMENT_DEFAULT_GREEN_COLOR 0xF
#define MOVEMENT_DEFAULT_BLUE_COLOR 0x0

/* Set to true for 24h mode or false for 12h mode */
#define MOVEMENT_DEFAULT_24H_MODE false

/* Enable or disable the sound on mode button press */
#define MOVEMENT_DEFAULT_BUTTON_SOUND true

#define MOVEMENT_DEFAULT_BUTTON_VOLUME WATCH_BUZZER_VOLUME_SOFT
#define MOVEMENT_DEFAULT_SIGNAL_VOLUME WATCH_BUZZER_VOLUME_LOUD
#define MOVEMENT_DEFAULT_ALARM_VOLUME WATCH_BUZZER_VOLUME_LOUD

/* Set the timeout before switching back to the main watch face
 * 0: 60 seconds, 1: 2 minutes, 2: 5 minutes, 3: 30 minutes
 */
#define MOVEMENT_DEFAULT_TIMEOUT_INTERVAL 0

/* Set the timeout before switching to low energy mode
 * 0: Never, 1: 10m, 2: 1h, 3: 2h, 4: 6h, 5: 12h, 6: 1d, 7: 7d
 */
#define MOVEMENT_DEFAULT_LOW_ENERGY_INTERVAL 2

/* Set the led duration: 0: off, 1: 1s, 2: 3s, 3: 5s */
#define MOVEMENT_DEFAULT_LED_DURATION 1

#define MOVEMENT_DEBOUNCE_TICKS 0

#endif // MOVEMENT_CONFIG_H_
