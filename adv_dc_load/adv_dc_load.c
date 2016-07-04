/*
 * GccApplication1.c
 *
 * Created: 7/4/2016 5:06:33 PM
 *  Author: chk
 */ 

#include <avr/io.h>#include <avr/pgmspace.h>#include <avr/interrupt.h>#include <util/delay.h>#include <stdlib.h>#include "MCP3004.h"#include "myGLCD.h"#include "SPImaster.h"#include "myUSART.h"#include "MCP4911.h"#include "RotEnc.h"#include "font.h"

// Voltage Divider parameters - this voltage divider reduces the voltage of the DUT into measurable levels#define VoltageDividerRatio		125.39

// ADC parameters ---------------------------------------------------------------------------------------#define ADC_channel_Voltage		0#define ADC_channel_Current		1#define Vref_Volts				4.105				// define the adc's Vref and resolution so the adc_lsb_value_mV can be calculated#define ADC_resolution			10
#define ADCsteps				1024				// todo: implement a calculation 2^ADC_resolution
#define ADC_BitValue_mV			4.008789
	// double const ADC_LSB_value_mV =	Vref_Volts/ADCsteps;	// double const ADC_LSB_value_mV = 4.0; // Vref/1024

// digital filter parameters, variables and functions ----------------------------------------------------#define SAMPLES_FOR_ONE_TRUE_ADC_READING	24

