#include "PIDlib.h"

#include <EEPROM.h>

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

void PID::setFactorP(float p) {
  _p = p;
}

void PID::setFactorI(float i) {
  _i = i;
}

void PID::setFactorD(float d) {
  _d = d;
}

void PID::saveFactors() {
  for (uint8_t i = 0; i < sizeof(float); i++) {
    EEPROM.write(PID_EEPROM_ADDR + i, (uint32_t)_p >> 8 * (3 - i));
    EEPROM.write(PID_EEPROM_ADDR + i + 4, (uint32_t)_i >> 8 * (3 - i));
    EEPROM.write(PID_EEPROM_ADDR + i + 8, (uint32_t)_d >> 8 * (3 - i));
  }
}

void PID::loadFactors() {
  for (uint8_t i = 0; i < sizeof(float); i++) {
    _p += (float)(EEPROM.read(PID_EEPROM_ADDR + i) << 8 * (3 - i));
    _i += (float)(EEPROM.read(PID_EEPROM_ADDR + i + 4) << 8 * (3 - i));
    _d += (float)(EEPROM.read(PID_EEPROM_ADDR + i + 8) << 8 * (3 - i));
  }
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

  float correction = pComponent + iComponent + dComponent;

  if (correction < -1)
    return -1.0;
  else if (correction > 1)
    return 1.0;

  return correction;
}
