#include "servo.h"

void InitServo( int startValue ) {

	RESET_SERVO_TIMER
	SET_SERVO_TIMER_TO_FAST_PWM
	ENABLE_SERVO_PWM_OUTPUT_PIN_A
	
	MoveServo(startValue);
	
	START_SERVO_TIMER_500NS_TICKS

}
 
void StopServo() {

	WAIT_UNTIL_NO_SERVO_PULSE_IS_BEING_EMITTED
	STOP_SERVO_TIMER
	
}

void MoveServo(int targetValue) {

	int pulseLengthUS = map( targetValue, MIN_TARGET_VALUE, MAX_TARGET_VALUE, MIN_PULSE_US, MAX_PULSE_US );
	CONFIGURE_SERVO_PULSE(pulseLengthUS, PULSE_INTERVAL_US)
	
}