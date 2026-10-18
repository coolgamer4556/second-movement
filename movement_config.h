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

#endif // MOVEMENT_CONFIG_H_
