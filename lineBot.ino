/**
 * Autor: Lapin Matvey (inst: @l_motya)
 * Using:
 *    GyverCore https://github.com/AlexGyver/GyverCore.git
 *    FreeRTOS for arduino
*/
#include <Arduino_FreeRTOS.h>

void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, 1);
  delay(100);
  digitalWrite(13, 0);
  delay(500);
}
