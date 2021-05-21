#include "MotionEstimator.h"

#include "config.h"
#include "PIDlib.h"
#include "FastGPIO.h"
#include "MotorDriver.h"
#include "LineRecognizer.h"
#include "ObstacleDetector.h"
#include <Arduino_FreeRTOS.h>

#define __TASK__ " MotionEstimator "

MotorDriver motorLeft(MOTOR_A_DRIVER_IN1_PIN, MOTOR_A_DRIVER_IN2_PIN, MOTOR_A_DRIVER_PWM_PIN, -1);
MotorDriver motorRight(MOTOR_B_DRIVER_IN1_PIN, MOTOR_B_DRIVER_IN2_PIN, MOTOR_B_DRIVER_PWM_PIN, -1);

PID pid(PID_DEFAULT_P, PID_DEFAULT_D, PID_DEFAULT_D);

void taskMotionEstimator(void *pvParameters) {
  (void)pvParameters;

  MSG_INFO("Start");

  lineStatus_t status;
  int8_t error;

  getLineData(&error, &status);

  for (;;) {
    getLineData(&error, &status);
    if (status == LD_STATUS_BLACK_LINE || status == LD_STATUS_WHITE_LINE) {   // if the line is white or black, start a regular ride along the line
      float correction = pid.calculate(error);

      motorLeft.setSpeed(ME_AVG_SPEED * (1 + correction));
      motorRight.setSpeed(ME_AVG_SPEED * (1 - correction));

      MSG_INFO(error);
      //MSG_INFO(ME_AVG_SPEED * (1 + correction));
      //MSG_INFO(ME_AVG_SPEED * (1 - correction));

    } else if (status == LD_STATUS_BLACK) {   //if we meet a black stop line, we just go straight until we meet again

      motorLeft.setSpeed(ME_AVG_SPEED);
      motorRight.setSpeed(ME_AVG_SPEED);

      vTaskDelay(ME_STOP_LINE_TRAVEL_TIME / portTICK_PERIOD_MS);
      while (status != LD_STATUS_BLACK) vTaskDelay(200 / portTICK_PERIOD_MS);;
      vTaskDelay(ME_STOP_LINE_TRAVEL_TIME / portTICK_PERIOD_MS);
    }
    vTaskDelay(ME_TASK_TIME / portTICK_PERIOD_MS);
  }
}

void motorsInit() {
  motorLeft.init();
  motorRight.init();
}
