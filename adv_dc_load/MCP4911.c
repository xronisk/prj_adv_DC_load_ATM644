#include "MCP4911.h"		//DAC

void MCP4911_setupPorts(void)
{
	MCP4911_DDR |= (1 << MCP4911_SDI);								// MOSI OUTPUT
	MCP4911_DDR |= (1 << MCP4911_SCK);								// SCK OUTPUT
	MCP4911_DDR |= (1 << MCP4911_LDACn);							// LDACn OUTPUT
	MCP4911_DDR |= (1 << MCP4911_SSn);								// SSn OUTPUT
	
	MCP4911_PORT |= (1 << MCP4911_LDACn);							// IDLE LDACn HIGH
	MCP4911_PORT |= (1 << MCP4911_SSn);
}

void MCP4911_sendWord(uint16_t DACword)
{
	uint16_t tmpSPIdata = (DACword << 2);
	tmpSPIdata |= 0x3000;
	
	//send via SPI
	MCP4911_PORT &= ~(1 << MCP4911_SSn);								// PULL SSn LOW
	//_delay_us(1);
	SPImaster_tranceiveByte((tmpSPIdata >> 8));
	//_delay_us(1);
	SPImaster_tranceiveByte((uint8_t) tmpSPIdata);
	//_delay_us(1);
	MCP4911_PORT |= (1 << MCP4911_SSn);								// PULL SSn HIGH
	// UPDATE OUTPUT OF DAC
	MCP4911_PORT &= ~(1 << MCP4911_LDACn);								// PULL LDACn LOW - ACTIVATE
	//_delay_us(1);
	MCP4911_PORT |= (1 << MCP4911_LDACn);
}