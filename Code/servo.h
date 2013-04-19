#ifndef SERVO_H
#define SERVO_H
 
#include <avr/io.h>

#include "util.h"

/* Macroes */
/* Constants start... */
#define SERVO_TIMER_TICK_NS \
		500

/* Math utility start... */
#define US_TO_SERVO_COUNTER(US) \
		( ( (US) ) * ( (float)1000 / SERVO_TIMER_TICK_NS) )
/* Math utility end... */

/* Servo HW controls start... */

#define ENABLE_SERVO_PWM_OUTPUT_PIN \
		DDRB |= (1 << PB5)

#define CONFIGURE_SERVO_PULSE(INTERVAL_US, PULSE_US) \
		ICR1 = US_TO_SERVO_COUNTER( INTERVAL_US ); \
		OCR1A = US_TO_SERVO_COUNTER( PULSE_US )

#define	START_SERVO_TIMER_500NS_TICKS \
		TCCR1B |= (1 << CS11)

/* Servo HW controls end... */

const static long PULSE_INTERVAL_US 	= 4000;		//US
const static long MIN_PULSE_US 		= 1000;		//US
const static long MAX_PULSE_US 		= 2000;		//US
const static long MIN_TARGET_VALUE 	= -60;		//Degrees
const static long MAX_TARGET_VALUE 	= 60;		//Degrees


void InitServo( int startValue );
void StopServo( void );
void MoveServo( int targetValue );

#endif

