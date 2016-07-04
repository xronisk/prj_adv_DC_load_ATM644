#include "myGLCD.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "font.h"
#include <util/delay.h>



// end of do not touch parameters--------------------------------------------------------

// --------------------------------------------------------------------------------------
void GLCD_init_ports()
{
	// databus initially output & low
		GLCD_DATA_DDR 	   = 0xFF;
		GLCD_DATA_PORT	   = 0x00;
		
	// control pins output & initially low 
		uint8_t x = ((1<<GLCD_CS1)|(1<<GLCD_CS2)|(1<<GLCD_RS)|(1<<GLCD_RW)|(1<<GLCD_E)|(1<<GLCD_RST));
		GLCD_CONTROL_DDR  |= x;
		GLCD_CONTROL_PORT &= ~x;			
		GLCD_CONTROL_PORT |=  (1<<GLCD_CS1);	//GLCD_deactiv_chip1;
		GLCD_CONTROL_PORT |=  (1<<GLCD_CS2);	//GLCD_deactiv_chip2;
		GLCD_E_clr();
		
		GLCD_Reset();
		_delay_ms(10);
		GLCD_notReset();
		GLCD_wait(2);
}

void GLCD_init_module()
{
	GLCD_init_ports();
	GLCD_writeByteFull(GLCD_CHIPboth, GLCD_CMD, 0x3E); //power off
	_delay_ms(10);
	GLCD_wait(2);
	GLCD_writeByteFull(GLCD_CHIPboth, GLCD_CMD, 0x3F); //power on
	GLCD_wait(2);

	GLCD_writeByteFull(GLCD_CHIPboth, GLCD_CMD, GLCD_StartLine(0));
	GLCD_wait(0);
	GLCD_wait(1);
	//GLCD_setStartLine(GLCD_CHIPboth, 56);

	GLCD_clr();
	_delay_ms(5);
}

void GLCD_writeByteFull(uint8_t chp, uint8_t typ, uint8_t d)
{
	GLCD_gotoChip(chp);
	//GLCD_setReg(typ)
	(typ) ? GLCD_setData() : GLCD_setCmd();
	GLCD_wr(d);
	GLCD_wait(chp);
}

uint8_t GLCD_read()
{	
	uint8_t tmp;
	GLCD_setRead();
	
	//GLCD_delayLong;
	GLCD_E_set();
	GLCD_delayShort;
	tmp = GLCD_DATA_PIN;
	GLCD_E_clr();
	return tmp;
}

uint8_t GLCD_readData() // to read data call it twice, remember to cancel position advance between
{
	uint8_t tmp;
	GLCD_setData();
	tmp = GLCD_read();
	return tmp;
}

uint8_t GLCD_readStatus(uint8_t chp)//--------------------------------------------------ok
{
	uint8_t st;
	GLCD_setRead();
	GLCD_setCmd();
	GLCD_gotoChip(chp);
	
	st = GLCD_read();

	st &= 0b10110000;
	return st;
}

void GLCD_wait(uint8_t chp)
{
	if (chp == 2)
	{
		while(GLCD_readStatus(1) & (GLCD_STATUS_BUSY | GLCD_STATUS_RESET)) { }
		while(GLCD_readStatus(0) & (GLCD_STATUS_BUSY | GLCD_STATUS_RESET)) { }
	}
	else
		while(GLCD_readStatus(chp) & (GLCD_STATUS_BUSY | GLCD_STATUS_RESET)) { }
}

void GLCD_clrPage(uint8_t chp, uint8_t pg)
{
	GLCD_gotoPage(chp, pg);
	// goto 0th byte of that page
		GLCD_gotoColumn(0);
	// go through each byte of that page and write 0's
		for(uint8_t x = 0; x <= 63; x++)
			{
				GLCD_writeByteFull(chp, GLCD_DATA, 0x00); // clear the page column by column
			}
	
}

void GLCD_clrColumn(uint8_t col)
{
	//GLCD_gotoColumn(col);
	uint8_t chp = col / 64;
	col = col % 64;
	//GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
	
	//
	for (uint8_t i = 0; i <= 7; i++)
	{
		GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
		GLCD_writeByteFull(chp, GLCD_CMD, GLCD_selectPage(i));
		GLCD_writeByteFull(chp, GLCD_DATA, 0x00);
	}
}

void GLCD_clr()
{
	for(uint8_t x = 0; x < 8; x++) // loop into every page
	 {
		//GLCD_gotoPage(GLCD_CHIPboth, x);
		GLCD_clrPage(GLCD_CHIPboth, x);
	 }
}

void GLCD_setStartLine(uint8_t chp, uint8_t l)//------------------------------ok
{
	GLCD_writeByteFull(chp, GLCD_CMD, GLCD_StartLine(l));
}

