/*
 * GccApplication1.c
 *
 * Created: 7/4/2016 5:06:33 PM
 *  Author: chk
 */ 

#include <avr/io.h>#include <avr/pgmspace.h>#include <avr/interrupt.h>#include <util/delay.h>#include <stdlib.h>#include "MCP3004.h"#include "myGLCD.h"#include "SPImaster.h"#include "myUSART.h"#include "MCP4911.h"#include "RotEnc.h"#include "font.h"

// Voltage Divider parameters - this voltage divider reduces the voltage of the DUT into measurable levels#define VoltageDividerRatio		125.39

// ADC parameters#define ADC_channel_Voltage		0#define ADC_channel_Current		1#define Vref_Volts				4.105				// define the adc's Vref and resolution so the adc_lsb_value_mV can be calculated#define ADC_resolution			10

volatile uint16_t SPIdata;volatile uint16_t RENC_counter = 0;volatile uint8_t  RENC_state = 0;volatile double	ADCreading_I,				ADCreading_V,				P_consumed,				ADCreading_DACVoltage;

void GLCD_showIndex(void);
void GLCD_showData(void);

int main(void)
{
	_delay_ms(800);	GLCD_init_module();	RENC_setupPort();	MCP4911_setupPorts();	MCP3004_setup();
	DDRA |= (1 << 7)|(1 << 6);		// debugging tool...
	
	//RENC_setupInterrupt();		EICRA = 0x00;		EIMSK = 0x00;		PCICR = 0x01;		PCMSK0 =  (1<<RENC_CHA)|(1<<RENC_CHB);
	
	GLCD_showIndex();
	
	/* setting timer/counter1 to tick every 51.2us, restart every 250ms, trigger an interrupt (show data on GLCD) upon restart */
		OCR1A = 4882;				//	4883	* 51,2 us ~ 250m sec		TCCR1A = 0x00;		TCCR1B |= 1 << WGM12;		// CTC mode, clear timer/counter on compare with ocr1a		TIMSK1 |= 1 << OCIE1A;		// interrupt on compare counter with ocr1a
		// TIMSK1 |= 1 << OCIE1B;	// interrupt on compare counter with ocr1b*/		// OCR1B = 292;
		TCCR1B |= 0b00000101;		// start timer : tick every 1024 * CPU_ticks = 1024 * 50ns = 51.2us		
    
	while(1)
    {
        //TODO:: Please write your application code 
    }
}

/*	interrupt service routine triggered by ANY action of Rotary encoder (either valid or invalid).	Evaluates the state of rotary encoder, if it is a valid action prepares the word to be send to the DAC	and sends it */
ISR(PCINT0_vect){	RENC_check();	SPIdata = RENC_counter & 0x03FF;	MCP4911_sendWord(SPIdata);}

ISR(TIMER1_COMPA_vect)									// every 250msec{	PORTA |= 1 << 7;	GLCD_showData();	PORTA &= ~(1 << 7);}void GLCD_showIndex(void){	GLCD_gotoColumnPage(2, 0);	GLCD_putStr("V - V:");	GLCD_gotoColumnPage(2, 1);	GLCD_putStr("I - A:");	GLCD_gotoColumnPage(2, 2);	GLCD_putStr("P - W:");/*	GLCD_gotoColumnPage(2, 2);	GLCD_putStr("DAC mV:");*/	GLCD_gotoColumnPage(2, 4);	GLCD_putStr("DAC word:");}void GLCD_showData(void){	buffer = malloc(7);	GLCD_gotoColumnPage(68, 0 );	GLCD_putStr(dtostrf(ADCreading_V, 6, 3,buffer)); GLCD_putStr("   ");	GLCD_gotoColumnPage(68, 1 );	GLCD_putStr(dtostrf(ADCreading_I, 6, 3, buffer)); GLCD_putStr("   ");	GLCD_gotoColumnPage(68, 2 );	GLCD_putStr(dtostrf(P_consumed, 6, 3, buffer));	GLCD_putStr("   ");/*	GLCD_gotoColumnPage(68, 3 );	GLCD_putStr(dtostrf(ADCreading_DACVoltage * Vref_Volts, 6, 3, buffer));	GLCD_putStr("   "); */	GLCD_gotoColumnPage(74, 4 );	GLCD_putStr(utoa(RENC_counter, buffer, 10));	GLCD_putStr("   ");	free(buffer);}