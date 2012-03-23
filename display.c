/*
 * Christopher Hewitt (hewittc@jlab.org)
 * 
 * display.c: CrystalFontz CFA-633 LCD Control for AT90USB1287
 * AVR Studio 4
 *
 * Version 2009.06.24
 * 2009 Jefferson Lab
 */

#include "typedefs.h"
#include "main.h"

/* Initialize the USART */
void USART_Initialize(void) {

	// Set baud rate and enable TX/RX
	#if BAUD_RATE == 19200
		UBRR1H = (U8) 0;
		UBRR1L = (U8) 25;
		
	#else
		#warning "Using 115200 baud. Verify display configuration."
		UBRR1H = (U8) 0;
		UBRR1L = (U8) 8;
		UCSR1A = (1 << U2X1);
	#endif

	// Enable TX and RX
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);

	// Set frame format: 8 data, 1 stop bit
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);

}

/* 
 * This code is from the IRDA LAP documentation, which appears to have been copied from PPP:
 * http://irda.affiniscape.com/associations/2494/files/Specifications/IrLAP11_Plus_Errata.zip 
 */
U16 get_crc(U8 *command, U8 len) {

	// CRC lookup table to avoid bit-shifting loops
	static const U16 crcLookupTable[256] =
		{0x00000,0x01189,0x02312,0x0329B,0x04624,0x057AD,0x06536,0x074BF,
		0x08C48,0x09DC1,0x0AF5A,0x0BED3,0x0CA6C,0x0DBE5,0x0E97E,0x0F8F7,
		0x01081,0x00108,0x03393,0x0221A,0x056A5,0x0472C,0x075B7,0x0643E,
		0x09CC9,0x08D40,0x0BFDB,0x0AE52,0x0DAED,0x0CB64,0x0F9FF,0x0E876,
		0x02102,0x0308B,0x00210,0x01399,0x06726,0x076AF,0x04434,0x055BD,
		0x0AD4A,0x0BCC3,0x08E58,0x09FD1,0x0EB6E,0x0FAE7,0x0C87C,0x0D9F5,
		0x03183,0x0200A,0x01291,0x00318,0x077A7,0x0662E,0x054B5,0x0453C,
		0x0BDCB,0x0AC42,0x09ED9,0x08F50,0x0FBEF,0x0EA66,0x0D8FD,0x0C974,
		0x04204,0x0538D,0x06116,0x0709F,0x00420,0x015A9,0x02732,0x036BB,
		0x0CE4C,0x0DFC5,0x0ED5E,0x0FCD7,0x08868,0x099E1,0x0AB7A,0x0BAF3,
		0x05285,0x0430C,0x07197,0x0601E,0x014A1,0x00528,0x037B3,0x0263A,
		0x0DECD,0x0CF44,0x0FDDF,0x0EC56,0x098E9,0x08960,0x0BBFB,0x0AA72,
		0x06306,0x0728F,0x04014,0x0519D,0x02522,0x034AB,0x00630,0x017B9,
		0x0EF4E,0x0FEC7,0x0CC5C,0x0DDD5,0x0A96A,0x0B8E3,0x08A78,0x09BF1,
		0x07387,0x0620E,0x05095,0x0411C,0x035A3,0x0242A,0x016B1,0x00738,
		0x0FFCF,0x0EE46,0x0DCDD,0x0CD54,0x0B9EB,0x0A862,0x09AF9,0x08B70,
		0x08408,0x09581,0x0A71A,0x0B693,0x0C22C,0x0D3A5,0x0E13E,0x0F0B7,
		0x00840,0x019C9,0x02B52,0x03ADB,0x04E64,0x05FED,0x06D76,0x07CFF,
		0x09489,0x08500,0x0B79B,0x0A612,0x0D2AD,0x0C324,0x0F1BF,0x0E036,
		0x018C1,0x00948,0x03BD3,0x02A5A,0x05EE5,0x04F6C,0x07DF7,0x06C7E,
		0x0A50A,0x0B483,0x08618,0x09791,0x0E32E,0x0F2A7,0x0C03C,0x0D1B5,
		0x02942,0x038CB,0x00A50,0x01BD9,0x06F66,0x07EEF,0x04C74,0x05DFD,
		0x0B58B,0x0A402,0x09699,0x08710,0x0F3AF,0x0E226,0x0D0BD,0x0C134,
		0x039C3,0x0284A,0x01AD1,0x00B58,0x07FE7,0x06E6E,0x05CF5,0x04D7C,
		0x0C60C,0x0D785,0x0E51E,0x0F497,0x08028,0x091A1,0x0A33A,0x0B2B3,
		0x04A44,0x05BCD,0x06956,0x078DF,0x00C60,0x01DE9,0x02F72,0x03EFB,
		0x0D68D,0x0C704,0x0F59F,0x0E416,0x090A9,0x08120,0x0B3BB,0x0A232,
		0x05AC5,0x04B4C,0x079D7,0x0685E,0x01CE1,0x00D68,0x03FF3,0x02E7A,
		0x0E70E,0x0F687,0x0C41C,0x0D595,0x0A12A,0x0B0A3,0x08238,0x093B1,
		0x06B46,0x07ACF,0x04854,0x059DD,0x02D62,0x03CEB,0x00E70,0x01FF9,
		0x0F78F,0x0E606,0x0D49D,0x0C514,0x0B1AB,0x0A022,0x092B9,0x08330,
		0x07BC7,0x06A4E,0x058D5,0x0495C,0x03DE3,0x02C6A,0x01EF1,0x00F78};

	U16 newCrc = 0xFFFF;

	// This algorithm is based on the IrDA LAP example
	while(len--)
		newCrc = (newCrc >> 8) ^ crcLookupTable[(newCrc ^ *command++) & 0xFF];

	// Make this crc match the one�s complement that is sent in the packet
	return(~newCrc);

}

