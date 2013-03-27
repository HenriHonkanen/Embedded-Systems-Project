/* Note: Led are connected to PORTC and switches to PORTA so that each bit on port represents state of switch or led accordingly. 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h> //uint8_t
#include <stdbool.h>
#define INPUT 0x00
#define OUTPUT 0xff 
volatile int counter = 0;

#define SET_BIT(reg, bit) ((reg) |= _BV(bit))
#define CLEAR_BIT(reg, bit) ((reg) &= ~_BV(bit))
/**
 * arg: it 7:0 - INT7:0: External Interrupt Request 7 - 0 Enable  
 **/
#define ENABLE_EXTERNAL_INTERRUPT(bit)(EIMSK |= _BV(bit))

#define SETUP_SENSOR_IO()\
    DDRA = INPUT;\
    DDRD = OUTPUT\

#define SETUP_MOTOR_IO()\
    SET_BIT(DDRK, PK0);\
    SET_BIT(DDRK, PK1);\
    SET_BIT(DDRH, PH3)\
/**
 * INA = 1, INA = 0
 **/
#define SETUP_MOTOR_CLOCKWISE() (SET_BIT(PORTK, PK0))

#define SETUP_MODE14_FAST_PWM(timer_number)\
    CLEAR_BIT(TCCR##timer_number##A, WGM##timer_number##0);\
    SET_BIT(TCCR##timer_number##A, WGM##timer_number##1);\
    SET_BIT(TCCR##timer_number##B, WGM##timer_number##2);\
    SET_BIT(TCCR##timer_number##B, WGM##timer_number##3)\

#define SETUP_TOP_VALUE(timer_number, val) (ICR##timer_number = (val))

/**
 * Clear OCnA/OCnB/OCnC on compare match, set OCnA/OCnB/OCnC at BOTTOM (non-inverting mode) 
 * Ref: Table 17-4 p/159
 */
#define ENABLE_COMPARE_OUTPUT_MODE(timer_number)(SET_BIT(TCCR##timer_number##A, COM##timer_number##A1))


void set_motor_frequency() { 
    /** 
     * Maximum PWM input frequency of the motor driver is 20 kHz. 
     * Forumula:    F_OCnxPWM = f_clk_IO/N*(1+TOP)
     *              F_OCnxPWM < 20 kHz = 20 * 1000 Hz
     *              f_clk_IO = 16 MHz = 16 * 1000 000 Hz 
     *              => N*(1+TOP) > f_clk_IO/F_OCnxPWN = 1600/2 = 800
     *              =>  Case 1: N = 256, TOP > 2.125 
     *                  Case 2: N = 64, TOP > 11.5
     *                  Case 3: N = 8, TOP > 99
     *                  *Case 4: N = 1, TOP > 799
     *
     *  In non-inverting Compare Output mode, the Output Compare (OCnx) is cleared on the compare match between TCNTn and OCRnx, 
     *  and set at BOTTOM.
     **/
     //Ref: 17.11 p/161
    SETUP_TOP_VALUE(4, 799);
    SET_BIT(TCCR4B, CS40);//No prescaler
    OCR4A = 120;//output compare register
}

ISR(INT5_vect) { 
    CLEAR_BIT(TCCR4A, COM4A1);//Compare Output Mode disabled 
}
/**
 * Maximum PWM input frequency of the given modern digital servo is 250 Hz.
 * Formula:     F_OCnPWM = f_clk_IO/N*(1+TOP)
 *              F_OCnPWM < 250 Hz
 *              f_clk_IO = 16 MHz = 16 * 1000 000 Hz
 *              => N*(1+TOP) > f_clk_IO/F_OCnPWM = 64000
                =>  Case 1: N = 1, 1+TOP > 64000
                =>  *Case 2: N = 8, 1+TOP > 8000
 */
void set_servo_timer()
{
    //set up TOP value
    SETUP_TOP_VALUE(1, 7999); 
    //Ref: Section 17.11 > Table 17-6 (p/161)
    SET_BIT(TCCR1B, CS21); // start the timer with prescaler /8
    //TIMSK1 |= _BV(TOIE1);//Turn on the interrupt handler 

}

ISR(TIMER1_OVF_vect) 
{   
    
    if (counter >= 0)
        counter++;
    if (counter == 200) {
        counter = -1;
        CLEAR_BIT(TCCR1A, COM1A1);
    }
}

int main()
{  
    sei();
    //sensor
    SETUP_SENSOR_IO();
    //The motor won't run while the DDR is output on ENA and/or ENB!
    SETUP_MOTOR_IO();
    SETUP_MOTOR_CLOCKWISE();
    SETUP_MODE14_FAST_PWM(4);
    set_motor_frequency();
    //switch
    CLEAR_BIT(DDRE, PE5);//Input pin 
    ENABLE_EXTERNAL_INTERRUPT(INT5);//External Interrupt Request 5 INT5 enabled

    //Servo
    SET_BIT(DDRB, PB5);
    SETUP_MODE14_FAST_PWM(1); 
    set_servo_timer(); 
    TIMSK1 |= _BV(TOIE1);//Turn on the interrupt handler 
    counter = -1; 
	//bool on = false;
    for (;;)
    {
        if (PINA < 0xff) {
            git p(4);
            //on = true;
            //if (!(PINA & _BV(PA0))) {
            if (counter == -1 && !(PINA & _BV(PA7))) {
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 4000;
            }

            else if (counter == -1 && !(PINA & _BV(PA6))) { 
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 3500;
            }

            else if (counter == -1 && !(PINA & _BV(PA5))) { 
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 3300;
            }

            else if (counter == -1 && !(PINA & _BV(PA4))) { 
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 3100;
            }

            else if (counter == -1 && !(PINA & _BV(PA3))) { 
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 2800;
            }
            else if (counter == -1 && !(PINA & _BV(PA2))) { 
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 2500;
            }

            else if (counter == -1 && !(PINA & _BV(PA1))) { 
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 2200;
            }
            else if (counter == -1 && !(PINA & _BV(PA0))) { 
                ENABLE_COMPARE_OUTPUT_MODE(1);
                counter = 0;
                OCR1A = 1700;
            }

            //}

        }



        /*
        if (on && !(PINE & _BV(PE5))) { //POLLING: PINE is the input from PE0 to PE7
            //CLEAR_BIT(PORTH, PH3);
            CLEAR_BIT(TCCR4A, COM4A1);
            on = false;
        }*/        
    }
    return 0;
}
