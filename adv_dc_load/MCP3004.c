#include "MCP3004.h"

/*	a)	initialises direction and states of SPI ports and pins 
		as well as the MCP3004's specified SlaveSelect pin
	b) sets the SPI mode as 0	(Shift SCK-edge = Falling, Capture SCK-edge = Rising)
	c) sets the prescaller of SPI as Fcpu / 128
	d) enables the SPI, sets the mCU as master	 */
void MCP3004_setup(void)
{
	SPImaster_initPort();
	SPImaster_DDR  |= (1 << MCP3004_SSn);		// SS is output
	MCP3004_pullSSn_HIGH();
	SPImaster_mode(0);
	SPImaster_prescaller(128);
	SPImaster_Enable();
}

/* returns an unsigned 16bit integer which holds the Digital Output Code of ADC
   since MCP3004 is an 10bit ADC, the 6 MSBs are null.
   The analog value of the returned integer equals
    AnValue = (Vreference * ReturnedInteger) / (2^10)*/
uint16_t MCP3004_fetchReading(uint8_t chnl)
{
	MCP3004_PORT &= ~(1 << MCP3004_SDI);				// make sure that initially MOSI is low
	uint8_t MCP3004_rawDATA_H, MCP3004_rawDATA_L;
	MCP3004_pullSSn_LOW();
	SPImaster_tranceiveByte(0x01);														// send 7 zeros + 1 startbit
	MCP3004_rawDATA_H = SPImaster_tranceiveByte(0xC0 | ((chnl << 4) & 0b00110000));		// send cmd: single-ended conversion
																						// from selected channel, receive 2 MSB
	MCP3004_rawDATA_L = SPImaster_tranceiveByte(0x00);									// receive the rest 7 LSB
	MCP3004_pullSSn_HIGH();
	MCP3004_PORT &= ~(1 << MCP3004_SDI);				// restore low MOSI
	MCP3004_rawDATA_H = ((MCP3004_rawDATA_H ) & 0b00000011);
	
	return	((MCP3004_rawDATA_H << 8) |
			((MCP3004_rawDATA_L )));
}