/* Send packet over USART */
void USART_Transmit(U8 *command, U8 len) {

	// Compute CRC
	U16 CRC = get_crc(command, len);

	// Send command
	for(int i = 0; i < len; i++) {
		// Wait for empty transmit buffer
		while (!(UCSR1A & (1 << UDRE1))) { }
		// Put data into buffer, sends the data
		UDR1 = command[i];
	}
	
	// Wait for empty transmit buffer
	while (!(UCSR1A & (1 << UDRE1))) { }
	// Send CRC (LSB)
	UDR1 = CRC & 0xFF;

	// Wait for empty transmit buffer
	while (!(UCSR1A & (1 << UDRE1))) { }
	// Send CRC (MSB)
	UDR1 = CRC >> 8;

}

/* Receive a packet over USART */
U8 USART_Receive(void) {

	// Wait for data to be received
	while (!(UCSR1A & (1 << RXC1))) { }

	// Get and return received data from buffer
	return UDR1;

}

/*
 * Command 0: Ping Command
 *
 * data: Data to send
 * len:  Length of data (0-16 valid)
 */
void LCD_Ping(U8 *data, U8 len) {

	U8 command[len + 2];
	command[0] = 0;
	command[1] = len;
	for(int i = 0; i < len; i++) {
		command[i + 2] = data[i];
	}
	USART_Transmit(command, len + 2);

}

/*
 * Command 1: Get Hardware & Firmware Version
 */
void LCD_Version(void) {

	U8 command[2];
	command[0] = 1;
	command[1] = 0;
	USART_Transmit(command, 2);

}

/*
 * Command 6: Clear LCD Screen
 */
void LCD_Clear(void) {

	U8 command[2];
	command[0] = 6;
	command[1] = 0;
	USART_Transmit(command, 2);

}

/*
 * Command 7: Set LCD Contents, Line 1
 *
 * data: Data to send (must be 16 characters)
 */
void LCD_SetLine1(U8 *data) {

	U8 command[18];
	command[0] = 7;
	command[1] = 16;
	for(int i = 0; i < 16; i++) {
		command[i + 2] = data[i];
	}
	USART_Transmit(command, 18);

}

/*
 * Command 8: Set LCD Contents, Line 2
 *
 * data: Data to send (must be 16 characters)
 */
void LCD_SetLine2(U8 *data) {

	U8 command[18];
	command[0] = 8;
	command[1] = 16;
	for(int i = 0; i < 16; i++) {
		command[i + 2] = data[i];
	}
	USART_Transmit(command, 18);

}

/*
 * Command 11: Set LCD Cursor Position
 *
 * column: Column to move cursor to (0-19 valid)
 * row:    Row to move cursor to (0-3 valid)
 */
void LCD_SetCursorPosition(U8 column, U8 row) {

	U8 command[4];
	command[0] = 11;
	command[1] = 2;
	command[2] = column;
	command[3] = row;
	USART_Transmit(command, 4);

}

/*
 * Command 12: Set LCD Cursor Style
 * 
 * style: Cursor style (0-3 valid)
 *          0 = no cursor
 *          1 = blinking block cursor
 *          2 = underscore cursor
 *          3 = blinking block plus underscore
 */
void LCD_SetCursorStyle(U8 style) {

	U8 command[3];
	command[0] = 12;
	command[1] = 1;
	command[2] = style;
	USART_Transmit(command, 3);

}

