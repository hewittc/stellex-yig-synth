/*
 * Christopher Hewitt (hewittc@jlab.org)
 * Original code by John Miles (miles@pop.net)
 * See: http://www.ke5fx.com/stellex.htm
 *
 * synth.c: Control system for Stellex YIG synthesizer
 * AVR Studio 4
 *
 * Version 2009.06.24
 * 2009 Jefferson Lab
 */

#include "typedefs.h"
#include "main.h"

void BUS_write_word(S8 bits, S32 data, U8 data_line, U8 clk_line) {

   // Drop all signals
   PORTB = 0;

   for (S8 i = bits - 1; i >= 0; i--)
   {
      // Shift word into input register, starting with MSB
      // After asserting each bit, toggle CLK briefly to load it
      if (data & (1L << i)) {
         PORTB = data_line;
         PORTB = data_line | clk_line;
         PORTB = data_line;
      } else {
         PORTB = 0;
         PORTB = clk_line;
         PORTB = 0;
      }
   }

   _delay_ms(1);
}

void PLL_write_word(S32 bits, S32 data) {

   BUS_write_word(bits, data, PLL_DATA, PLL_CLK);

}

void PLL_latch_data(void) {

   // Assert LE briefly to update destination register from input buffer
   PORTB = PLL_LATCH;
   PORTB = 0;
   
}

/* Set desired N divisor to determine PLL output frequency */
void PLL_set_N_divisor(S32 N) {

   // Set N divisor, low charge-pump gain
   PLL_write_word(21, PLL_LATCH_N | (N << 2) /* | (1 << 20) */);
   PLL_latch_data();
   
}


/* 
 * Since Fcomp is 250 kHz and each step is multiplied by 4
 * by the external prescaler, the N divisor is just the frequency in MHz
 */
void PLL_set_MHz(S32 MHz) {

   PLL_set_N_divisor(MHz);

}

/* Return TRUE if PLL is locked */
BOOL PLL_locked(void) {

   return (PINB & PLL_LOCKED) != 0;

}

