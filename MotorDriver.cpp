#include "motorDriver.h"

#include "config.h"

MotorDriver::MotorDriver(uint8_t in1Pin, uint8_t in2Pin, uint8_t pwmPin, int8_t direction) {
  _in1Pin = in1Pin;
  _in2Pin = in2Pin;
  _pwmPin = pwmPin;
  _direction = direction;
}

MotorDriver::~MotorDriver() {}

void MotorDriver::init() {
  pinMode(_in1Pin, OUTPUT);
  pinMode(_in2Pin, OUTPUT);
  pinMode(_pwmPin, OUTPUT);
}

void MotorDriver::setSpeed(int8_t speed) {
  speed = (int8_t)(_direction * speed * MOTOR_SPEED_SCALE);

  if (speed == 0) {
    analogWrite(_pwmPin, 0);
    digitalWrite(_in1Pin, 1);
    digitalWrite(_in2Pin, 1);
  } else {
    bool dir = (speed > 0) ? 1 : 0;
    digitalWrite(_in1Pin, dir);
    digitalWrite(_in2Pin, !dir);
    analogWrite(_pwmPin, constrain((int)((dir ? speed : -speed) * 2.55), 0, 255));
  }
}
