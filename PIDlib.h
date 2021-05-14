#pragma once

#include <stdint.h>
#include <string.h>

#include "config.h"

class PID {
  private:
  float _p, _i, _d;
  int8_t _errorArray[PID_EEPROM_ADDR];
  uint8_t arrayIterator = -1;

  public:
    PID(float p, float i, float d);
    ~PID();

    void setFactors(float p, float i, float d);
    void clearErrorArray();
    
    float calculate(int8_t error);
};