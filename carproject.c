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

#define SMALL 10
#define MEDIUM 20
#define LARGE 40



ISR(INT5_vect) {
	SetMotorSpeed(40);
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

void UpdateSensors() {

	if (PINA < 0xff) {

			if (!(PINA & _BV(PA7))) {
				sensors[0]+=1.0f;
			}
			else {
				sensors[0]-=0.1f;
			}

			if (!(PINA & _BV(PA6))) { 
				sensors[1]+=1.0f;
			}
			else {
				sensors[1]-=0.1f;
			}

			if (!(PINA & _BV(PA5))) { 
				sensors[2]+=0.5f;
			}
			else {
				sensors[2]-=0.1f;
			}

			if (!(PINA & _BV(PA4))) {
				sensors[3]+=1.0f;
			}
			else {
				sensors[3]-=0.1f;
			}

			if (!(PINA & _BV(PA3))) { 
				sensors[4]+=1.0f;
			}
			else {
				sensors[4]-=0.1f;
			}

			if (!(PINA & _BV(PA2))) {
				sensors[5]+=1.0f;
			}
			else {
				sensors[5]-=0.1f;
			}

			if (!(PINA & _BV(PA1))) { 
				sensors[6]+=1.0f;
			}
			else {
				sensors[6]-=0.1f;
			}

			if (!(PINA & _BV(PA0))) { 
				sensors[7]+=1.0f;
			}
			else {
				sensors[7]-=0.1f;
			}

		}

		int i = 0;
		for(i; i<8; i++) {
			if(sensors[i] > 1.0f) {
				sensors[i] = 1.0f;
			}
			else if(sensors[i] < 0.0f) {
				sensors[i] = 0.0f;
			}
		}


}

bool GetFilteredSensorValue(int sensor) {
	return (sensors[sensor]>0.2f);
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

	float direction = 0;
	int target = 0;
	
	int counter = 0;

	for (;;)
	{	
		UpdateSensors();

		int sensors_hit = 0;
		//float shift = 0;

		if (PINA < 0xff) {

			counter=0;

			if (GetFilteredSensorValue(0)) {
				//shift += ShiftTowards(40,target,10);
				target = LARGE;
				sensors_hit++;
				//MoveServo(40);
			}

			if (GetFilteredSensorValue(1)) { 
				//shift += ShiftTowards(30,target,10);
				target = MEDIUM;
				sensors_hit++;
				//MoveServo(30);
			}

			if (GetFilteredSensorValue(2)) { 
				//shift += ShiftTowards(20,target,10);
				target = SMALL;
				sensors_hit++;
				//MoveServo(20);
			}

			if (GetFilteredSensorValue(3)) {
				//shift += ShiftTowards(0,target,10);
				target = 0;
				sensors_hit++;
				//MoveServo(0);
			}

			if (GetFilteredSensorValue(4)) { 
				//shift += ShiftTowards(0,target,10);
				target = 0;
				sensors_hit++;
				//MoveServo(0);
			}
			if (GetFilteredSensorValue(5)) {
				//shift += ShiftTowards(-20,target,10);
				target = -SMALL;
				sensors_hit++;
				//MoveServo(-20);
			}

			if (GetFilteredSensorValue(6)) { 
				//shift += ShiftTowards(-30,target,10);
				target = -MEDIUM;
				sensors_hit++;
				//MoveServo(-30);
			}

			if (GetFilteredSensorValue(7)) { 
				//shift += ShiftTowards(-40,target,10);
				target = -LARGE;
				sensors_hit++;
				//MoveServo(-40);
			}

			//target = shift/sensors_hit;

			if(sensors_hit==1) {
				MoveServo(target);

				switch(target) {
					case 0: SetMotorSpeed(70); break; 
					case -SMALL:
					case SMALL: SetMotorSpeed(50); break; 
					case -MEDIUM:					
					case MEDIUM: SetMotorSpeed(40); break;
					case -LARGE: 
					case LARGE: SetMotorSpeed(30); break; 
				}
			}
		}
		else {
			counter++;

			if(counter>3000) {
				SetMotorSpeed(0);
			}
		}
		//direction = ShiftTowards(target,direction,0.01);	

	}
return 0;
}
