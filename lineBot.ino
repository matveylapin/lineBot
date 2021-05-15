/**
 * Autor: Lapin Matvey (inst: @l_motya)
 * Using:
 *    GyverCore https://github.com/AlexGyver/GyverCore.git
 *    FreeRTOS for arduino
*/

#include "config.h"
#include "Control.h"
#include <Arduino_FreeRTOS.h>

void setup() {

  Serial.begin(9600);
  while (!Serial);

  xTaskCreate(taskControl, "Control", 128, NULL, 1, NULL);

}

void loop() { }
