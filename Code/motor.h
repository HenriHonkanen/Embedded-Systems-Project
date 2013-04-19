#ifndef MOTOR_H
#define MOTOR_H

#include <avr/io.h>
#include "util.h"

#define SETUP_MOTOR_IO()\
    SET_BIT(DDRK, PK0);\
    SET_BIT(DDRK, PK1);\
    SET_BIT(DDRH, PH3)\

#define SETUP_MOTOR_CLOCKWISE() (SET_BIT(PORTK, PK0))

#define START_MOTOR_TIMER\
	SET_BIT(TCCR4B, CS40)

#define SET_MOTOR_SPEED(value)\
	OCR4A = value;

#define MOTOR_DUTY_CYCLE 800

const static long MIN_POWER 		= 0;		
const static long MAX_POWER 		= MOTOR_DUTY_CYCLE/3;		
const static long MIN_RELATIVE_SPEED 	= 0;	
const static long MAX_RELATIVE_SPEED  	= 100;

void InitMotor(void);
void SetMotorSpeed(int percentage);

#endif
