#ifndef MOVEMENT_CONFIG_H_
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
