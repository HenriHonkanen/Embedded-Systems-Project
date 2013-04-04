#ifndef LCD_H
#define LCD_H

#include <avr/io.h>

#include "util.h"

#ifndef F_CPU
	#define F_CPU 16000000// Clock Speed
#endif

#define M_BAUD 9600ul
#define USART_UBBR ((F_CPU/(M_BAUD<<4))-1)

void USART_init(void);
void USART_Transmit( unsigned int data );
unsigned int USART_Receive( void );
void ClearScreen(void);
void WriteText( char* txt, int row);
void WriteText_StartingFrom( char* txt, int row, int start);
void SetCharacter( char c, int x , int y);
void InitLCD(void);

#endif
