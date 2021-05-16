/**
 * Autor: Lapin Matvey (inst: @l_motya)
 * Using:
 *    FreeRTOS for arduino https://github.com/feilipu/Arduino_FreeRTOS_Library
*/

#include "config.h"
#include "Control.h"
#include <Arduino_FreeRTOS.h>
#include "LineRecognizer.h"
#include "FastGPIO.h"

void setup() {
  Serial.begin(9600);
  lineDetectInit();
  for (uint8_t i = 0; i < 10; i++) {
    Serial.println(fastAnalogRead(A1));
    delay(10);
  }
  //lineDetectRunCalibrate(0, 0);
  Serial.println("gay 2");
  xTaskCreate(taskControl, "Control", 256, NULL, 2, NULL);
}

void loop() { }

void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName) {
  Serial.println(pcTaskName);
}
