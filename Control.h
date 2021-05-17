#pragma once

typedef enum {
  CONTROL_MODE_DEFAULT = 0,
  CONTROL_MODE_CALIBRATION = 1,
  CONTROL_MODE_ATTENTION = 2,
  CONTROL_MODE_RUN = 3,
  CONTROL_MODE_PID_EDIT = 4 // if not for pk in physics, there could be a PID setting via the console
} controlMode_t;

void taskControl(void *pvParameters);