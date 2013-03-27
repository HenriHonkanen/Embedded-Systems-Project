#ifndef SERVO_H
#define SERVO_H
 
#include <avr/io.h>

#include "util.h"

/* Macroes */
/* Constants start... */
#define SERVO_TIMER_TICK_NS \
		500
/* Constants end... */

/* Math utility start... */
#define US_TO_SERVO_COUNTER(US) \
		( ( (US) * 1000 ) / SERVO_TIMER_TICK_NS )
/* Math utility end... */

/* Servo HW controls start... */
#define RESET_SERVO_TIMER \
		TCCR1A = 0; \
		TCCR1B = 0; \
		TCNT1  = 0;


#define SET_SERVO_TIMER_TO_FAST_PWM \
		TCCR1B |= (1 << WGM11); \
		TCCR1B |= (1 << WGM12); \
		TCCR1B |= (1 << WGM13);

#define ENABLE_SERVO_PWM_OUTPUT_PIN_A \
		DDRB =0x20; \
		TCCR1A |= (1 << COM1A1); \
		TCCR1A |= (1 << COM1A0);

#define CONFIGURE_SERVO_PULSE(INTERVAL_US, PULSE_US) \
		ICR1 = US_TO_SERVO_COUNTER( INTERVAL_US ); \
		OCR1A = US_TO_SERVO_COUNTER( INTERVAL_US - PULSE_US );

#define	START_SERVO_TIMER_500NS_TICKS \
		TCCR1B |= (1 << CS11);

#define STOP_SERVO_TIMER \
		TCCR1B &= ~( 1 << CS11 | 1 << CS10 | 1 << CS12 ); \
		TCNT1  = 0;

#define WAIT_UNTIL_NO_SERVO_PULSE_IS_BEING_EMITTED \
		while( TCNT1 >= OCR1A) {;}
/* Servo HW controls end... */

const static long PULSE_INTERVAL_US = 20000;	//US
const static long MIN_PULSE_US 		= 750;		//US
const static long MAX_PULSE_US 		= 2250;		//US
const static long MIN_TARGET_VALUE 	= -85;		//Degrees
const static long MAX_TARGET_VALUE 	= 85;		//Degrees

void InitServo( int startValue );
void StopServo();
void MoveServo( int targetValue );

#endif SERVO_H