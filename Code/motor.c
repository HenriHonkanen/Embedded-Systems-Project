#include "motor.h"

void InitMotor(void) {
	RESET_TIMER(4);
	SETUP_MOTOR_IO();
	SETUP_MOTOR_CLOCKWISE();
	SETUP_MODE14_FAST_PWM(4);
	ENABLE_COMPARE_OUTPUT_MODE(4);
	SetMotorSpeed(0);
	SETUP_TOP_VALUE(4, MOTOR_DUTY_CYCLE);

	START_MOTOR_TIMER;
}

void SetMotorSpeed(int percentage) {
	int value = map( percentage, MIN_RELATIVE_SPEED, MAX_RELATIVE_SPEED, MIN_POWER, MAX_POWER );
   	SET_MOTOR_SPEED(value);
}
