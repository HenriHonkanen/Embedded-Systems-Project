#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "servo.h"
#include "motor.h"
#include "util.h"

#define ENABLE_EXTERNAL_INTERRUPT(bit) SET_BIT(EIMSK, bit)
#define DISABLE_EXTERNAL_INTERRUPT(bit) CLEAR_BIT(EIMSK, bit)

#define SETUP_SENSOR_IO()\
    DDRA = INPUT;\
    DDRD = OUTPUT\

#define CENTER 5
#define SMALL 15
#define MEDIUM 30
#define LARGE 45

bool driveFlag = false;

ISR(INT5_vect) {
	driveFlag = true;
}

float ShiftTowards( float target , float direction , float speed ) {

	if((target-direction) > speed || (target-direction) < -speed ) {

		if(direction < target) {
			return direction+speed;
		}
		else if (direction > target) {
			return direction-speed;
		}
	}
	return target;
}


float sensors[8];
float lastTarget = 0;
float sensorTarget[8] = {LARGE, MEDIUM, SMALL, CENTER, -CENTER, -SMALL, -MEDIUM, -LARGE};
float lapFlag = false;
int lapCounter = 0;

void UpdateSensors(void) {

	if (PINA < 0xff) {

			if (!(PINA & _BV(PA7))) {
				sensors[0]+=1.0f;
			}
			else {
				sensors[0]-=1.0f;
			}

			if (!(PINA & _BV(PA6))) { 
				sensors[1]+=1.0f;
			}
			else {
				sensors[1]-=1.0f;
			}

			if (!(PINA & _BV(PA5))) { 
				sensors[2]+=1.0f;
			}
			else {
				sensors[2]-=1.0f;
			}

			if (!(PINA & _BV(PA4))) {
				sensors[3]+=1.0f;
			}
			else {
				sensors[3]-=1.0f;
			}

			if (!(PINA & _BV(PA3))) { 
				sensors[4]+=1.0f;
			}
			else {
				sensors[4]-=1.0f;
			}

			if (!(PINA & _BV(PA2))) {
				sensors[5]+=1.0f;
			}
			else {
				sensors[5]-=1.0f;
			}

			if (!(PINA & _BV(PA1))) { 
				sensors[6]+=1.0f;
			}
			else {
				sensors[6]-=1.0f;
			}

			if (!(PINA & _BV(PA0))) { 
				sensors[7]+=1.0f;
			}
			else {
				sensors[7]-=1.0f;
			}

		}

		int i = 0;
		for(;i<8; i++) {
			if(sensors[i] > 1.0f) {
				sensors[i] = 1.0f;
			}
			else if(sensors[i] < 0.0f) {
				sensors[i] = 0.0f;
			}
		}


}

float GetFilteredSensorValue(void) {
	UpdateSensors();
	int counter = 0;
	static float previous = 0.0;
	float value= 0.0f;
	int i;
	for (i = 0; i < 8; i++) {
		if (sensors[i] == 1.0)
			 counter++;
	}
	if (counter > 5) {
		if (!lapFlag)
			lapCounter++;
		lapFlag = true;
		previous = 0.0;
		return 0.0;
	}
	lapFlag = false;
	if (sensors[0])
		value = 1.0;
	else if (sensors[7])
		value = -1.0;
	else if (sensors[1])
		value = 0.7;
	else if (sensors[6])
		value = -0.7;
	else if (sensors[2])
		value = 0.35;
	else if (sensors[5])
		value = -0.35;
	else if (sensors[3])
		value = 0.0;
	else if (sensors[4])
		value = -0.0;
	else	
		value = previous; 


	previous = value;
	return value;
}

float absFloat(float value) {
	if (value < 0.0)
		return -value;
	return value;
}

int main(void)
{  
	int i = 0;
	for(; i<8; i++) {
			sensors[i] = 0;
	}

	sei();
	SETUP_SENSOR_IO();

	CLEAR_BIT(DDRE, PE5);//Input pin 
	ENABLE_EXTERNAL_INTERRUPT(INT5);//External Interrupt Request 5 INT5 enabled*/

	InitServo(0);
	InitMotor();

	float currentDirection = 0;
	float targetDirection = 0;

	for (;;)
	{	
		float direction = GetFilteredSensorValue();
		if (lapCounter > 3) {
			SetMotorSpeed(0);
			while(true);
		}
		targetDirection = direction * 45.0;
		if (absFloat(targetDirection) >= absFloat(currentDirection)) {
			currentDirection = targetDirection;				
		}
		else {
			currentDirection += (targetDirection - currentDirection);
		}
		MoveServo(currentDirection);
		if (driveFlag)
			SetMotorSpeed(50 + (1.0 - absFloat(direction)) * 40);
		//direction = ShiftTowards(target,direction,0.01);	

	}
return 0;
}
