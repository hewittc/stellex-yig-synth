/*
 * Christopher Hewitt (hewittc@jlab.org)
 * 
 * main.c: Stellex synthesizer control with CrystalFontz LCD support
 * AVR Studio 4
 *
 * Version 2009.06.24
 * 2009 Jefferson Lab
 */

#include "typedefs.h"
#include "main.h"

// PLL programming constants
const S32 R = REF_CLOCK_HZ / STEP_SIZE_HZ; 

// EEPROM address
S32 EEMEM freq_eeprom;

// Frequencies in MHz
S32 freq_cur = 6500;
S32 freq_new = 6500;
S32 freq_last = -1;

// Lock on status
U8 lock_on = FALSE;

// Position of edit cursor
S8 curs_pos = 3;

// Some special graphics
C8 arrow[2] = {GRAPHIC_ARROW_RIGHT, '\0'};
C8 locked[2] = {GRAPHIC_CHECK, '\0'};
C8 not_locked[2] = {GRAPHIC_CANCEL, '\0'};

/* Just for testing at the moment... */
int main(void) {

	// PB0-PB2 = outputs for LMX2326 control lines
	// PB3     = input for LMX2326 Fo/LD line
	DDRB  = 0x07;
	PORTB = 0x00;

	// Initialize USART
	USART_Initialize();
	
	// Remove clock prescaler for full 8MHz operation
	cli();
	CLKPR = 0x80;
	CLKPR = 0x00;
	sei();

	// Load saved frequency value from EEPROM or resort to default frequency
	S32 freq_stored;
	eeprom_read_block((void*) &freq_stored, (const void*) &freq_eeprom, sizeof(S32));
	if((freq_stored > MIN_FREQUENCY) && (freq_stored < MAX_FREQUENCY)) {
		freq_new = freq_cur = freq_stored;
	} else {
		freq_new = freq_cur = DEF_FREQUENCY;
	}

	// Create ring buffer for receiving key presses
	Buffer_Initialize();

	// Clear the screen and set cursor style
	LCD_Clear();
	_delay_ms(5);
	LCD_SetCursorStyle(1);
	_delay_ms(5);

	// SET>
	C8 set_new[] = "SET";
	strcat(set_new, arrow);
	LCD_SendData(0, 1, (U8*) set_new, strlen((C8*) set_new));
	_delay_ms(5);

	// Initialize PLL chip
	//   - Enable digital lock detection
	//   - Positive VCO tuning characteristic
	PLL_write_word(21, PLL_LATCH_INIT | (1L << 4) | (1L << 7));
	PLL_latch_data();

	//    R divider = 10 MHz / 250 kHz (40)
	//    Lock detection period = five consecutive reference cycles
	PLL_write_word(21, PLL_LATCH_R | (R << 2) | (1L << 20));
	PLL_latch_data();

	// Wait 2 seconds for hardware and supplies to stabilize
	_delay_ms(2000);

	for(;;) {
		// Poll the lock-detect input and display status
		if (PLL_locked()) {
		  	LCD_SendData(15, 0, (U8*) locked, strlen((C8*) locked));
		} else {
			LCD_SendData(15, 0, (U8*) not_locked, strlen((C8*) not_locked));
		}

      	if(Buffer_Elements() > 0) {
			U8 key_press = Buffer_Remove();

			// Left arrow button pressed
			if(key_press == KEY_LEFT_PRESS) {

				// Move cursor to the left one block
				curs_pos--;
				if(curs_pos < 0) {
					curs_pos += 4;
				}

			// Right arrow button pressed
			} else if(key_press == KEY_RIGHT_PRESS) {

				// Move cursor to the right one block
				curs_pos++;
				if(curs_pos > 3) {
					curs_pos -= 4;
				}

			// Up arrow button pressed
			} else if(key_press == KEY_UP_PRESS) {

				// Increment the corresponding frequency digit
				if(curs_pos == 0) {
					freq_new += 1000;
				} else if(curs_pos == 1) {
					freq_new += 100;
				} else if(curs_pos == 2) {
					freq_new += 10;
				} else if(curs_pos == 3) {
					freq_new += 1;
				}
			
			// Down arrow button pressed
			} else if(key_press == KEY_DOWN_PRESS) {

				// Decrement the corresponding frequency digit
				if(curs_pos == 0) {
					freq_new -= 1000;
				} else if(curs_pos == 1) {
					freq_new -= 100;
				} else if(curs_pos == 2) {
					freq_new -= 10;
				} else if(curs_pos == 3) {
					freq_new -= 1;
				}
			
			// Enter button pressed
			} else if(key_press == KEY_ENTER_PRESS) {

				// Make sure frequency is within the allowed frequency range
				if((freq_new > MIN_FREQUENCY) && (freq_new < MAX_FREQUENCY)) {
				
					// Only make changes if frequency differs from last setting
					if (freq_cur != freq_last) {
						// Send new frequency to the synthesizer
						PLL_set_MHz(freq_cur);
						freq_last = freq_cur;
						freq_cur = freq_new;

						// Write the new frequency to the EEPROM
						eeprom_write_block((const void*) &freq_cur, (void*) &freq_eeprom, sizeof(S32));
      				}
				}

			// Exit button pressed
			} else if(key_press == KEY_EXIT_PRESS) {

				// Reset the modified frequency
				freq_new = freq_cur;

			}
		}

		// Update top row of display
		C8 top_row[12];
		ltoa(freq_cur, top_row, 10);
		strcat(top_row, (C8*) "MHz");
		LCD_SendData(0, 0, (U8*) top_row, strlen((C8*) top_row));
		_delay_ms(5);

		// Update second row of display
		C8 bot_row[12];
		ltoa(freq_new, bot_row, 10);
		strcat(bot_row, (C8*) "MHz"); 
		LCD_SendData(4, 1, (U8*) bot_row, strlen((C8*) bot_row));
		_delay_ms(5);

		//C8 elements[3];
		//ltoa(ringElements, elements, 10);
		//LCD_SendData(10, 0, (U8*) elements, strlen((C8*) elements));
		//_delay_ms(20);

		LCD_SetCursorPosition(curs_pos + 4, 1);
		_delay_ms(5);
	}
	

	return 0;
}

