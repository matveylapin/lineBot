#include "PIDlib.h"

PID::PID(float p, float i, float d) {
  this->setFactors(p, i, d);
  this->clearErrorArray();
}

PID::~PID() {}

void PID::setFactors(float p, float i, float d) {
  _p = p;
  _i = i;
  _d = d;
}

void PID::clearErrorArray() {
  memset(_errorArray, 0, PID_EEPROM_ADDR);
  arrayIterator = -1;
}

float PID::calculate(int8_t error) {

  arrayIterator = (arrayIterator + 1) % 10;
  _errorArray[arrayIterator] = error;

  float pComponent = error * _p;
  float dComponent = (error - _errorArray[(arrayIterator + 11) % 10]) * _d;
  float iComponent = 0;

  for (uint8_t i = 0; i < PID_ERROR_ARRAY_SIZE; i++) iComponent += _errorArray[i];
  iComponent = iComponent / 10 * _i;

  return pComponent + iComponent + dComponent;
}
