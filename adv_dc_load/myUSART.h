#ifndef MYUSART_H
#define MYUSART_H

#include <avr/io.h>
#include <stdlib.h>

// port this library to ATmega644

#define UDR		UDR0

#define UCSRA	UCSR0A
#define TXC		TXC0
#define U2X		U2X0
#define UDRE	UDRE0

#define UCSRB	UCSR0B
#define RXEN	RXEN0
#define TXEN	TXEN0
#define UCSZ2	UCSZ02

#define UCSRC	UCSR0C
#define UMSEL	UMSEL00
#define UMSELx	UMSEL01
#define UPM1	UPM01
#define UPM0	UPM00
#define USBS	USBS0
#define UCSZ1	UCSZ01
#define UCSZ0	UCSZ00
#define UCPOL	UCPOL0


#define UBRRH	UBRR0H
#define UBRRL	UBRR0L

//#define URSEL	URSEL0


#define STRING_USARTbuffer_SIZE	16
extern char USARTUSARTbuffer[STRING_USARTbuffer_SIZE];

#ifndef USART_chooseBAUDRATE
#define USART_chooseBAUDRATE	38400
#endif // USART_chooseBAUDRATE
#define BAUDRATE				((F_CPU + USART_chooseBAUDRATE * 8L) / (USART_chooseBAUDRATE * 16L) - 1)
//((F_CPU)/(USART_chooseBAUDRATE*16UL)-1)
//((F_CPU + UART_BAUD_RATE * 8L) / (UART_BAUD_RATE * 16L) - 1)

#define modeSynchronous			1
#define modeAsynchronous		0

/* USART_init
modeSynch	: if 1 --> Synchronous Communication, if 0 Asynchronous ! currently only asynchronous works  !
Baudrate	: set in definition
frSize		: valid 5 - 6 - 7 - 8 - 9
sParity		: valid 0 for no parity, 1 for odd,	2 for even
stBits		: valid 1 for 1, 2 for 2 stop bits	
*/
void USART_init(uint8_t modeSynch, uint8_t frSize, uint8_t sParity, uint8_t stBits);

/* USART_transmitFrame
valid : only 8 bit frames
TODO: cover cases for various frame sizes
*/
void USART_transmitFrame(uint8_t frame);

void USART_transmitString(const char * str);

void USART_TxEnable(uint8_t onoffswitch);
void USART_RxEnable(uint8_t onoffswitch);

#endif // MYUSART_H 
