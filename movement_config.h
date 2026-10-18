#ifndef MOVEMENT_CONFIG_H_
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

/* Determines the intensity of the led colors
 * Set a hex value 0-15 with 0x0 being off and 0xF being max intensity */
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
 * Valid values are:
 * 0: 60 seconds
 * 1: 2 minutes
 * 2: 5 minutes
 * 3: 30 minutes
 */
#define MOVEMENT_DEFAULT_TIMEOUT_INTERVAL 0

/* Set the timeout before switching to low energy mode
 * Valid values are:
 * 0: Never
 * 1: 10m
 * 2: 1h
 * 3: 2h
 * 4: 6h
 * 5: 12h
 * 6: 1 day
 * 7: 7 days
 */
#define MOVEMENT_DEFAULT_LOW_ENERGY_INTERVAL 2

/* Set the led duration in seconds
 * Valid values are:
 * 0: off
 * 1: 1s
 * 2: 3s
 * 3: 5s
 */
#define MOVEMENT_DEFAULT_LED_DURATION 1

#endif // MOVEMENT_CONFIG_H_
