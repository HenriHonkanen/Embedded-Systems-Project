#include "LCD.h"

void USART_init(void) {

	// Set baud rate
	UBRR1H = (uint8_t)(USART_UBBR>>8);
	UBRR1L = (uint8_t)USART_UBBR;

	UCSR1B = 0;
	//Enable recv and send 
	SET_BIT(UCSR1B, RXEN1);	
 	SET_BIT(UCSR1B, TXEN1);

	UCSR1C = 0;
	//Set data size to 8 bit.
	SET_BIT(UCSR1C, UCSZ11);	
 	SET_BIT(UCSR1C, UCSZ10);
}


void USART_Transmit( unsigned int data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1))) 
		;
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

unsigned int USART_Receive( void )
{
	unsigned char res;
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) )
		;

	/* from buffer */
	res = UDR1;
	return res;
}

void ClearScreen(void) {
	USART_Transmit( 'E' );
	USART_Receive();
}

void WriteText( char* txt, int row) {
	WriteText_StartingFrom(txt, row, 0);
}

void WriteText_StartingFrom( char* txt, int row, int start) {
	int i = 0;
	static const int max_row_length = 20;
	for(;txt[i]!='\0' && i+start < max_row_length;i++) {
		SetCharacter(txt[i],i+start,row);
	}
}

void SetCharacter( char c, int x , int y) {
	USART_Transmit( 'T' );
	USART_Transmit( c );
	USART_Transmit( x );
	USART_Transmit( y );
	USART_Transmit( 0xFF );
	USART_Transmit( 0xFF );
	USART_Receive();
}

void InitLCD(void) {
	//LCD INIT
	USART_init();
	USART_Transmit( 0x55 );
	USART_Receive();

	//SET FONT
	USART_Transmit( 0x46 );
	USART_Transmit( 0x03 );
	USART_Receive();
}
