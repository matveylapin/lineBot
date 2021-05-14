/**
 * Autor: Lapin Matvey (inst: @l_motya)
 * Using:
 *    GyverCore https://github.com/AlexGyver/GyverCore.git
 *    FreeRTOS for arduino
*/

#include "Control.h"
#include "MotorDriver.h"
#include "LineRecognizer.h"
#include "MotionEstimator.h"
#include <Arduino_FreeRTOS.h>

void setup() {

  //xTaskCreate(taskLineDetect, "LineDetect", 128, NULL, 2, NULL);
  //xTaskCreate(taskMotionEstimator, "MotionEstimator", 128, NULL, 2, NULL);

  xTaskCreate(taskControl, "Control", 128, NULL, 1, NULL);

}

void loop() {
}
