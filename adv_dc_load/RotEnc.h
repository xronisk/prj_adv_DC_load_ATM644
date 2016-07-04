#ifndef ROTENC_H
#define ROTENC_H#include <avr/io.h>
//#include <avr/pgmspace.h>// pin definitions for rotary encoder#define RENC_PORT	PORTA#define RENC_DDR	DDRA#define RENC_PIN	PINA#define RENC_CHA	2#define RENC_CHB	3
//possible rotary encoder states#define RENC_DIR_CW		0x10#define RENC_DIR_CCW	0x20#define RENC_DIR_IDLE	0x00
#define RENC_START 0x00#define RENC_CW_FINAL 0x1#define RENC_CW_BEGIN 0x2#define RENC_CW_NEXT 0x3#define RENC_CCW_BEGIN 0x4#define RENC_CCW_FINAL 0x5#define RENC_CCW_NEXT 0x6//unsigned char ttable[7][4];volatile uint8_t RENC_state;extern volatile uint16_t RENC_counter;void RENC_setupPort(void);uint8_t RENC_process(void);void RENC_check(void);#endif