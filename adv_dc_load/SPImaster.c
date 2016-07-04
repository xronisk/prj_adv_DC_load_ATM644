#include "SPImaster.h"
#include <util/delay.h>

void SPImaster_initPort()
{
	SPImaster_DDR &= ~(1 << SPImaster_pinMISO);		// MISO is input
	SPImaster_DDR |=  (1 << SPImaster_pinMOSI);		// MOSI is output
	SPImaster_DDR |=  (1 << SPImaster_pinSCK);		// SCK is output
	SPImaster_DDR  |= (1 << SPImaster_pinSS);		// SS is output
	SPImaster_PORT |= (1 << SPImaster_pinSS); 		//
}

void SPImaster_prescaller(uint8_t presc)
{
	// mask SPCR to preserve other bits while clearing prescaller bitd
		SPCR &= 0b11111100;
		
		switch (presc) {
			case  2: SPSR |= 1 << SPI2X; break;
			case  4: break;
			case  8: SPSR |= 1 << SPI2X; SPCR |= 1; break;
			case 16: SPCR |= 1; break;
			case 32: SPSR |= 1 << SPI2X; SPCR |= 2; break;
			case 64: SPCR |= 2; break;
			default: SPCR |= 3;	}
}

void SPImaster_mode(uint8_t mode)
{
	if ((mode > 3)||(mode < 0))	mode = 0;

	if (mode % 2)	SPCR |=  (1 << CPHA);	else			SPCR &= ~(1 << CPHA);					// if  CPOL = 1 --> SCK high when idle		if (mode / 2)	{SPCR |= (1 << CPOL); SPImaster_PORT |=  (1 << SPImaster_pinSCK);}
// if CPOL = 0 --> SCK low when idle
	else			{SPCR &= ~(1 << CPOL); SPImaster_PORT &= ~(1 << SPImaster_pinSCK);}
}


void SPImaster_DataOrder(uint8_t data_order)
{
	if (data_order)	SPCR |=  (1 << DORD);			// L-S-bit of data transmitted first
	else			SPCR &= ~(1 << DORD);}			// M-S-bit of data transmitted first
	
uint8_t SPImaster_tranceiveByte(uint8_t bytee){	// wait 1 clock tick (after pulling CS low), so that slave prepares his byte-to-send on time
		//asm volatile("nop\n\t"::);
	
	// load data to send
		SPDR = bytee;
	// wait until transmission ends
		while(!(SPSR & (1<<SPIF)));

		return SPDR;}