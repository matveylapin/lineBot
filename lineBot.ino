/**
 * Author: Lapin Matvey (inst: @l_motya)
 * Using:
 *    FreeRTOS for arduino https://github.com/feilipu/Arduino_FreeRTOS_Library
*/

#include "config.h"
#include "Control.h"
#include <Arduino_FreeRTOS.h>
#include "LineRecognizer.h"
#include "ObstacleDetector.h"

void setup() {
  Serial.begin(9600);
  lineDetectInit();
  //obstacleDetectorInit();
  pinMode(A5, INPUT);
  xTaskCreate(taskControl, "Control", 256, NULL, 1, NULL);
}

void loop() { }

// this thing for configuring rtos is called on stack overflow
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName) {
  Serial.println(pcTaskName);
}
