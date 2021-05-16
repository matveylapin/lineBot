#include "FastGPIO.h"

#include <Arduino.h>
#include "config.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int fastAnalogRead(uint8_t pin) {
  pin = ((pin < 8) ? pin : pin - 14);
  ADMUX = (DEFAULT << 6) | (pin & 0x07);
  sbi(ADCSRA, ADSC);
  //bitSet(ADCSRA, ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  return ADC;
}

void fastDigitalWrite(uint8_t pin, bool x) {

  /*
  disabling PWM on pins that support it
  */
  switch (pin) {						
    case 3: bitClear(TCCR2A, COM2B1);
      break;
    case 5: bitClear(TCCR0A, COM0B1);
      break;
    case 6: bitClear(TCCR0A, COM0A1);
      break;
    case 9: bitClear(TCCR1A, COM1A1);
      break;
    case 10: bitClear(TCCR1A, COM1B1);
      break;
    case 11: bitClear(TCCR2A, COM2A1);
      break;
  }

  /*
  writing to registers of new state
  */
  if (pin < 8) {
    bitWrite(PORTD, pin, x);
  } else if (pin < 14) {
    bitWrite(PORTB, (pin - 8), x);
  } else if (pin < 20) {
    bitWrite(PORTC, (pin - 14), x);
  }
}

void fastAnalogWrite(uint8_t pin, uint16_t duty) {
	
	if (!duty) {
		digitalWrite(pin, LOW);
		return;
	}

	switch (pin) {
	case 5:
		bitSet(TCCR0A, COM0B1);
		OCR0B = duty;
		return;
	case 6:
		bitSet(TCCR0A, COM0A1);
		OCR0A = duty;
		return;
	case 10:
		bitSet(TCCR1A, COM1B1);
		OCR1B = duty;
		return;
	case 9:
		bitSet(TCCR1A, COM1A1);
		OCR1A = duty;
		return;
	case 3:
		bitSet(TCCR2A, COM2B1);
		OCR2B = duty;
		return;
	case 11:
		bitSet(TCCR2A, COM2A1);
		OCR2A = duty;
		return;
	}
}