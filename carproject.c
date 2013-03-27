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




ISR(INT5_vect) {
	SetMotorSpeed(50);
}

int main(void)
{  
	sei();
	SETUP_SENSOR_IO();

	CLEAR_BIT(DDRE, PE5);//Input pin 
	ENABLE_EXTERNAL_INTERRUPT(INT5);//External Interrupt Request 5 INT5 enabled*/

	InitServo(0);
	InitMotor();

	for (;;)
	{
		if (PINA < 0xff) {
			ENABLE_COMPARE_OUTPUT_MODE(4);
			//on = true;
			//if (!(PINA & _BV(PA0))) {
			if (!(PINA & _BV(PA7))) {
				MoveServo(40);
			}

			else if (!(PINA & _BV(PA6))) { 
				MoveServo(30);
			}

			else if (!(PINA & _BV(PA5))) { 
				MoveServo(20);
			}

			else if (!(PINA & _BV(PA4))) {
				MoveServo(0);
			}

			else if (!(PINA & _BV(PA3))) { 
				MoveServo(0);
			}
			else if (!(PINA & _BV(PA2))) {
				MoveServo(-20);
			}

			else if (!(PINA & _BV(PA1))) { 
				MoveServo(-30);
			}
			else if (!(PINA & _BV(PA0))) { 
				MoveServo(-40);
			}
		}

	}
return 0;
}
