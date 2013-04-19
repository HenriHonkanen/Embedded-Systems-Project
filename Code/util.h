#ifndef UTIL_H
#define UTIL_H
 
#include <avr/io.h>

#define INPUT 0x00
#define OUTPUT 0xff 

#define SET_BIT(reg, bit) ((reg) |= _BV(bit))
#define CLEAR_BIT(reg, bit) ((reg) &= ~_BV(bit))

#define ENABLE_COMPARE_OUTPUT_MODE(timer_number)(SET_BIT(TCCR##timer_number##A, COM##timer_number##A1))

#define SETUP_MODE14_FAST_PWM(timer_number)\
    CLEAR_BIT(TCCR##timer_number##A, WGM##timer_number##0);\
    SET_BIT(TCCR##timer_number##A, WGM##timer_number##1);\
    SET_BIT(TCCR##timer_number##B, WGM##timer_number##2);\
    SET_BIT(TCCR##timer_number##B, WGM##timer_number##3)\

#define SETUP_TOP_VALUE(timer_number, val) (ICR##timer_number = (val))

#define RESET_TIMER(timer_number) \
		TCCR##timer_number##A = 0; \
		TCCR##timer_number##B = 0; \
		TCNT##timer_number = 0;

#define STOP_TIMER(timer_number) \
		TCCR##timer_number##B &= ~( 1 << CS11 | 1 << CS10 | 1 << CS12 ); \
		TCNT##timer_number = 0;

#define WAIT_UNTIL_NO_PULSE_IS_BEING_EMITTED(timer_number) \
		while( TCNT##timer_number >= OCR##timer_number##A) {;}

#define F_CPU 16000000

long map(long v, long a, long b, long x,long y);

#endif

