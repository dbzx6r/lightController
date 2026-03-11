#pragma once
#include <Arduino.h>

// Interrupt-driven EC11 rotary encoder + push button + KEY0 button.
// Call encoder_init() once in setup(), then poll encoder_getDelta(),
// encoder_getPush(), and encoder_getKey0() from loop().

void encoder_init();

// Returns accumulated tick delta since last call (positive = CW, negative = CCW).
int  encoder_getDelta();

// Returns true once per physical press of the encoder shaft button (SW).
bool encoder_getPush();

// Returns true once per physical press of KEY0.
bool encoder_getKey0();
