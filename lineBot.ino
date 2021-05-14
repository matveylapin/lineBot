/**
 * Autor: Lapin Matvey (inst: @l_motya)
 * Using:
 *    GyverCore https://github.com/AlexGyver/GyverCore.git
 *    FreeRTOS for arduino
*/

#include <Arduino_FreeRTOS.h>

#include "MotorDriver.h"
#include "LineRecognizer.h"

void setup() {
  xTaskCreate(taskLineDetect, "LineDetect", 128, NULL, 2, NULL);

  //xTaskCreate(taskMotionEstimator, "MotionEstimator", 128, NULL, 2, NULL);
}

void loop() {

}
