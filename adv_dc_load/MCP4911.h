#ifndef MCP4911_H
#define MCP4911_H

#include "SPImaster.h"

#define	MCP4911_DDR		SPImaster_DDR
#define	MCP4911_PORT	SPImaster_PORT
#define	MCP4911_SDI		SPImaster_pinMOSI
#define	MCP4911_SCK		SPImaster_pinSCK
#define	MCP4911_LDACn	1
#define	MCP4911_SSn		0
void MCP4911_setupPorts(void);
void MCP4911_sendWord(uint16_t DACword);

#endif