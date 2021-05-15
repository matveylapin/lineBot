#include "MotorDriver.h"

#include "config.h"
#include "FastGPIO.h"
#include "LineRecognizer.h"

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

/*
reverse - changes the direction of rotation of the motor
*/
void MotorDriver::reverse() {
  _direction *= -1;
}

/*
setSpeed - the method sets the speed. at speed = 0 braking is performed. negative speed corresponds to reverse rotation
*/
void MotorDriver::setSpeed(int8_t speed) {
  speed = (int8_t)(_direction * speed);

  if (speed == 0) {
    fastAnalogWrite(_pwmPin, 0);
    fastDigitalWrite(_in1Pin, 1);
    fastDigitalWrite(_in2Pin, 1);
  } else {
    bool dir = (speed > 0) ? 1 : 0;
    fastDigitalWrite(_in1Pin, dir);
    fastDigitalWrite(_in2Pin, !dir);
    fastAnalogWrite(_pwmPin, constrain((int)((dir ? speed : -speed) * 2.55), 0, 255));
  }
}
