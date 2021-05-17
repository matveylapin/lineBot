#pragma once 

#include <stdint.h>

typedef enum {
  OD_STATUS_SEARCH = 0,
  OD_STATUS_CAUGHT = 1
} obstacleDetectorStatus_t;

class Ultrasonic
{
  private:
    int _trigPin;
    int _echoPin;

  public:
    uint16_t distance;
    void init(uint8_t trigPin, uint8_t echoPin);
    uint16_t getDistance();
};

void taskObstacleDetector(void* pvParameters);
void obstacleDetectorInit();