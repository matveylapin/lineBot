
// <MOTOR CONFIG>
#define MOTOR_A_DRIVER_IN1_PIN 7  // motor A in1 pin
#define MOTOR_A_DRIVER_IN2_PIN 8  // motor A in2 pin
#define MOTOR_A_DRIVER_PWM_PIN 6  // motor A pwm pin

#define MOTOR_B_DRIVER_IN1_PIN 4  // motor B in1 pin
#define MOTOR_B_DRIVER_IN2_PIN 2  // motor B in2 pin
#define MOTOR_B_DRIVER_PWM_PIN 5  // motor B pwm pin

// </MOTOR CONFIG>

// <LINE RECOGNIZER>
#define LR_PINS         {15, 16, 17, 18}
#define LR_SENSOR_COUNT 4
#define LR_ADC_MAX      1024
#define LR_COLOR_BORDER 200
#define LR_NOISE_BORDER 50
#define LR_EEPROM_ADDR  0

#define LD_ON_BORDER   900
#define LD_OFF_BORDER  100
// </LINE RECOGNIZER>

// <PID>
#define PID_ERROR_ARRAY_SIZE  10
#define PID_EEPROM_ADDR       20
// </PID>