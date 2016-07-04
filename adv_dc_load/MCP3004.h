/*
ADC		- 10 bit
		- Successive Aproximation

*/

#ifndef MCP3004_H
#define MCP3004_H

#include "SPImaster.h"

// the SPI pins location in master device - we dont need MOSI when handling MCP3004, this chip only sends data after being activated
#define	MCP3004_DDR		SPImaster_DDR
#define	MCP3004_PORT	SPImaster_PORT
#define	MCP3004_SDO		SPImaster_pinMISO
#define	MCP3004_SDI		SPImaster_pinMOSI
#define	MCP3004_SCK		SPImaster_pinSCK

#define	MCP3004_SSn		3					// here is the location of MCP3004's SlaveSelect pin in master device

#define MCP3004_pullSSn_LOW()	(MCP3004_PORT &= ~(1 << MCP3004_SSn))
#define MCP3004_pullSSn_HIGH()	(MCP3004_PORT |=  (1 << MCP3004_SSn))

/*	a)	initialises the directions and the states of SPI ports and pins 
		as well as the MCP3004's specified SlaveSelect pin
	b) sets the SPI mode as 0	(Shift SCK-edge = Falling, Capture SCK-edge = Rising)
	c) sets the prescaller of SPI as Fcpu / 128
	d) enables the SPI, sets the mCU as master	 */
void MCP3004_setup(void);

/* returns an unsigned 16bit integer which holds the Digital Output Code of ADC
   since MCP3004 is an 10bit ADC, the 6 MSBs are null.
   The analog value of the returned integer equals
    AnValue = (Vreference * ReturnedInteger) / (2^10)*/
uint16_t MCP3004_fetchReading(uint8_t chnl);

#endif /*MCP3004_H*/
