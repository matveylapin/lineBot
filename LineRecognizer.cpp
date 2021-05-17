#include "LineRecognizer.h"

#include <Arduino.h>
#include <EEPROM.h>

#include "FastGPIO.h"
#include <Arduino_FreeRTOS.h>

#define __TASK__ "LineDetect"

LineRecognizer::LineRecognizer(uint8_t* sensorPins) {
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) _sensorPins[i] = sensorPins[i];
}

LineRecognizer::~LineRecognizer() {}

/*
init - initialization
*/
void LineRecognizer::init() {
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) pinMode(_sensorPins[i], INPUT);
  ADCSRA |= (1 << ADPS1); 
  ADCSRA &= ~ ((1 << ADPS2) | (1 << ADPS0));
  ADCSRA &= ~(1 << ADATE);
  /*
  ADCSRA = 0;
  ADCSRB = 0;
  ADMUX |= (1 << REFS0);

  analog_ref = ADMUX;

  ADMUX |= (0 & 0x07);

  ADCSRA |= (1 << ADPS2) | (1 << ADPS0);
  ADCSRA &= ~ (1 << ADPS1);

  ADCSRA |= (1 << ADATE);
  ADCSRA |= (1 << ADIE);
  ADCSRA |= (1 << ADEN);
  ADCSRA |= (1 << ADSC);
  */
}

/*
readRaw - reading raw data from adc.
*/
void LineRecognizer::readRaw(uint16_t* sensorValues) {
  sensorValues[0] = fastAnalogRead(_sensorPins[0]);
  sensorValues[1] = fastAnalogRead(_sensorPins[1]);
  sensorValues[2] = fastAnalogRead(_sensorPins[2]);
  sensorValues[3] = fastAnalogRead(_sensorPins[3]);
}

/*
readCalibrated - reduction of values ​​from sensors [0, 1024) to [0, 1000] taking into account the calibration values.
*/
void LineRecognizer::readCalibrated(uint16_t* sensorValues) {
  this->readRaw(sensorValues);
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {

    uint16_t delta = _calibratedWhite[i] - _calibratedBlack[i];
    sensorValues[i] = 1000 - (uint16_t)(1000 * (float)sensorValues[i] / delta);
    if (sensorValues[i] > 1000) sensorValues[i] = 1000;
    else if (sensorValues[i] < 0)
      sensorValues[i] = 0;
  }
}

/*
readLine - the deviation from the line is calculated using the weighted average.
Returns:
1500 when the sensors are in the middle of the line;
0 - sensors are in the extreme left / right position;
3000 - in the extreme right / left position.
*/
uint8_t LineRecognizer::readLine(uint16_t* sensorValues, uint8_t whiteLine) {

  uint8_t onLine = 0;
  uint16_t sum = 0, weightedSum = 0;
  static uint8_t lastValue = 0;

  //this->readCalibrated(sensorValues);
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
    if (whiteLine) sensorValues[i] = 1000 - sensorValues[i];

    if (sensorValues[i] > LR_COLOR_BORDER) onLine = 1;  // checking if at least one sensor is on the line.

    if (sensorValues[i] > LR_NOISE_BORDER) {
      sum += sensorValues[i];
      weightedSum += sensorValues[i] * i;
    }
  }

  if (!onLine) {  // if no sensor is on the line, the function returns the last extreme value.
    if (lastValue > (LR_SENSOR_COUNT - 1) * 66.66667 / 2)
      return (uint8_t)((LR_SENSOR_COUNT - 1) * 66.66667);
    else
      return 0;
  }

  lastValue = (uint8_t)(((float)weightedSum / (float)sum) * 66.66667);

  return lastValue;
}

