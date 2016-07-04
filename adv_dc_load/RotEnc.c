#include "RotEnc.h"//#include <avr/pgmspace.h>volatile uint16_t RENC_counter;//volatile uint8_t RENC_state;const unsigned char ttable[7][4] = {	// RENC_START	{RENC_START,    RENC_CW_BEGIN,  RENC_CCW_BEGIN, RENC_START},	// RENC_CW_FINAL	{RENC_CW_NEXT,  RENC_START,     RENC_CW_FINAL,  RENC_START | RENC_DIR_CW},	// RENC_CW_BEGIN	{RENC_CW_NEXT,  RENC_CW_BEGIN,  RENC_START,     RENC_START},	// RENC_CW_NEXT	{RENC_CW_NEXT,  RENC_CW_BEGIN,  RENC_CW_FINAL,  RENC_START},	// RENC_CCW_BEGIN	{RENC_CCW_NEXT, RENC_START,     RENC_CCW_BEGIN, RENC_START},	// RENC_CCW_FINAL	{RENC_CCW_NEXT, RENC_CCW_FINAL, RENC_START,     RENC_START | RENC_DIR_CCW},	// RENC_CCW_NEXT	{RENC_CCW_NEXT, RENC_CCW_FINAL, RENC_CCW_BEGIN, RENC_START},};void RENC_setupPort(void){	// prepare ports	RENC_DDR  &= ~((1<<RENC_CHA)|(1<<RENC_CHB));		// channel pins are inputs	RENC_PORT &= ~((1<<RENC_CHA)|(1<<RENC_CHB));		// internal pullups deactivated for channel pins}

void RENC_check(void){	uint8_t x = RENC_process();
	if (x==0x10)
	{
		if (RENC_counter == 1023)
		RENC_counter = 1023;
		else
		RENC_counter++;
	}
	if (x==0x20)
	{
		if (RENC_counter == 0)
		RENC_counter = 0;
		else
		RENC_counter--;
	}
}

uint8_t RENC_process(void)
{	uint8_t pinstate=0;	pinstate = (((bit_is_set(RENC_PIN, RENC_CHA) >> RENC_CHA) & 1) << 1) | ((bit_is_set(RENC_PIN, RENC_CHB) >> RENC_CHB) & 1); //0b000000AB	RENC_state = ttable[RENC_state & 0xF][pinstate];	return (RENC_state & 0x30);}