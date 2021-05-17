#include "ObstacleDetector.h"

#include "config.h"
#include <Arduino.h>
#include <Servo.h>

void Ultrasonic::init(uint8_t trigPin, uint8_t echoPin)
{
  _trigPin = trigPin;
  _echoPin = echoPin;
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
}

uint16_t Ultrasonic::getDistance()
{
  long duration;

  //Send signal
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);

  //Measure signal duration
  duration = pulseIn(_echoPin, HIGH);

  //Calculating distance
  distance = (duration / 2) / 29.1;

  return distance;
}

Servo servo;
Ultrasonic radar;

void taskObstacleDetector(void* pvParameters) {
  (void)pvParameters;

  for (;;) {
    /*
    не успел доделать((( Тут будет вращение сервы и считываение данных с дальномера. Дальше будет искаться самое мальенькое растояние(будет значение активации чтоб не сбить банку)
    и после того как оно будет найдено(серва будет стараться "целиться" на него) в MotionEstimator поменяться источник ошибки для пид регулятора, будем стремиться к тому
    чтоб препядствие всегда находилось справа от робота и угол сервы был максимальным
    */
  }
}

void obstacleDetectorInit() {
  servo.attach(OD_SERVO_PIN);
  servo.write(90);
  radar.init(OD_RADAR_TRIG_PIN, OD_RADAR_ECHO_PIN);
}