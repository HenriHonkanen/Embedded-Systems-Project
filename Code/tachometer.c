#include <tachometer.h>

extern volatile float tachoPulsesPerSecond;
bool tachoOverflowFlag;


void SetupTachometer(void) {
	RESET_TACHOMETER_TIMER
	SETUP_TACHOMETER_TIMER_INTERRUPT
	SETUP_TACHOMETER_TIMER_EXT_INPUT
	SetupTachometerTimer();
}

void SetupTachometerTimer(void) {
	SETUP_PULSE_INTERVAL_TIMER
}

float GetTachometerTimerInSeconds(void) {
	return (1.0f/(float)F_CPU)*(float)64*(float)TCNT3;
}

ISR(TIMER5_COMPA_vect) {
	TCNT5 = 0;
	if(tachoOverflowFlag) {
		tachoOverflowFlag = false;
	}
	else {
		tachoPulsesPerSecond = 1.0f/GetTachometerTimerInSeconds();
	}
	TCNT3 = 0;
}

ISR(TIMER3_OVF_vect) {
	tachoOverflowFlag = true;
	tachoPulsesPerSecond = 0;
}