/*
calibrateWhite - calculates calibration values ​​for white.
*/
void LineRecognizer::calibrateWhite() {
  uint16_t sensorValues[LR_SENSOR_COUNT];
  memset(_calibratedWhite, 0, LR_SENSOR_COUNT * sizeof(uint16_t));
  for (uint8_t i = 0; i < 10; i++) {
    readRaw(sensorValues);
    for (uint8_t j = 0; j < LR_SENSOR_COUNT; j++) {
      if (_calibratedWhite[j] < sensorValues[j])
        _calibratedWhite[j] = sensorValues[j];
    }
    delay(50);
    //vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

/*
calibrateBlack - calculates calibration values ​​for black.
*/
void LineRecognizer::calibrateBlack() {
  uint16_t sensorValues[LR_SENSOR_COUNT];
  memset(_calibratedBlack, 1024, LR_SENSOR_COUNT * sizeof(uint16_t));
  for (uint8_t i = 0; i < 10; i++) {
    readRaw(sensorValues);
    for (uint8_t j = 0; j < LR_SENSOR_COUNT; j++) {
      if (_calibratedBlack[j] > sensorValues[j])
        _calibratedBlack[j] = sensorValues[j];
    }
    delay(50);
    //vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

/*
calibrateSave - loads calibration values into nonvolatile memory.
*/
void LineRecognizer::calibrateSave() {
  uint8_t locator = LR_EEPROM_ADDR;
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
    EEPROM.write(locator, _calibratedBlack[i] >> 8);
    EEPROM.write(locator + 1, _calibratedBlack[i] & 0xFF);
    locator += 2;
  }

  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
    EEPROM.write(locator, _calibratedWhite[i] >> 8);
    EEPROM.write(locator + 1, _calibratedWhite[i] & 0xFF);
    locator += 2;
  }
}

/*
calibrateLoad - loads calibration values from nonvolatile memory.
*/
void LineRecognizer::calibrateLoad() {
  uint8_t locator = LR_EEPROM_ADDR;
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
    _calibratedBlack[i] = (uint16_t)(EEPROM.read(locator) << 8) + (EEPROM.read(locator + 1));
    locator += 2;
  }

  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
    _calibratedWhite[i] = (uint16_t)(EEPROM.read(locator) << 8) + (EEPROM.read(locator + 1));
    locator += 2;
  }
}

lineStatus_t _status = LD_STATUS_BLACK_LINE;
int8_t _error = 0;
uint8_t linePins[] = LR_PINS;
uint16_t sensorValues[LR_SENSOR_COUNT] = { 0 };

LineRecognizer line(linePins);

void taskLineDetect(void* pvParameters) {
  (void)pvParameters;

  for (;;) {

    //int16_t lineError = (int8_t)((line.readLine(sensorValues) - 1500) / 30);
    line.readCalibrated(sensorValues);
    lineStatus_t tempStatus = LD_STATUS_BLACK_LINE;

    uint16_t lowValueCount = 0, midValueCount = 0, highValueCount = 0;
    for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
      if (sensorValues[i] >= LD_ON_BORDER)
        highValueCount++;
      else if (sensorValues[i] <= LD_OFF_BORDER)
        lowValueCount++;
      else
        midValueCount++;
    }

    if (lowValueCount == LR_SENSOR_COUNT) tempStatus = LD_STATUS_WHITE;
    else if (highValueCount == LR_SENSOR_COUNT)
      tempStatus = LD_STATUS_BLACK;
    else if ((highValueCount >= midValueCount) || (highValueCount > lowValueCount))
      tempStatus = LD_STATUS_WHITE_LINE;
    else if ((lowValueCount >= midValueCount) || (lowValueCount >= highValueCount))
      tempStatus = LD_STATUS_BLACK_LINE;

    _status = tempStatus;
    _error = (int8_t)((line.readLine(sensorValues, (_status == LD_STATUS_WHITE_LINE) ? 1 : 0) - 100));

    vTaskDelay(portTICK_PERIOD_MS * LD_TASK_FREQ);
  }
}

void lineDetectRunCalibrate(uint8_t targetColor, uint8_t save) {
  if (targetColor == 0)
    line.calibrateWhite();
  else if (targetColor == 1)
    line.calibrateBlack();

  if (save == 1) line.calibrateSave();
}

void lineDetectInit() {
  line.init();
  line.calibrateLoad();
}

void getLineData(int8_t* error, lineStatus_t* status) {
  status = &_status;
  error = &_error;
}
