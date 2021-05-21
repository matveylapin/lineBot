#include "ObstacleDetector.h"

#include <Arduino.h>
#include <Servo.h>
#include "config.h"
#include <Arduino_FreeRTOS.h>

#define __TASK__ " ObstacleDetector "

void Ultrasonic::init(uint8_t trigPin, uint8_t echoPin)
{
  _trigPin = trigPin;
  _echoPin = echoPin;
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
}

uint16_t Ultrasonic::getDistance()
{
  uint32_t duration;
  

  //Send signal
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(200);
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);

  //Measure signal duration
  duration = pulseIn(_echoPin, HIGH);

  //Calculating distance
  distance = (uint16_t)((duration / 2) / 29.1);

  return distance;
}

obstacleDetectorStatus_t status = OD_STATUS_SEARCH;

Servo servo;
Ultrasonic radar;

void taskObstacleDetector(void* pvParameters) {
  (void)pvParameters;

  MSG_INFO("Start");

  //uint8_t tmpAngle = 90;
  status = OD_STATUS_SEARCH;

  for (;;) {
    /*
    не успел доделать((( Тут будет вращение сервы и считываение данных с дальномера. Дальше будет искаться самое мальенькое растояние(будет значение активации чтоб не сбить банку)
    и после того как оно будет найдено(серва будет стараться "целиться" на него) в MotionEstimator поменяться источник ошибки для пид регулятора, будем стремиться к тому
    чтоб препядствие всегда находилось справа от робота и угол сервы был максимальным
    */
    uint16_t distance = radar.getDistance();

    MSG_INFO(distance);

    //if ((distance < OD_RADAR_TOGGLE_VALUE) || (status == OD_STATUS_SEARCH)) {
    //  status = OD_STATUS_CAUGHT;
    //}// else if (status == OD_STATUS_CAUGHT) {
    //}

    vTaskDelay(OD_TASK_TIME / portTICK_PERIOD_MS);
  }
}

void obstacleDetectorInit() {
  servo.attach(OD_SERVO_PIN);
  servo.write(90);
  radar.init(OD_RADAR_TRIG_PIN, OD_RADAR_ECHO_PIN);
}