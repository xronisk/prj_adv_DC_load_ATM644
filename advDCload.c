/*
 * GccApplication1.c
 *
 * Created: 7/4/2016 5:06:33 PM
 *  Author: chk
 */ 

#include <avr/io.h>

// Voltage Divider parameters - this voltage divider reduces the voltage of the DUT into measurable levels

// ADC parameters

volatile uint16_t SPIdata;

void GLCD_showIndex(void);
void GLCD_showData(void);

int main(void)
{
	_delay_ms(800);
	DDRA |= (1 << 7)|(1 << 6);		// debugging tool...
	
	//RENC_setupInterrupt();
	
	GLCD_showIndex();
	
	/* setting timer/counter1 to tick every 51.2us, restart every 250ms, trigger an interrupt (show data on GLCD) upon restart */
		OCR1A = 4882;				//	4883	* 51,2 us ~ 250m sec
		// TIMSK1 |= 1 << OCIE1B;	// interrupt on compare counter with ocr1b*/
		TCCR1B |= 0b00000101;		// start timer : tick every 1024 * CPU_ticks = 1024 * 50ns = 51.2us
    
	while(1)
    {
        //TODO:: Please write your application code 
    }
}

/*	interrupt service routine triggered by ANY action of Rotary encoder (either valid or invalid).
ISR(PCINT0_vect)

ISR(TIMER1_COMPA_vect)									// every 250msec