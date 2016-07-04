#include<avr/io.h>

#ifndef SPIMASTER_H
#define SPIMASTER_H

#define SPImaster_PORT		PORTB
#define SPImaster_DDR		DDRB
#define SPImaster_pinMOSI	5	// pb5
#define SPImaster_pinMISO	6	// pb6
#define SPImaster_pinSCK	7	// pb7
#define SPImaster_pinSS		4	// pb4 // create as many SlaveSelect as needed

#define SPImaster_Enable()		(SPCR |=  (1 << SPE)|(1 << MSTR))
#define SPImaster_Disable()		(SPCR &= ~(1 << SPE))

void SPImaster_initPort(void);

void SPImaster_prescaller(uint8_t presc);

/* SPI Mode | CPOL	|  CPHA	| Shift SCK-edge| Capture SCK-edge
		0	|	0	|	0	|	Falling		|	Rising
		1	|	0	|	1	|	Rising		|	Falling
		2	|	1	|	0	|	Rising		|	Falling
		3	|	1	|	1	|	Falling		|	Rising			*/
void SPImaster_mode(uint8_t mode);

/*	for transmission with LSB first select 1
	for transmission with MSB first select 0	*/
void SPImaster_DataOrder(uint8_t data_order);

uint8_t SPImaster_tranceiveByte(uint8_t bytee);

#endif


