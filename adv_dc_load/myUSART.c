#include "myUSART.h"

char USARTbuffer[STRING_USARTbuffer_SIZE];

void USART_transmitFrame(uint8_t frame)
{
	/* Wait for empty transmit USARTbuffer */
	while ( !( UCSRA & (1<<UDRE)) )	;
	
	/* Put data into USARTbuffer, sends the data */
	UDR = frame;
	
	/* Wait for Transmission to end */
	loop_until_bit_is_set(UCSRA, TXC);
	UCSRA |= 1 << TXC;
}

// sends a string over the UART
void USART_transmitString(const char * str)
{
	//might need (const char *str) as argument
		while(*str!='\0') {USART_transmitFrame(*str);	str++;}	
}

void USART_init(uint8_t modeSynch, uint8_t frSize, uint8_t sParity, uint8_t stBits)
{	
	// Double speed disable
	UCSRA &= ~(1 << U2X);
	
	uint8_t tmpUCSRC = 0x00;
	
	// set Synchronous or Asynchronous link
	if (modeSynch)	tmpUCSRC |= 1 << UMSEL;
	else			tmpUCSRC &= ~((1 << UMSEL)|(1 << UCPOL));
	
	// set baud rate
	//UCSRC &= ~(1 << URSEL); // access UBRRH instead of UCSRC
	UBRRH = (BAUDRATE>>8);  // shift the register right by 8 bits to keep only the 2 MSBits that exceed 8 bit size
	UBRRL = BAUDRATE;
	
	// Dataframe setup
	UCSRB &= ~(1 << UCSZ2);
	if (frSize >> 4) // if framesize = 9
	{
		frSize = 8;
		UCSRB |= 1 << UCSZ2;
	}
	frSize     = (frSize - 5) << UCSZ0; // create the 0b?????xx? pattern
	tmpUCSRC &= ~((1 << UCSZ1)|(1 << UCSZ0));
	tmpUCSRC  |= frSize;

	// parity setup
	if (sParity == 0)	tmpUCSRC &= ~((1 << UPM1)|(1 << UPM0));		// no parity
	else
	{
		UCSRC |= 1 << UPM1;
		if (sParity == 1)	tmpUCSRC &= ~(1 << UPM0);	// odd parity
		else				tmpUCSRC |=  (1 << UPM0);	// even parity
	}
	
	// set stop bits
	stBits--;
	stBits = stBits << USBS;
	tmpUCSRC &= ~(1 << USBS);
	tmpUCSRC |= stBits;
		
	//UCSRC = tmpUCSRC | (1 << URSEL);
}

void USART_TxEnable(uint8_t onoffswitch)
{
	if (onoffswitch) UCSRB |=  (1 << TXEN);
	else			 UCSRB &= ~(1 << TXEN);
}

void USART_RxEnable(uint8_t onoffswitch)
{
	if (onoffswitch) UCSRB |=  (1 << RXEN);
	else			 UCSRB &= ~(1 << RXEN);
}
