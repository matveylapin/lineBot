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
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++) {
    _calibratedBlack[i] = 1023;
    _calibratedWhite[i] = 0;
  }
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
    Serial.println(sensorValues[i]);
    uint16_t delta = _calibratedWhite[i] - _calibratedBlack[i];
    sensorValues[i] = 1000 - (uint16_t)(1000.0 * (float)sensorValues[i] / (float)delta);
    Serial.println(sensorValues[i]);
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
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++)
    _calibratedWhite[i] = 0;
  for (uint8_t i = 0; i < 10; i++) {
    readRaw(sensorValues);
    for (uint8_t j = 0; j < LR_SENSOR_COUNT; j++) {
      if (_calibratedWhite[j] < sensorValues[j])
        _calibratedWhite[j] = sensorValues[j];
    }
    delay(100);
    //vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

/*
calibrateBlack - calculates calibration values ​​for black.
*/
void LineRecognizer::calibrateBlack() {
  uint16_t sensorValues[LR_SENSOR_COUNT];
  for (uint8_t i = 0; i < LR_SENSOR_COUNT; i++)
    _calibratedBlack[i] = 1023;
  for (uint8_t i = 0; i < 10; i++) {
    readRaw(sensorValues);
    for (uint8_t j = 0; j < LR_SENSOR_COUNT; j++) {
      if (_calibratedBlack[j] > sensorValues[j])
        _calibratedBlack[j] = sensorValues[j];
    }
    delay(100);
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
int8_t _error = 123;
uint8_t linePins[] = LR_PINS;
uint16_t sensorValues[LR_SENSOR_COUNT] = { 0 };

LineRecognizer line(linePins);

void taskLineDetect(void* pvParameters) {
  (void)pvParameters;

  MSG_INFO("Start");

  for (;;) {

    lineStatus_t tempStatus = LD_STATUS_BLACK_LINE;

    sensorValues[0] = digitalRead(A4);
    sensorValues[1] = digitalRead(A3);
    sensorValues[2] = digitalRead(A1);
    sensorValues[3] = digitalRead(A2);

    if ( sensorValues[0] == HIGH && sensorValues[1] == LOW && sensorValues[2] == LOW && sensorValues[3] == LOW) {
      _error = 0;
      tempStatus = LD_STATUS_TURN_LEFT;
    } else if ( sensorValues[0] == LOW && sensorValues[1] == LOW && sensorValues[2] == LOW && sensorValues[3] == HIGH) {
      _error = 0;
      tempStatus = LD_STATUS_TURN_RIGHT;
    } else if (sensorValues[0] == HIGH && sensorValues[1] == HIGH && sensorValues[2] == HIGH && sensorValues[3] == HIGH) {
      _error = 0;
      tempStatus = LD_STATUS_WHITE;
    } else if (sensorValues[0] == LOW && sensorValues[1] == LOW && sensorValues[2] == LOW && sensorValues[3] == LOW) {
      _error = 0;
      tempStatus = LD_STATUS_BLACK;
    } else if ( sensorValues[0] == LOW) {
      _error = 75;
      tempStatus = LD_STATUS_BLACK_LINE;
    } else if ( sensorValues[3] == LOW ) {
      _error = -75;
      tempStatus = LD_STATUS_BLACK_LINE;
    } else if ( sensorValues[1] == LOW && sensorValues[2] == HIGH) {
      _error = 50;
      tempStatus = LD_STATUS_BLACK_LINE;
    } else if (sensorValues[1] == HIGH && sensorValues[2] == LOW) {
      _error = -50;
      tempStatus = LD_STATUS_BLACK_LINE;
    } else if (sensorValues[1] == LOW && sensorValues[2] == LOW) {
      _error = 0;
      tempStatus = LD_STATUS_BLACK_LINE;
    }
      
    _status = tempStatus;

    vTaskDelay(LD_TASK_TIME / portTICK_PERIOD_MS);
  }
}

void lineDetectRunCalibrate(uint8_t targetColor, uint8_t save) {
  if (targetColor == 0)
    line.calibrateWhite();
  else if (targetColor == 1)
    line.calibrateBlack();

  Serial.println(line._calibratedBlack[0]);
  Serial.println(line._calibratedWhite[0]);

  if (save == 1)
    line.calibrateSave();
}

void lineDetectInit() {
  line.init();
}

void getLineData(int8_t* error, lineStatus_t* status) {
  *status = _status;
  *error = _error;
}
