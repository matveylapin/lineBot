#pragma once

#include <stdint.h>
#include <Arduino.h>

class MotorDriver {
private:
  uint8_t _in1Pin, _in2Pin, _pwmPin;
  int8_t _direction;

public:
  MotorDriver(uint8_t in1Pin, uint8_t in2Pin, uint8_t pwmPin, int8_t direction = 1);
  ~MotorDriver();

  void init();

  void reverse();               // changes the direction of rotation of the motor
  void setSpeed(int8_t speed);  // set speed -100 <= speed <= 100
};

void motorsInit();