// do not edit :#define MEDIAN								(SAMPLES_FOR_ONE_TRUE_ADC_READING+1)/2
uint16_t C_array[SAMPLES_FOR_ONE_TRUE_ADC_READING];uint16_t V_array[SAMPLES_FOR_ONE_TRUE_ADC_READING];
void Insertion_Sort(uint16_t Array[]){	int8_t i;	uint8_t j;	uint16_t current;		// Sort Array[]	for(j=1; j<SAMPLES_FOR_ONE_TRUE_ADC_READING; j++)	{		current = Array[j];		i = j - 1;		while(i>=0 && Array[i]>current)		{			Array[i+1] = Array[i];			i--;		}		Array[i+1] = current;	}}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *buffer;									// here are the strings held to be used by x-to-string functions
volatile uint16_t	SPIdata = 0;volatile uint16_t	RENC_counter = 0;volatile uint8_t	RENC_state = 0;volatile double		ADCreading_I,					ADCreading_V,					P_consumed,					ADCreading_DACVoltage;

void GLCD_showIndex(void);
void GLCD_showData(void);
void UART_showIndex(void);
void UART_showData(void);

int main(void)
{
	_delay_ms(800);	GLCD_init_module();	RENC_setupPort();	MCP3004_setup();
	MCP4911_setupPorts();
	MCP4911_sendWord(SPIdata);			// on reset, turn dac's output to 0
	
	//USART_init(0,8,0,1);
	DDRA |= (1 << 7)|(1 << 6);		// debugging tool...
	
	// todo : calculate ADC's bit value(mV) here: const ADC_BitValue_mV = Vref/(2^ADC_Resolution)
	
	//Setup the interrupt on Rotary encoder action here		EICRA = 0x00;		EIMSK = 0x00;		PCICR = 0x01;		PCMSK0 = (1<<RENC_CHA)|(1<<RENC_CHB);
	
	GLCD_showIndex();
	
	/* setting timer/counter1 to tick every 51.2us, restart every 9.984ms, trigger an interrupt (show data on GLCD) upon 25 restarts */
		OCR1A = 194; // 4882;				//	4883	* 51,2 us ~ 250m sec		TCCR1A = 0x00;		TCCR1B |= 1 << WGM12;		// CTC mode, clear timer/counter on compare with ocr1a		TIMSK1 |= 1 << OCIE1A;		// interrupt on compare counter with ocr1a
/*
		 TIMSK1 |= 1 << OCIE1B;	// interrupt on compare counter with ocr1b* /
		 OCR1B = 500;//292; // 293*51.2us = 15ms*/
	// start timer : tick every 1024 * CPU_ticks = 1024 * 50ns = 51.2us		TCCR1B |= 0b00000101;				
	
	sei();   
	
	while(1)
    {
        //TODO:: Please write your application code 
    }
}

/*	interrupt service routine triggered by ANY action of Rotary encoder (either valid or invalid).	Evaluates the state of rotary encoder, if it is a valid action prepares the word to be send to the DAC	and sends it */
ISR(PCINT0_vect){	RENC_check();	SPIdata = RENC_counter & 0x03FF;	MCP4911_sendWord(SPIdata);}

/*Interrup*/
ISR(TIMER1_COMPA_vect)									// every 250msec{	static uint8_t cntr = 0;		if (cntr < SAMPLES_FOR_ONE_TRUE_ADC_READING)
	{
		PORTA |= 1 << 7;
		
		V_array[cntr] = MCP3004_fetchReading(ADC_channel_Voltage);		C_array[cntr] = MCP3004_fetchReading(ADC_channel_Current);
		
/*
		ADCreading_V = MCP3004_fetchReading(ADC_channel_Voltage) * ADC_BitValue_mV / *Vref_Volts* / / VoltageDividerRatio;
		ADCreading_I = MCP3004_fetchReading(ADC_channel_Current) * ADC_BitValue_mV  / 1000 ;
		P_consumed	 = ADCreading_V * ADCreading_I;*/
		cntr++;
		PORTA &= ~(1 << 7);
	} 
	else
	{
		PORTA |=  (1 << 6);
		Insertion_Sort(C_array);		Insertion_Sort(V_array);
		
		ADCreading_V = V_array[MEDIAN] * ADC_BitValue_mV / VoltageDividerRatio;
		ADCreading_I = C_array[MEDIAN] * ADC_BitValue_mV / 1000;
		P_consumed	 = ADCreading_V * ADCreading_I;
		GLCD_showData();
		cntr = 0;
		PORTA &= ~(1 << 6);
	}}void GLCD_showIndex(void){	GLCD_gotoColumnPage(2, 0);	GLCD_putStr("V - V:");	GLCD_gotoColumnPage(2, 1);	GLCD_putStr("I - A:");	GLCD_gotoColumnPage(2, 2);	GLCD_putStr("P - W:");/*	GLCD_gotoColumnPage(2, 2);	GLCD_putStr("DAC mV:");*/	GLCD_gotoColumnPage(2, 4);	GLCD_putStr("DAC word:");}void GLCD_showData(void){	buffer = malloc(7);	GLCD_gotoColumnPage(68, 0 );	GLCD_putStr(dtostrf(ADCreading_V, 6, 3,buffer)); GLCD_putStr("   ");	GLCD_gotoColumnPage(68, 1 );	GLCD_putStr(dtostrf(ADCreading_I, 6, 3, buffer)); GLCD_putStr("   ");	GLCD_gotoColumnPage(68, 2 );	GLCD_putStr(dtostrf(P_consumed, 6, 3, buffer));	GLCD_putStr("   ");/*	GLCD_gotoColumnPage(68, 3 );	GLCD_putStr(dtostrf(ADCreading_DACVoltage * Vref_Volts, 6, 3, buffer));	GLCD_putStr("   "); */	GLCD_gotoColumnPage(74, 4 );	GLCD_putStr(utoa(RENC_counter, buffer, 10));	GLCD_putStr("   ");	free(buffer);}void UART_showIndex(void){	USART_TxEnable(1);	USART_transmitString("\n\r");	USART_transmitString("INDEX");	USART_transmitString("\t");	USART_transmitString("Vbatt_V");	USART_transmitString("\t");	USART_transmitString("Io_A");	USART_transmitString("\t");	USART_transmitString("Po_W");	USART_TxEnable(0);}void UART_showData(void){	// todo ...}