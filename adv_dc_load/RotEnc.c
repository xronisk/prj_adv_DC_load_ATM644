#include "RotEnc.h"

void RENC_check(void)
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
{