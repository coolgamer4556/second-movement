#ifndef MOVEMENT_CONFIG_H_
#define MOVEMENT_CONFIG_H_

#include "movement_faces.h"

const watch_face_t watch_faces[] = {
    // --- PRIMARY FACES (Short press MODE) ---
    clock_face,
    fast_stopwatch_face,
    tomato_face,

    // --- SECONDARY FACES (Hold MODE) ---
    alarm_face,
    set_time_face,
    settings_face,
};

#define MOVEMENT_NUM_FACES (sizeof(watch_faces) / sizeof(watch_face_t))
#define MOVEMENT_SECONDARY_FACE_INDEX 3

#endif // MOVEMENT_CONFIG_H_#ifndef MOVEMENT_CONFIG_H_
#define MOVEMENT_CONFIG_H_

#include "movement_faces.h"

// PRIMARY FACES (Cycled with a short press of the MODE button)
#define PRIMARY_FACES(F) \
  F(clock) \
  F(fast_stopwatch) \
  F(tomato) \

// SECONDARY FACES (Accessed by holding down the MODE button)
#define SECONDARY_FACES(F) \
  F(alarm) \
  F(set_time) \
  F(preferences) \

#define TERTIARY_FACES(F)

#define MOVEMENT_DEBOUNCE_TICKS 0

#endif // MOVEMENT_CONFIG_H_
