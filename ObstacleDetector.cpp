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
    
  }
}

void obstacleDetectorInit() {
  servo.attach(OD_SERVO_PIN);
  servo.write(90);
  radar.init(OD_RADAR_TRIG_PIN, OD_RADAR_ECHO_PIN);
}