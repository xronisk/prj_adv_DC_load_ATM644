#ifndef MYGLCD_H
#define MYGLCD_H

#include <avr/io.h>

// pin definitions for GLCD module-------------------------------------------------
#define GLCD_DATA_DDR				DDRC	// DATA PORT IS PORT C ON MIKROE AVR6
#define GLCD_DATA_PORT				PORTC
#define GLCD_DATA_PIN				PINC

#define GLCD_CONTROL_DDR			DDRD	// CONTROL PORT IS PORT C ON MIKROE AVR6
#define GLCD_CONTROL_PORT			PORTD
#define GLCD_CONTROL_PIN			PIND

#define GLCD_CS1					2		// ACTIVE LOW - SELECT SEG 1-64
#define GLCD_CS2					3		// ACTIVE LOW - SELECT SEG 65-128
#define GLCD_RS						4		// REGISTER SELECT, HIGH = EXPECTS DATA / LOW = EXPECTS COMMAND
#define GLCD_DI						4		// REGISTER SELECT, HIGH = EXPECTS DATA / LOW = EXPECTS COMMAND
#define GLCD_RW						5		// LOW = WRITE to / HIGH = READ from
#define GLCD_E						6		// ACTIVE HIGH
#define GLCD_RST					1		// ACTIVE LOW


// activate & deactivate chips ----------------------------------------------------------
#define GLCD_CHIP1					0
#define GLCD_CHIP2					1
#define GLCD_CHIPboth				2

// RST GLCD -----------------------------------------------------------------------------
#define GLCD_Reset()				(GLCD_CONTROL_PORT &= ~(1<<GLCD_RST))
#define GLCD_notReset()				(GLCD_CONTROL_PORT |=  (1<<GLCD_RST))

// manipulate Read / Write ----------------------------------------------------------------
#define GLCD_setRead()				(GLCD_DATA_DDR = 0x00, GLCD_CONTROL_PORT |=  (1<<GLCD_RW))		//set direction of bus,
#define GLCD_setWrite()				(GLCD_DATA_DDR = 0xFF, GLCD_CONTROL_PORT &= ~(1<<GLCD_RW))

// select Register Instruction / Data ------------------------------------------------------
#define GLCD_DATA					1
#define GLCD_CMD					0
#define GLCD_setData()				(GLCD_CONTROL_PORT |=  (1<<GLCD_DI))
#define GLCD_setCmd()				(GLCD_CONTROL_PORT &= ~(1<<GLCD_DI))

// act on E pin -------------------------------------------------------------------------
#define GLCD_E_set()				(GLCD_CONTROL_PORT |=  (1<<GLCD_E))
#define GLCD_E_clr()				(GLCD_CONTROL_PORT &= ~(1<<GLCD_E))

#define longDelay					22
#define shortDelay					14
#define GLCD_delayLong				for(uint8_t GLCD_dly=0; GLCD_dly< longDelay; GLCD_dly++){asm volatile("nop\n");}
#define GLCD_delayShort				for(uint8_t GLCD_dly=0; GLCD_dly<shortDelay; GLCD_dly++){asm volatile("nop\n");}

#define GLCD_E_strobe()				{\
	GLCD_delayLong;\
	GLCD_E_set();\
	GLCD_delayShort;\
	GLCD_E_clr();\
}

// status -------------------------------------------------------------------------
#define GLCD_STATUS_BUSY			(1<<7)
#define GLCD_STATUS_OFF				(1<<5)
#define GLCD_STATUS_RESET			(1<<4)



// Command bytes -// to create the command byte, Y / X / L is filtered for the LSbits
// (...& 0x3F) then the other MSbits are set to form the command, helpful but complicated
#define GLCD_selectColumn(Y)		(((Y) & 0x3F) | 0x40)
#define GLCD_selectPage(X)			(((X) & 0x07) | 0xB8)
#define GLCD_StartLine(L)			(((L) & 0x3F) | 0xC0)
/* orientation of vertical axis 
#define  GLCD_0_UP					0
#define  GLCD_0_DOWN				1
*/
//--------------------------------------------------------------------------------------
void GLCD_init_ports(void);
void GLCD_init_module(void);


#define GLCD_wr(bytee)				{\
									GLCD_setWrite();\
									GLCD_DATA_PORT = bytee;\
									GLCD_E_strobe();\
									}
									
void GLCD_writeByteFull(uint8_t chp, uint8_t typ, uint8_t d);

uint8_t GLCD_read(void);
uint8_t GLCD_readData(void);							// to read data, call it twice, remember to cancel position advance between
uint8_t GLCD_readStatus(uint8_t chp);
void GLCD_wait(uint8_t chp);	

void GLCD_clrPage(uint8_t chp, uint8_t pg);
void GLCD_clrColumn(uint8_t col);								
void GLCD_clr();
void GLCD_setStartLine(uint8_t chp, uint8_t l);//------------------------------ok

void GLCD_gotoChip(uint8_t chp);
void GLCD_gotoColumn(uint8_t col);
void GLCD_gotoPage(uint8_t chp, uint8_t pg);
void GLCD_gotoColumnPage(uint8_t col, uint8_t pg);

void GLCD_dot_onXY(uint8_t col, uint8_t row);			// --- IT DELETES OTHER POINTS of that byte!
void GLCD_math_dot_onXY(uint8_t col, uint8_t row);		// --- IT DELETES OTHER POINTS!!
void GLCD_dot_append_XY(uint8_t col, uint8_t row);		// --- IT DOESN'T DELETE OTHER POINTS of that byte!

void GLCD_horiLine(uint8_t x, uint8_t y, uint8_t ln);
void GLCD_vertLine(uint8_t x, uint8_t y, uint8_t ln);
void GLCD_RectangleSimple(uint8_t x, uint8_t y, uint8_t width, uint8_t height); // wrong


void GLCD_putCh(uint8_t c);
void GLCD_putStr(const char* str);

#endif