//---------------------------------------------------------------------------
void GLCD_gotoChip(uint8_t chp)
{
	if (chp == 0)										// if chip 1 selected
	{
		GLCD_CONTROL_PORT &= ~(1<<GLCD_CS1);			// enable chip 1
		GLCD_CONTROL_PORT |=  (1<<GLCD_CS2);			// disable chip 2
	}
	else if (chp == 1)									// if chip 2 selected
	{
		GLCD_CONTROL_PORT &= ~(1<<GLCD_CS2);			// enable chip 2
		GLCD_CONTROL_PORT |=  (1<<GLCD_CS1);			// disable chip 1
	}
	else if (chp == 2)									// if both chips selected
	{
		GLCD_CONTROL_PORT &= ~(1<<GLCD_CS1);			// enable chip 1
		GLCD_CONTROL_PORT &= ~(1<<GLCD_CS2);			// enable chip 2
	}
}

void GLCD_gotoColumn(uint8_t col)
{
	//if (col > 127) col = 0;

	uint8_t chp = col / 64;
	col = col % 64;
	GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
}

void GLCD_gotoPage(uint8_t chp, uint8_t pg)
{
	GLCD_writeByteFull(chp, GLCD_CMD, GLCD_selectPage(pg));
}

void GLCD_gotoColumnPage(uint8_t col, uint8_t pg)
{
	if (col > 127) col = 0;
	uint8_t chp = col / 64;
	col = col % 64;
	
	GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
	GLCD_writeByteFull(chp, GLCD_CMD, GLCD_selectPage(pg));
	
}

void GLCD_dot_onXY(uint8_t col, uint8_t row)		// --- IT DELETES OTHER POINTS of that byte!!!!!!
{
	// find the appropriate chip
	uint8_t chp = col / 64;
	
	// set the right column
	col = col % 64;
	
	// set the appropriate row - page and actuall bit
	uint8_t pg = row / 8;
	uint8_t dot = 1 << ((row % 8) );
	
	// set the defined position
	GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
	GLCD_writeByteFull(chp, GLCD_CMD, GLCD_selectPage(pg));
	
	// draw the dot
	GLCD_writeByteFull(chp, GLCD_DATA, dot);
} 

void GLCD_dot_append_XY(uint8_t col, uint8_t row)	// --- IT DOESN'T DELETE OTHER POINTS of that byte!!!!!!
{
	// find the appropriate chip
		uint8_t chp = col / 64;
	
	// find the right column
		col = col % 64;
	
	// find the appropriate page and actual bit
		uint8_t pg = row / 8;
		uint8_t dot = 1 << ((row % 8));
		
	// 	read the previous byte on that page / column
		GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
		GLCD_readData();
		GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
		uint8_t tempByte = GLCD_readData();
		
	// the new dot respects the previous dots
		tempByte |= dot;
	
	// set the defined position
		GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
		GLCD_writeByteFull(chp, GLCD_CMD, GLCD_selectPage(pg));
	
	// draw the dots
		GLCD_writeByteFull(chp, GLCD_DATA, tempByte);
}

void GLCD_math_dot_onXY(uint8_t col, uint8_t row)	// --- IT DELETES OTHER POINTS!!!!!!!!!!!!!
{
	// find the appropriate chip
	uint8_t chp = col / 64;
	
	// set the right column
	col = col % 64;
	
	// set the appropriate row - page and actuall bit
	uint8_t pg = 7 - (row / 8);
	uint8_t dot = 1 << (7 - (row % 8) );
	
	// set the defined position
	GLCD_writeByteFull(chp, GLCD_CMD,  GLCD_selectColumn(col));
	GLCD_writeByteFull(chp, GLCD_CMD, GLCD_selectPage(pg));
	
	// draw the dot
	GLCD_writeByteFull(chp, GLCD_DATA, dot);
}

void GLCD_horiLine(uint8_t x, uint8_t y, uint8_t ln)
{
	if(x + ln - 1 > 127) ln = 128 - x;
	
	for(uint8_t xtmp = x; xtmp <= x + ln - 1; xtmp ++)
	{
		
	GLCD_dot_append_XY(xtmp, y);
	}
}

void GLCD_vertLine(uint8_t x, uint8_t y, uint8_t ln)
{
	if(y + ln - 1 > 63) ln = 64 - y;
	
	for(uint8_t ytmp = y; ytmp <= y + ln - 1; ytmp ++)
	{
		
		GLCD_dot_append_XY(x, ytmp);
	}
}

void GLCD_RectangleSimple(uint8_t x, uint8_t y, uint8_t width, uint8_t height) // wrong
{
	GLCD_horiLine(x, y, width);
	GLCD_horiLine(x, y + height - 1, width);
	GLCD_vertLine(x, y, height);
	GLCD_vertLine(x + width - 1, y, height);
}

void GLCD_putCh(uint8_t c) // draft
{
//_delay_ms(100);
	 // if (c < 32) c = 32;
	 // if (c > 128) c = 128;
	
	for(uint8_t i = 0; i < 5; i++)							// the font has width of 6 (5 + 1 blank) pixels
	{
		GLCD_setData();
		GLCD_wr(pgm_read_byte((font_5x7_data + 5 * (c-32)) + i));
		_delay_us(1);
		//GLCD_wait(2);
	}
	// increase position of x by 6 to write next char
	GLCD_setData();
	GLCD_wr(0x00);

}

void GLCD_putStr(const char* str) {
	while(*str != '\0') {
		GLCD_putCh(*str++);
	}
}