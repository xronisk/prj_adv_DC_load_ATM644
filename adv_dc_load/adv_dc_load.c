/*
 * GccApplication1.c
 *
 * Created: 7/4/2016 5:06:33 PM
 *  Author: chk
 */ 

#include <avr/io.h>

// Voltage Divider parameters - this voltage divider reduces the voltage of the DUT into measurable levels

// ADC parameters ---------------------------------------------------------------------------------------
#define ADCsteps				1024				// todo: implement a calculation 2^ADC_resolution
#define ADC_BitValue_mV			4.008789
	// double const ADC_LSB_value_mV =	Vref_Volts/ADCsteps;

// digital filter parameters, variables and functions ----------------------------------------------------

// do not edit :
uint16_t C_array[SAMPLES_FOR_ONE_TRUE_ADC_READING];
void Insertion_Sort(uint16_t Array[])

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *buffer;									// here are the strings held to be used by x-to-string functions
volatile uint16_t	SPIdata = 0;

void GLCD_showIndex(void);
void GLCD_showData(void);
void UART_showIndex(void);
void UART_showData(void);

int main(void)
{
	_delay_ms(800);
	MCP4911_setupPorts();
	MCP4911_sendWord(SPIdata);			// on reset, turn dac's output to 0
	
	//USART_init(0,8,0,1);
	DDRA |= (1 << 7)|(1 << 6);		// debugging tool...
	
	// todo : calculate ADC's bit value(mV) here: const ADC_BitValue_mV = Vref/(2^ADC_Resolution)
	
	//Setup the interrupt on Rotary encoder action here
	
	GLCD_showIndex();
	
	/* setting timer/counter1 to tick every 51.2us, restart every 9.984ms, trigger an interrupt (show data on GLCD) upon 25 restarts */
		OCR1A = 194; // 4882;				//	4883	* 51,2 us ~ 250m sec
/*
		 TIMSK1 |= 1 << OCIE1B;	// interrupt on compare counter with ocr1b* /
		 OCR1B = 500;//292; // 293*51.2us = 15ms*/
	// start timer : tick every 1024 * CPU_ticks = 1024 * 50ns = 51.2us
	
	sei();   
	
	while(1)
    {
        //TODO:: Please write your application code 
    }
}

/*	interrupt service routine triggered by ANY action of Rotary encoder (either valid or invalid).
ISR(PCINT0_vect)

/*Interrup*/
ISR(TIMER1_COMPA_vect)									// every 250msec
	{
		PORTA |= 1 << 7;
		
		V_array[cntr] = MCP3004_fetchReading(ADC_channel_Voltage);
		
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
		Insertion_Sort(C_array);
		
		ADCreading_V = V_array[MEDIAN] * ADC_BitValue_mV / VoltageDividerRatio;
		ADCreading_I = C_array[MEDIAN] * ADC_BitValue_mV / 1000;
		P_consumed	 = ADCreading_V * ADCreading_I;
		GLCD_showData();
		cntr = 0;
		PORTA &= ~(1 << 6);
	}