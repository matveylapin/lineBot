#include <EEPROM.h>
#include "LineRecognizer.h"

LineRecognizer::LineRecognizer(uint8_t* sensorPins) {
  memcpy(_sensorPins, sensorPins, LR_SENSOR_COUNT);
}

LineRecognizer::~LineRecognizer() {}

/*
init - initialization
*/
void LineRecognizer::init() {
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) pinMode(_sensorPins[i], OUTPUT);
}

/*
readRaw - reading raw data from adc.
*/
void LineRecognizer::readRaw(uint16_t* sensorValues) {
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) sensorValues[i] = analogRead(_sensorPins[i]);
}

/*
readCalibrated - reduction of values ​​from sensors [0, 1024) to [0, 1000] taking into account the calibration values.
*/
void LineRecognizer::readCalibrated(uint16_t* sensorValues) {

  this->readRaw(sensorValues);
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {

    uint16_t delta = _calibratedWhite[i] - _calibratedBlack[i];
    sensorValues[i] = (uint16_t)(1000 * (float)sensorValues[i] / delta);
    if (sensorValues[i] > 1000) sensorValues[i] = 1000;
    else if (sensorValues[i] < 0) sensorValues[i] = 0;
  }
}

/*
readLine - the deviation from the line is calculated using the weighted average.
Returns:
1500 when the sensors are in the middle of the line;
0 - sensors are in the extreme left / right position;
3000 - in the extreme right / left position.
*/
uint16_t LineRecognizer::readLine(uint16_t* sensorValues, uint8_t whiteLine) {

  uint8_t onLine = 0;
  uint16_t sum = 0, weightedSum = 0;
  static uint16_t lastValue = 0;

  this->readCalibrated(sensorValues);
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
    if (!whiteLine) sensorValues[i] = 1000 - sensorValues[i];

    if (sensorValues[i] > LR_COLOR_BORDER) onLine = 1;      // checking if at least one sensor is on the line.

    if (sensorValues[i] > LR_NOISE_BORDER) {
      sum += sensorValues[i];
      weightedSum += sensorValues[i] * i;
    }
  }

  if (!onLine) {                                            // if no sensor is on the line, the function returns the last extreme value.
    if (lastValue > (LR_SENSOR_COUNT - 1) * 1000 / 2)
      return (LR_SENSOR_COUNT - 1) * 1000;
    else
      return 0;
  }

  return (uint16_t)((float)(weightedSum / sum) * 1000);
}

/*
calibrateWhite - calculates calibration values ​​for white.
*/
void LineRecognizer::calibrateWhite() {
  uint16_t* sensorValues;
  memset(_calibratedWhite, 0, LR_SENSOR_COUNT);

  for (uint8_t i = 0; i < 10; i++) {

    this->readRaw(sensorValues);
    for (uint8_t j = 0; j < LR_SENSOR_COUNT; j++) {
      if (i == 0 || _calibratedWhite[j] < sensorValues[j]) 
        _calibratedWhite[j] = sensorValues[j];
    }
  }
}

/*
calibrateBlack - calculates calibration values ​​for black.
*/
void LineRecognizer::calibrateBlack() {
  uint16_t* sensorValues;
  memset(_calibratedBlack, 0, LR_SENSOR_COUNT);

  for (uint8_t i = 0; i < 10; i++) {

    this->readRaw(sensorValues);
    for (uint8_t j = 0; j < LR_SENSOR_COUNT; j++) {
      if (i == 0 || _calibratedBlack[j] > sensorValues[j]) 
        _calibratedBlack[j] = sensorValues[j];
    }
  }
}

/*
calibrateSave - loads calibration values into nonvolatile memory.
*/
void LineRecognizer::calibrateSave() {
  uint8_t locator = 0;
  for (uint8_t i = 0; i < LR_SENSOR_COUNT * 2; i++) {
    EEPROM.write(locator, _calibratedBlack[i] >> 8);
    EEPROM.write(locator + 1, _calibratedBlack[i] & 0xFF);
    locator++;
  }

  for (uint8_t i = 0; i < LR_SENSOR_COUNT * 2; i++) {
    EEPROM.write(locator, _calibratedWhite[i] >> 8);
    EEPROM.write(locator + 1, _calibratedWhite[i] & 0xFF);
    locator++;
  }
}

/*
calibrateLoad - loads calibration values from nonvolatile memory.
*/
void LineRecognizer::calibrateLoad() {
  uint8_t locator = 0;
  for (uint8_t i = 0; i < LR_SENSOR_COUNT * 2; i++) {
    _calibratedBlack[i] = (EEPROM.read(locator) << 8) + (EEPROM.read(locator + 1));
    locator++;
  }

  for (uint8_t i = 0; i < LR_SENSOR_COUNT * 2; i++) {
    _calibratedWhite[i] = (EEPROM.read(locator) << 8) + (EEPROM.read(locator + 1));
    locator++;
  }
}