/*
 * Command 13: Set LCD Contrast
 *
 * contrast: Contrast to set (0-50 valid)
 *             0 = light       (Vlcd=4v)
 *            16 = about right (Vlcd=5v)
 *            29 = dark        (Vlcd=6v)
 *         30-50 = very dark   (Vlcd=6.0 to 6.7v)
 *                             (may be useful at cold temperatures)
 */
void LCD_SetContrast(U8 contrast) {

	U8 command[3];
	command[0] = 13;
	command[1] = 1;
	command[2] = contrast;
	USART_Transmit(command, 3);

}

/*
 * Command 14: Set LCD & Keypad Backlight
 *
 * brightness: Backlight power setting (0-100 valid)
 *               0 = off
 *            1-99 = variable brightness
 *             100 = on
 */
void LCD_SetBrightness(U8 brightness) {

	U8 command[3];
	command[0] = 14;
	command[1] = 1;
	command[2] = brightness;
	USART_Transmit(command, 3);

}

/* 
 * Command 31: Send Data to LCD 
 *
 * x: X coordinate
 * y: Y coordinate 
 * data: Data to send
 */
void LCD_SendData(U8 x, U8 y, U8 *data, U8 len) {

	U8 command[len + 4];
	command[0] = 31;
	command[1] = len + 2;
	command[2] = x;
	command[3] = y;
	for(int i = 0; i < len; i++) {
		command[i + 4] = data[i];
	}
	USART_Transmit(command, len + 4);

}

/*
 * Ring buffer for USART data 
 */
volatile U8 *setAddr;
volatile U8 *getAddr;
volatile U8 ringBuffer[BUFF_SIZE];
volatile U8 ringBufferError;
volatile U8 ringElements;

/* Create ring buffer */
void Buffer_Initialize(void) {

	// IN pointer to start of buffer
	setAddr = (U8*) &ringBuffer;
	// OUT pointer to start of buffer
	getAddr = (U8*) &ringBuffer;
	// Reset the number of elements in buffer
	ringElements = 0;

}

/* Add a byte of data to the ring buffer */
void Buffer_Add(U8 data) {

	// Raise error flag if buffer is full
	if(ringElements == BUFF_SIZE) {
		ringBufferError |= BUFF_ERR_OVERFLOW;
		return;
	} else {
		ringBufferError = 0;
	}
	
	// Add data to buffer
	*setAddr = data;

	// Increment IN pointer and number of elements
	setAddr++;
	ringElements++;

	// Wrap pointer if end of array reached
	if (setAddr == (U8*) &ringBuffer[BUFF_SIZE])
		setAddr = (U8*) &ringBuffer; 

}	

/* Remove a byte of data from the ring buffer */
U8 Buffer_Remove(void) {

	// Raise error flag if buffer is empty
	if(!(ringElements)) {
		ringBufferError |=  BUFF_ERR_EMPTY;
		return 0;
	} else {
		ringBufferError = 0;
	}

	// Grab the stored byte into a temp variable
	U8 data = *getAddr; 

	// Increment OUT pointer and decrement number of elements
	getAddr++;
	ringElements--;
	
	// Wrap pointer if end of array reached
	if (getAddr == (U8*) &ringBuffer[BUFF_SIZE])
		getAddr = (U8*) &ringBuffer;

	return data;    
}

U8 Buffer_Elements(void) {

	return ringElements;

}

/* Execute with every USART receive */
ISR(USART1_RX_vect) {

	static U8 state;
	static U8 count;
	static U8 packet_command;
	static U8 packet_count;
	static U8 packet_data[18];
	static U16 packet_crc;

	// Packet Command byte
	if(state == 0) {
		packet_command = UDR1;
		state = 1;

	// Packet Length byte
	} else if(state == 1) {
		packet_count = UDR1;
		if(packet_count == 0) {
			state = 3;
		} else {
			count = 0;
			state = 2;
		}

	// Packet Data bytes
	} else if(state == 2) {

		packet_data[count] = UDR1;
		count++;
		if (count == packet_count) {
			state = 3;
		}

	// Packet CRC byte (LSB)
	} else if(state == 3) {

		packet_crc = (UDR1 << 8);
		state = 4;

	// Packet CRC byte (MSB)
	}  else {

		packet_crc |= UDR1;

		// Add to key press queue if packet is key press
		if((packet_command == 0x80) && (packet_count == 0x01) 
			&& (packet_data[0] > 0x00) && (packet_data[0] < 0x07)) {
				Buffer_Add(packet_data[0]);
		}
		state = 0;

	}

}
