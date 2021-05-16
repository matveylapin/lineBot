#include "Control.h"

#include "config.h"
#include "FastGPIO.h"
#include "GyverButton.h"
#include "LineRecognizer.h"
#include "MotionEstimator.h"
#include <Arduino_FreeRTOS.h>

#define __TASK__ " Control "

void taskControl(void *pvParameters) {
  (void)pvParameters;

  MSG_INFO("Start");

  TaskHandle_t xLineDetectHandle = NULL;
  TaskHandle_t xMotionEstimatorHandle = NULL;

  controlMode_t mode = CONTROL_MODE_DEFAULT;
  pinMode(CONTROL_LED_R_PIN, OUTPUT);
  pinMode(CONTROL_LED_G_PIN, OUTPUT);
  pinMode(CONTROL_LED_B_PIN, OUTPUT);

  GButton btn(CONTROL_BTN_PIN);
  btn.setDirection(NORM_OPEN);
  //lineDetectInit();

  MSG_INFO("Is init");
  MSG_INFO(F_CPU);
  MSG_INFO(portTICK_PERIOD_MS);

  for(;;) {
    btn.tick();

    bool isSingle = btn.isSingle();
    bool isDouble = btn.isDouble();

    if (isDouble && mode == CONTROL_MODE_DEFAULT) {
      MSG_INFO("Line calibrate start");

      mode = CONTROL_MODE_CALIBRATION;
      //fastDigitalWrite(CONTROL_LED_R_PIN, 1);
      vTaskDelay(CONTROL_CALIBRATION_DELAY / portTICK_PERIOD_MS);
      lineDetectRunCalibrate(0, 0);
      //fastDigitalWrite(CONTROL_LED_G_PIN, 1);
      vTaskDelay(CONTROL_CALIBRATION_DELAY / portTICK_PERIOD_MS);
      lineDetectRunCalibrate(1, 0);
      //fastDigitalWrite(CONTROL_LED_R_PIN, 0);
      //fastDigitalWrite(CONTROL_LED_G_PIN, 0);
      mode = CONTROL_MODE_DEFAULT;

      MSG_INFO("Line calibrate finish");
    } else if (btn.isHolded() && mode == CONTROL_MODE_DEFAULT) {
      MSG_INFO("Activate attention mode");

      mode = CONTROL_MODE_ATTENTION;

      lineDetectInit();
      motorsInit();

      xLineDetectHandle = NULL;
      xMotionEstimatorHandle = NULL;
    } else if(isSingle && mode == CONTROL_MODE_ATTENTION) {
      MSG_INFO("RUN");

      mode = CONTROL_MODE_RUN;
      xTaskCreate(taskLineDetect, "LineDetect", 128, NULL, 2, &xLineDetectHandle);
      xTaskCreate(taskMotionEstimator, "MotionEstimator", 128, NULL, 2, &xMotionEstimatorHandle);
    } else if (isSingle && mode == CONTROL_MODE_RUN) {

      MSG_INFO("Run mode disable");

      mode = CONTROL_MODE_DEFAULT;
      vTaskDelete(xMotionEstimatorHandle);
      vTaskDelete(xLineDetectHandle);
      if (xMotionEstimatorHandle != NULL ||xLineDetectHandle != NULL) MSG_ERR("Tasks not delete");
    }
  }
}