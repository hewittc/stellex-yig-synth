// Speed settings
#define F_CPU 		(8000000UL)
#define BAUD_RATE	19200

// Frequency range
#define MAX_FREQUENCY 9500
#define MIN_FREQUENCY 5500
#define DEF_FREQUENCY 6750

// Synthesizer properties
#define REF_CLOCK_HZ    10000000
#define STEP_SIZE_HZ    250000
#define PRESCALE_FACTOR 4      // (Prescaler before LMX2326)

// Port definitions
#define PLL_CLK    (1 << 0)    // Output 
#define PLL_DATA   (1 << 1)    // Output 
#define PLL_LATCH  (1 << 2)    // Output
#define PLL_LOCKED (1 << 3)    // Input

// ADF411x / LMX23x6 PLL latch labels
#define PLL_LATCH_R    0x00
#define PLL_LATCH_N    0x01
#define PLL_LATCH_INIT 0x03

// Ring buffer
#define BUFF_SIZE			5
#define BUFF_ERR_OVERFLOW	(1 << 1)
#define BUFF_ERR_EMPTY		(1 << 0)

// Key press associations
#define KEY_UP_PRESS		0x01
#define KEY_DOWN_PRESS		0x02
#define KEY_LEFT_PRESS		0x03
#define KEY_RIGHT_PRESS		0x04
#define KEY_ENTER_PRESS		0x05
#define KEY_EXIT_PRESS		0x06
#define KEY_UP_RELEASE		0x07
#define KEY_DOWN_RELEASE	0x08
#define KEY_LEFT_RELEASE	0x09
#define KEY_RIGHT_RELEASE	0x0A
#define KEY_ENTER_RELEASE	0x0B
#define KEY_EXIT_RELEASE	0x0C

// Special graphical characters
#define GRAPHIC_CHECK		0x05
#define GRAPHIC_CANCEL		0x06
#define GRAPHIC_ARROW_RIGHT	0x08

// Useful includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h> 
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

// display.c
void USART_Initialize(void);
U16 get_crc(U8*, U8);
void USART_Transmit(U8*, U8);
U8 USART_Receive(void);
void LCD_Ping(U8*, U8);
void LCD_Version(void);
void LCD_Clear(void);
void LCD_SetLine1(U8*);
void LCD_SetLine2(U8*);
void LCD_SetCursorPosition(U8, U8);
void LCD_SetCursorStyle(U8);
void LCD_SetContrast(U8);
void LCD_SetBrightness(U8);
void LCD_SendData(U8, U8, U8*, U8);
void Buffer_Initialize(void);
void Buffer_Add(U8);
U8 Buffer_Remove(void);
U8 Buffer_Elements(void);

// synth.c
void IO_wait(void);
void BUS_write_word(S8, S32, U8, U8);
void PLL_write_word(S32, S32);
void PLL_latch_data(void);
void PLL_set_N_divisor(S32);
void PLL_set_MHz(S32);
BOOL PLL_locked(void);
