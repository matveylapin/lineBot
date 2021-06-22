#include "MotionEstimator.h"

#include "config.h"
#include "PIDlib.h"
#include "FastGPIO.h"
#include "MotorDriver.h"
#include "LineRecognizer.h"
#include "ObstacleDetector.h"
#include <Arduino_FreeRTOS.h>
#include <Servo.h>

#define __TASK__ " MotionEstimator "

MotorDriver motorLeft(MOTOR_A_DRIVER_IN1_PIN, MOTOR_A_DRIVER_IN2_PIN, MOTOR_A_DRIVER_PWM_PIN, -1);
MotorDriver motorRight(MOTOR_B_DRIVER_IN1_PIN, MOTOR_B_DRIVER_IN2_PIN, MOTOR_B_DRIVER_PWM_PIN, -1);

PID pid(PID_DEFAULT_P, PID_DEFAULT_D, PID_DEFAULT_D);

Ultrasonic radar;
Servo servo;

void taskMotionEstimator(void *pvParameters) {
  (void)pvParameters;

  MSG_INFO("Start");
  int8_t turns[] = {0, -1, 0};
  int8_t turnIterator = 0;
  uint8_t  blackCount = 0;

  lineStatus_t status;
  int8_t error;
  for (;;) {
    getLineData(&error, &status);MSG_INFO(error);

    if (status == LD_STATUS_BLACK_LINE || status == LD_STATUS_WHITE_LINE) {   // if the line is white or black, start a regular ride along the line
      float correction = pid.calculate(error);

      motorLeft.setSpeed(ME_AVG_SPEED * (1 + correction));
      motorRight.setSpeed(ME_AVG_SPEED * (1 - correction));

    } else if ((status == LD_STATUS_TURN_LEFT) && (turns[turnIterator] == -1) && (turnIterator != -1)) {   //if we meet a black stop line, we just go straight until we meet again

      motorLeft.setSpeed(-ME_AVG_SPEED);
      motorRight.setSpeed(ME_AVG_SPEED + 5);

      if (turnIterator + 1 != sizeof(turns)) turnIterator++;
      else turnIterator = -1;
      MSG_INFO("left");

      vTaskDelay(1000 / portTICK_PERIOD_MS);

    } else if ((status == LD_STATUS_TURN_RIGHT) && (turns[turnIterator] == 1) && (turnIterator != -1)) {   //if we meet a black stop line, we just go straight until we meet again

      motorLeft.setSpeed(ME_AVG_SPEED + 5);
      motorRight.setSpeed(-ME_AVG_SPEED);

      if (turnIterator + 1 != sizeof(turns)) turnIterator++;
      else turnIterator = -1;
      MSG_INFO("right");

      vTaskDelay(1000 / portTICK_PERIOD_MS);

    } else if ((status == LD_STATUS_TURN_RIGHT || status == LD_STATUS_TURN_LEFT) && (turns[turnIterator] == 0) && (turnIterator != -1)) {
      if (turnIterator + 1 != sizeof(turns)) turnIterator++;
      else turnIterator = -1;
      MSG_INFO("no");
      vTaskDelay(1500 / portTICK_PERIOD_MS);

    } else if (status == LD_STATUS_BLACK) {
      if (blackCount == 0) {
      motorLeft.setSpeed(ME_AVG_SPEED);
      motorRight.setSpeed(ME_AVG_SPEED);
      while(radar.getDistance() > OD_RADAR_TOGGLE_VALUE) vTaskDelay(100 / portTICK_PERIOD_MS);
      motorLeft.setSpeed(ME_AVG_SPEED);
      motorRight.setSpeed(-ME_AVG_SPEED);
      vTaskDelay(1500 / portTICK_PERIOD_MS);
      motorLeft.setSpeed(ME_AVG_SPEED);
      motorRight.setSpeed(ME_AVG_SPEED);
      while(radar.getDistance() > OD_RADAR_TOGGLE_VALUE) vTaskDelay(100 / portTICK_PERIOD_MS);
      motorLeft.setSpeed(-ME_AVG_SPEED);
      motorRight.setSpeed(ME_AVG_SPEED);
      vTaskDelay(1500 / portTICK_PERIOD_MS);
      motorLeft.setSpeed(ME_AVG_SPEED);
      motorRight.setSpeed(ME_AVG_SPEED);
      while (status == LD_STATUS_WHITE) vTaskDelay(100 / portTICK_PERIOD_MS);
      blackCount++;
      } else {
        motorLeft.setSpeed(ME_AVG_SPEED);
        motorRight.setSpeed(ME_AVG_SPEED);
        vTaskDelay(400 / portTICK_PERIOD_MS);
        blackCount++;
      }
    }
    vTaskDelay(ME_TASK_TIME / portTICK_PERIOD_MS);
  }
}

void motorsInit() {
  motorLeft.init();
  motorRight.init();
  radar.init(OD_RADAR_TRIG_PIN, OD_RADAR_ECHO_PIN);
  servo.attach(OD_SERVO_PIN);
  servo.write(90);
  delay(1000);
}
