#include "servo.h"

void InitServo( int startValue ) {
	RESET_TIMER(1);
	SETUP_MODE14_FAST_PWM(1);
	ENABLE_SERVO_PWM_OUTPUT_PIN;
	ENABLE_COMPARE_OUTPUT_MODE(1);
	MoveServo(startValue);
	START_SERVO_TIMER_500NS_TICKS;
}
 
void StopServo() {
	WAIT_UNTIL_NO_PULSE_IS_BEING_EMITTED(1);
	STOP_TIMER(1);
}

void MoveServo(int targetValue) {
	int pulseLengthUS = map( targetValue, MIN_TARGET_VALUE, MAX_TARGET_VALUE, MIN_PULSE_US, MAX_PULSE_US );
	CONFIGURE_SERVO_PULSE(PULSE_INTERVAL_US, pulseLengthUS);
}
