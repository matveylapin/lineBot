
#define DEBUG_INFO
//#define DEBUG_ERR
//#define DEBUG_WRN

// <MOTOR CONFIG>
#define MOTOR_A_DRIVER_IN1_PIN 7  // motor A in1 pin
#define MOTOR_A_DRIVER_IN2_PIN 8  // motor A in2 pin
#define MOTOR_A_DRIVER_PWM_PIN 6  // motor A pwm pin

#define MOTOR_B_DRIVER_IN1_PIN 4  // motor B in1 pin
#define MOTOR_B_DRIVER_IN2_PIN 2  // motor B in2 pin
#define MOTOR_B_DRIVER_PWM_PIN 5  // motor B pwm pin

// </MOTOR CONFIG>

// <LINE RECOGNIZER>
#define LR_PINS \
  { 15, 16, 17, 18 }
#define LR_SENSOR_COUNT 4
#define LR_ADC_MAX 1024
#define LR_COLOR_BORDER 200
#define LR_NOISE_BORDER 50
#define LR_EEPROM_ADDR 0

#define LD_ON_BORDER 900
#define LD_OFF_BORDER 100
#define LD_TASK_FREQ 15
// </LINE RECOGNIZER>

// <PID>
#define PID_ERROR_ARRAY_SIZE 10
#define PID_EEPROM_ADDR 20
#define PID_DEFAULT_P 1.0
#define PID_DEFAULT_I 0.5
#define PID_DEFAULT_D 0.5
// </PID>

// <MOTION ESTIMATOR>
#define ME_TASK_TIME 20
#define ME_AVG_SPEED 50
#define ME_STOP_LINE_TRAVEL_TIME 1000
// </MOTION ESTIMATOR>

// <CONTROL>
#define CONTROL_BTN_PIN A0
#define CONTROL_LED_R_PIN 11
#define CONTROL_LED_G_PIN 10
#define CONTROL_LED_B_PIN 9
#define CONTROL_CALIBRATION_DELAY 500
// </CONTROL>

// logs and over macros
#ifdef DEBUG_INFO
  #define MSG_INFO(s) Serial.print(F("[ INFO ][")); \
                      Serial.print(F(__TASK__)); \
                      Serial.print(F("]: ")); \
                      Serial.println(s); \
                      while ((UCSR0A & _BV (TXC0)) == 0)
#else
  #define MSG_INFO(s)
#endif

#ifdef DEBUG_ERR
  #define MSG_ERR(s)  Serial.print(F("[ ERROR ][")); \
                      Serial.print(F(__TASK__)); \
                      Serial.print(F("]: ")); \
                      Serial.println(s); \
                      while ((UCSR0A & _BV (TXC0)) == 0)
#else
  #define MSG_ERR(s)
#endif

#ifdef DEBUG_WRN
  #define MSG_WRN(s)  Serial.print(F("[ WARNING ][")); \
                      Serial.print(F(__TASK__)); \
                      Serial.print(F("]: ")); \
                      Serial.println(s); \
                      while ((UCSR0A & _BV (TXC0)) == 0)
#else
  #define MSG_WRN(s)
#endif
