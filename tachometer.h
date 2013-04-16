#include "util.h"
#include <avr/interrupt.h>
#include <stdbool.h>

#define RESET_TACHOMETER_TIMER\
	TCCR5A = 0;\
	TCCR5B = 0;\

#define SETUP_TACHOMETER_TIMER_INTERRUPT\
	SET_BIT(TIMSK5,OCIE5A);\
	SET_BIT(TCCR5B,WGM52);\
	OCR5A = 1;\

#define SETUP_TACHOMETER_TIMER_EXT_INPUT\
	SET_BIT(PORTL,PL2);\
	SET_BIT(TCCR5B,CS50);\
	SET_BIT(TCCR5B,CS51);\
	SET_BIT(TCCR5B,CS52);\

#define SETUP_PULSE_INTERVAL_TIMER\
	TCCR3B = 0;\
	TCCR3A = 0;\
	SET_BIT(TIMSK3,TOIE3);\
	SET_BIT(TCCR3B,CS30);\
	SET_BIT(TCCR3B,CS31);\

void SetupTachometer(void); 

void SetupTachometerTimer(void);

float GetTachometerTimerInSeconds(void);

