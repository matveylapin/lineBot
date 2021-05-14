#pragma once

#include <stdint.h>

#include "config.h"

typedef enum {
  LD_STATUS_BLACK_LINE = 0,
  LD_STATUS_WHITE_LINE = 1,
  LD_STATUS_BLACK = 2,
  LD_STATUS_WHITE = 3
} lineStatus_t;

class LineRecognizer {
public:
  LineRecognizer(uint8_t* sensorPins);
  ~LineRecognizer();

  void init();

  void readRaw(uint16_t* sensorValues);                             // reading data from ADC
  void readCalibrated(uint16_t* sensorValues);                      // applying calibration to values
  uint8_t readLine(uint16_t* sensorValues, uint8_t whiteLine = 0);  // returns the deviation from the line

  void calibrateWhite();  // white calibration
  void calibrateBlack();  // black calibration
  void calibrateSave();   // saving calibration values ​​to EEPROM
  void calibrateLoad();   // reading calibration values ​​from EEPROM
private:
  uint8_t _sensorPins[LR_SENSOR_COUNT] = { 0 };
  uint16_t _calibratedBlack[LR_SENSOR_COUNT] = { 0 };
  uint16_t _calibratedWhite[LR_SENSOR_COUNT] = { LR_ADC_MAX };
};

void taskLineDetect(void* pvParameters);

void lineDetectInit();
void lineDetectRunCalibrate(uint8_t targetColor, uint8_t save);
void getLineData(int8_t* error, lineStatus_t* status);
