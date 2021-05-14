#include "Control.h"

#include "config.h"
#include "GyverButton.h"
#include "LineRecognizer.h"
#include "MotionEstimator.h"
#include <Arduino_FreeRTOS.h>

void taskControl(void *pvParameters) {
  (void)pvParameters;
  LOG("connection test 2");
  TaskHandle_t xLineDetectHandle = NULL;
  TaskHandle_t xMotionEstimatorHandle = NULL;

  controlMode_t mode = CONTROL_MODE_DEFAULT;
  pinMode(CONTROL_LED_R_PIN, OUTPUT);
  pinMode(CONTROL_LED_G_PIN, OUTPUT);
  pinMode(CONTROL_LED_B_PIN, OUTPUT);

  GButton btn(CONTROL_BTN_PIN);
  btn.setDirection(NORM_CLOSE);

  LOG("connection test");

  for(;;) {
    btn.tick();
    if (btn.isDouble()) {
      mode = CONTROL_MODE_CALIBRATION;
      digitalWrite(CONTROL_LED_R_PIN, 1);
      vTaskDelay(CONTROL_CALIBRATION_DELAY / portTICK_PERIOD_MS);
      lineDetectRunCalibrate(0, 0);
      digitalWrite(CONTROL_LED_G_PIN, 1);
      vTaskDelay(CONTROL_CALIBRATION_DELAY / portTICK_PERIOD_MS);
      lineDetectRunCalibrate(1, 1);
      digitalWrite(CONTROL_LED_R_PIN, 0);
      digitalWrite(CONTROL_LED_G_PIN, 0);
      mode = CONTROL_MODE_DEFAULT;

    } else if (btn.isHolded()) {
      mode = CONTROL_MODE_ATTENTION;

      lineDetectInit();
      motorsInit();

      xLineDetectHandle = NULL;
      xMotionEstimatorHandle = NULL;

    } else if(btn.isSingle() && mode == CONTROL_MODE_ATTENTION) {
      mode = CONTROL_MODE_RUN;
      xTaskCreate(taskLineDetect, "LineDetect", 128, NULL, 2, &xLineDetectHandle);
      xTaskCreate(taskMotionEstimator, "MotionEstimator", 128, NULL, 2, &xMotionEstimatorHandle);
    } else if (btn.isSingle() && mode == CONTROL_MODE_RUN) {
      mode = CONTROL_MODE_DEFAULT;
      vTaskDelete(xMotionEstimatorHandle);
      vTaskDelete(xLineDetectHandle);
    }
  }
}