/*
*  Author: Henri Honkanen 
*/ 

#include <avr/io.h>

//TODO: Move map to a utility library.
long map(long v, long a, long b, long x,long y) {
  if(v<=a) {
		return x;
	}
	if(v>=b) {
		return y;
	}
	
	long l_ab = (b-a);	
	long l_xy = (y-x);
	
	long t = v-a;

	float r = (float)t / (float)l_ab;
	
	return x + (l_xy*r);
}

/* SERVO */
/*
	Assumes 16Mhz, ATmega2560 board and Servo output pin to be Timer1.A HW output (pin PB5)

	The servo class uses the timer 1 as a direct HW PWM output timer for the servo.
	This means the timer 1 is used to generate a desired size pulse at certain intervals without CPU usage.
	
	We only want 1 instance of this class to exist, thus a instance existance check is built in to the class. 
	( We use a dedicated HW timer, usage of the same HW timer trough multiple instances is risky. )
	Use a pointers and references if you need to pass the servo around in external code. 
	
	Currently does not support re-targetting during movement.
	
	Interface:
		Servo()
			Constructor
			Also guards against multiple instances..
			
		~Servo()
			Destructor
			Stops movement as a safety feature. (Destruction of object without stopping movement)
		
		bool StartMoving()
			Starts emitting a servo control pulse at 20ms intervals using timer 1 in fast PWM mode.
			The length of the pulse needs to be set using SetTarget() before usage.
			Returns true if movement successfully started, false if not.
		
		void StopMoving()
			Stops the timer 1 from emitting the servo control pulses.
		
		bool SetTarget(int TargetValue)
			Maps the input to a pulse length using linear mapping.
			Assumes TargetValue to be a degree between -85 and 85 (Servo motion limits)
			Values outide the range get mapped to the range edges.
			Returns true if the target is successfully set.
		
		bool ErrorIsSet()
			Checks if the Servo is in error state due to incorrect usage..
		
		void ClearError() {
			Clears the error flag, if possible.

*/

/* Macroes */
/* Constants start... */
#define SERVO_TIMER_TICK_NS \
		500
/* Constants end... */

/* Math utility start... */
#define US_TO_SERVO_COUNTER(US) \
		( ( (US) * 1000 ) / SERVO_TIMER_TICK_NS )
/* Math utility end... */

/* Servo HW controls start... */
#define RESET_SERVO_TIMER \
		TCCR1A = 0; \
		TCCR1B = 0; \
		TCNT1  = 0;


#define SET_SERVO_TIMER_TO_FAST_PWM \
		TCCR1B |= (1 << WGM11); \
		TCCR1B |= (1 << WGM12); \
		TCCR1B |= (1 << WGM13);

#define ENABLE_SERVO_PWM_OUTPUT_PIN_A \
		DDRB =0x20; \
		TCCR1A |= (1 << COM1A1); \
		TCCR1A |= (1 << COM1A0);

#define CONFIGURE_SERVO_PULSE(INTERVAL_US, PULSE_US) \
		ICR1 = US_TO_SERVO_COUNTER( INTERVAL_US ); \
		OCR1A = US_TO_SERVO_COUNTER( INTERVAL_US - PULSE_US );

#define	START_SERVO_TIMER_500NS_TICKS \
		TCCR1B |= (1 << CS11);

#define STOP_SERVO_TIMER \
		TCCR1B &= ~( 1 << CS11 | 1 << CS10 | 1 << CS12 ); \
		TCNT1  = 0;

#define WAIT_UNTIL_NO_SERVO_PULSE_IS_BEING_EMITTED \
		while( TCNT1 >= OCR1A) {;}
/* Servo HW controls end... */

class Servo {
public:
	Servo() {
		m_Error = false;
		m_Moving = false;
		m_TargetSet = false;
		m_CriticalError = false;
		
		//Instance guard. We cannot have 2 instances of this class.
		if(m_InstanceExists) {
			m_Error = true;
			m_CriticalError = true;
		}
		else {
			m_InstanceExists = true;	
		}		
	}
	~Servo() {
		StopMoving();
		if(!m_CriticalError) {
			m_InstanceExists = false;
		}			
	}
	
	//Start sending previously set pulses to the servo..
	bool StartMoving() {
		if( m_Error || m_Moving || !m_TargetSet ) {
			m_Error = true;
			return false;
		}
		//TODO Check that interval is within timer max value..
		
		m_Moving = true;
		m_TargetSet = false;
		m_PulseInvervalUS = PULSE_INTERVAL_US;

		RESET_SERVO_TIMER
		
		SET_SERVO_TIMER_TO_FAST_PWM
		
		ENABLE_SERVO_PWM_OUTPUT_PIN_A

		CONFIGURE_SERVO_PULSE(m_PulseInvervalUS, m_PulseLengthUS)
		
		START_SERVO_TIMER_500NS_TICKS
			
		return true;
	}
	
	//Stop sending pulses to the servo..
	void StopMoving() {
		if(!m_Moving || m_Error) {
			return;
		}
		
		WAIT_UNTIL_NO_SERVO_PULSE_IS_BEING_EMITTED

		STOP_SERVO_TIMER
		
		m_Moving = false;
		return;
	}
	
	//Translate the given target into pulse width..
	bool SetTarget(int TargetValue) {
		if( m_Error || m_Moving ) {
			return false;
		}
		
		if(TargetValue<MIN_TARGET_VALUE || TargetValue > MAX_TARGET_VALUE ) {
			m_Error = true;
			return false;
		}
		
		//Map the angle to the pulse size.
		m_PulseLengthUS = map( TargetValue, MIN_TARGET_VALUE, MAX_TARGET_VALUE, MIN_PULSE_US, MAX_PULSE_US );
		m_TargetSet = true;
		return true;
	}
	
	bool ErrorIsSet() {
		return m_Error;
	}
	void ClearError() {
		if(!m_CriticalError) {
			m_Error = false;
		}
	}
		
	//Variables
	bool m_Error,m_CriticalError,m_Moving,m_TargetSet;
	unsigned long m_PulseInvervalUS;
	unsigned long m_PulseLengthUS;
	
	//Constants
	const static long PULSE_INTERVAL_US = 20000;	//US
	const static long MIN_PULSE_US = 750;		//US
	const static long MAX_PULSE_US = 2250;		//US
	const static long MIN_TARGET_VALUE = -85;	//Degrees
	const static long MAX_TARGET_VALUE = 85;	//Degrees
	
	//Instance checker, this class uses dedicated HW timers that cannot be shared.
	//->Multiple class instances cannot be allowed.
	static bool m_InstanceExists;
};
bool Servo::m_InstanceExists = false;
