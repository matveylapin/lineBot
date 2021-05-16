#pragma once

#include <stdint.h>

int fastAnalogRead(uint8_t pin);
void fastDigitalWrite(uint8_t pin, bool x);
void fastAnalogWrite(uint8_t pin, uint16_t duty);