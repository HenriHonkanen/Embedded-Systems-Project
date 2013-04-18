#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include <stdbool.h>

#include <stdlib.h> 
#include <stdio.h> 

#include "servo.h"
#include "motor.h"
#include "tachometer.h"
#include "LCD.h"
#include "util.h"


#define ENABLE_EXTERNAL_INTERRUPT(bit) SET_BIT(EIMSK, bit)
#define DISABLE_EXTERNAL_INTERRUPT(bit) CLEAR_BIT(EIMSK, bit)

#define SETUP_SENSOR_IO()\
    DDRA = INPUT;\
    DDRD = OUTPUT\

#define CENTER 0
#define SMALL 10
#define MEDIUM 35
#define LARGE 45


bool driveFlag = false;

bool updateScreen = true;
char* status = "Waiting";
char lap[4] = "0";
char time[4] = "-1";
char cyc[8] = "-1";

volatile long ticker = 0;
volatile int sec = 0;


volatile int targetSpeed;
volatile float currentSpeed;

volatile float tachoPulsesPerSecond;

float sensors[8];
float lastTarget = 0;
float sensorTarget[8] = {LARGE, MEDIUM, SMALL, CENTER, -CENTER, -SMALL, -MEDIUM, -LARGE};
float lapFlag = false;
int lapCounter = 0;

ISR(INT5_vect) {
	if(!driveFlag) {	
		status = "Driving";
		updateScreen = true;
		driveFlag = true;
		sprintf(time,"%d", sec);
		sprintf(cyc,"%d", 0);
	}
}

void UpdateSensors(void) {
	sensors[0]= ! (PINA & _BV(PA7));
	sensors[1]= ! (PINA & _BV(PA6));
	sensors[2]= ! (PINA & _BV(PA5));
	sensors[3]= ! (PINA & _BV(PA4));
	sensors[4]= ! (PINA & _BV(PA3));
	sensors[5]= ! (PINA & _BV(PA2));
	sensors[6]= ! (PINA & _BV(PA1));
	sensors[7]= ! (PINA & _BV(PA0));
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
	if (counter > 3) {
		if (!lapFlag) {
			lapCounter++;
			sprintf(lap,"%d",lapCounter);
			//updateScreen = true;
		}
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

	//Needs to be before sei.. don't ask why..
	//Also needs a delay from start up..
	_delay_ms(1000);
	InitLCD();

	sei();
	SETUP_SENSOR_IO();

	CLEAR_BIT(DDRE, PE5);//Input pin 
	
	ENABLE_EXTERNAL_INTERRUPT(INT5);//External Interrupt Request 5 INT5 enabled*/

	InitServo(0);
	InitMotor();	
	SetupTachometer();
	float currentDirection = 0;
	float targetDirection = 0;

	bool terminated = false;
	currentSpeed = 10;
	tachoPulsesPerSecond = 0;
	for (;;)
	{	

		if(updateScreen) {
			updateScreen = false;
			ClearScreen();
			WriteText("State:",1);
			//cli();	//Incase we are writing when button is pressed.
				WriteText_StartingFrom(status,2,7);
			//sei();
			WriteText("Lap:",3);
			WriteText_StartingFrom(lap,4,7);

			WriteText("Ticker:",5);
			WriteText_StartingFrom(time,6,7);

			WriteText("cycle:",7);
			WriteText_StartingFrom(cyc,8,7);
			
		}

		if(!terminated) {		

			float direction = GetFilteredSensorValue();

			if (lapCounter > 3) {			
				SetMotorSpeed(0);
				MoveServo(0);

				terminated = false;
				status = "Terminated";
				updateScreen = true;
				driveFlag = false;
				lapCounter = 0;
				continue;

			}
		
			targetDirection = direction * 45.0;

			if (absFloat(targetDirection) >= absFloat(currentDirection)) {
				currentDirection = targetDirection;				
			}
			else {
				currentDirection += (targetDirection - currentDirection);
			}

			MoveServo(currentDirection);
			
			if (driveFlag) {
				float desiredPulsePerSecond = 9 + (1.0 - absFloat(direction)) * 5;
				float difference = desiredPulsePerSecond - tachoPulsesPerSecond;
				float coefficient = 0.015;
				if (difference < 0) {
					coefficient = -0.015;
				}
				currentSpeed += 20 * coefficient;

				if(currentSpeed>80) {
					currentSpeed = 80;
				}
				if(currentSpeed<0) {
					currentSpeed = 0;
				}

				SetMotorSpeed(currentSpeed);	
			}
		}
	}
	return 0;
}
