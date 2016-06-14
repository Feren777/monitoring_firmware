/*
 * Copyright (c) 2009 by Michael Stapelberg <michael+es@stapelberg.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */
 
#include "config.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "em4095.h"

#define MANCHESTER_LONG (F_CPU / F_RFID * 64 / TIMER0_PRESCALER)
#define MANCHESTER_SHORT (F_CPU / F_RFID * 64 / TIMER0_PRESCALER / 2)
#define MANCHESTER_MIDDLE (MANCHESTER_LONG + MANCHESTER_SHORT) / 2


#ifdef DEBUG_EM4095
# include "core/debug.h"
# define EM4095DEBUG(a...) debug_printf("em4095: " a)
# define EM4095DEBUG2(a...) debug_printf(a)
#else
# define EM4095DEBUG(a...) do { } while(0)
#endif /* DEBUG_RFID */



static unsigned char rfid_data[8];
static volatile uint8_t rfid_flag;

// Connections to RFID Reader

// DEMOD to PE4
// SHD	 to GND


void em4095_init(void){

	//EM_DDR |= ( (1<<EM_MOD) | (1<<EM_SHD) | (1<<EM_LED) );		// MOD, SHD and LED to OUTPUT

	//EM_PORT &= ~(1<<EM_LED);	// LED LOW
	//EM_PORT &= ~(1<<EM_SHD);	// SHD LOW

	//TCCR0 = 1<<CS01 | 1<<CS00; 	//Timer/Counter0 prescaler 64
	TCCR0 = 1<<CS02;				// Prescaler to 64 on ATmega128
	//TC0_PRESCALER_64;

	//EICRB |= 1<<ISC40; 		//Any logical change on INT4 generates an interrupt request
	//EIMSK |= 1<<INT4; 		//External interrupt request 4 enable (demod_out from RFID chip to PE4)
	//sei();

#if defined(RFID_PCINT_PIN)
	EM4095DEBUG("configure_pcint\n");
	/* configure */
	rfid_configure_pcint();
#elif defined(RFID_INT_PIN)
	EM4095DEBUG("HAVE_RFID_INT: %d \n", RFID_INT_PIN);
	/* Initialize "real" Interrupt */
	_EIMSK |= _BV(RFID_INT_PIN);
	EICRB = (EICRB & ~RFID_INT_ISCMASK) | RFID_INT_ISC;
#endif
	EM4095DEBUG("RFID_MANCHESTER: %d \n", MANCHESTER_MIDDLE);
}


ISR(RFID_VECTOR){

	static uint16_t sync;
	static uint8_t bitcount;
	static uint8_t insync = 0;
	static uint8_t old_cnt;

	//PIN_TOGGLE(RFID_SHD);

	if ((uint8_t)(TC0_COUNTER_CURRENT - old_cnt) > MANCHESTER_MIDDLE)
	{
		old_cnt = TC0_COUNTER_CURRENT;

		sync <<= 1;
		bitcount++;

		// bit_is_clear(PINE, PE4) ?
		if ( !PIN_HIGH(RFID_DEMOD) )
		{		
			sync |= 1;
		}

		if ((sync & 0x3FF) == 0x1FF)
		{
			rfid_data[0] = 0xFF;
			bitcount = 1;
			insync = 1;
		}

		if (insync)
		{
			if ((bitcount % 8) == 0)
			{
				rfid_data[bitcount / 8] = sync & 0xFF;

				if (bitcount == 56)
				{
					rfid_flag = 1;
					insync = 0;
					EIMSK &= ~(1<<RFID_INT_PIN);
				}
			}
		}
	}
}

static uint8_t em4095_check_parity(uint16_t param){

	uint8_t bit;
	uint8_t par = 0;

	for (bit=0; bit<16; bit++)
		par ^= (param >> bit) & 0x01;

	if (par)
		return 0;
	else
		return 1;
}


uint8_t em4095_read_tag(uint8_t *d){

	uint8_t timeout = 50;

	/**
	do
	{
		if (bit_is_set(TIFR, TOV0))
		{
			timeout--;
			TC0_INT_OVERFLOW_CLR
		}

	} while ((!rfid_flag) && (timeout > 0));
	**/

	if ( (!rfid_flag) && (timeout > 0) )
	{

		if (bit_is_set(TIFR, TOV0))
		{
			timeout--;
			TC0_INT_OVERFLOW_CLR
		}

		return 0;
	}


	rfid_flag = 0;
	uint8_t temp[8];
	memcpy(temp, rfid_data, 8);
	memset(rfid_data, 0, 8);

	EIMSK |= 1<<RFID_INT_PIN;

	if (((temp[0] << 8 | temp[1]) & 0xFF80) != 0xFF80) //Check startbits
		return 0;

	if ((temp[7] & 0x01)) //Check stopbit
		return 0;

	if (!em4095_check_parity(temp[1] & 0x7C))
		return 0;

	if (!em4095_check_parity((temp[1] & 0x03) | (temp[2] & 0xE0)))
		return 0;

	if (!em4095_check_parity(temp[2] & 0x1F))
		return 0;

	if (!em4095_check_parity(temp[3] & 0xF8))
		return 0;

	if (!em4095_check_parity((temp[3] & 0x07) | (temp[4] & 0xC0)))
		return 0;

	if (!em4095_check_parity(temp[4] & 0x3E))
		return 0;

	if (!em4095_check_parity((temp[4] & 0x01) | (temp[5] & 0xF0)))
		return 0;

	if (!em4095_check_parity((temp[5] & 0x0F) | (temp[6] & 0x80)))
		return 0;

	if (!em4095_check_parity(temp[6] & 0x7C))
		return 0;

	if (!em4095_check_parity((temp[6] & 0x03) | (temp[7] & 0xE0)))
		return 0;

	if (!em4095_check_parity((temp[1] & 0x40) ^ (temp[1] & 0x02) ^ (temp[2] & 0x10) ^ (temp[3] & 0x80) ^ (temp[3] & 0x04) ^
									  (temp[4] & 0x20) ^ (temp[4] & 0x01) ^ (temp[5] & 0x08) ^ (temp[6] & 0x40) ^ (temp[6] & 0x02) ^
									  (temp[7] & 0x10)))
		return 0;

	if (!em4095_check_parity((temp[1] & 0x20) ^ (temp[1] & 0x01) ^ (temp[2] & 0x08) ^ (temp[3] & 0x40) ^ (temp[3] & 0x02) ^
									  (temp[4] & 0x10) ^ (temp[5] & 0x80) ^ (temp[5] & 0x04) ^ (temp[6] & 0x20) ^ (temp[6] & 0x01) ^
									  (temp[7] & 0x08)))
		return 0;

	if (!em4095_check_parity((temp[1] & 0x10) ^ (temp[2] & 0x80) ^ (temp[2] & 0x04) ^ (temp[3] & 0x20) ^ (temp[3] & 0x01) ^
									  (temp[4] & 0x08) ^ (temp[5] & 0x40) ^ (temp[5] & 0x02) ^ (temp[6] & 0x10) ^ (temp[7] & 0x80) ^
									  (temp[7] & 0x04)))
		return 0;

	if (!em4095_check_parity((temp[1] & 0x08) ^ (temp[2] & 0x40) ^ (temp[2] & 0x02) ^ (temp[3] & 0x10) ^ (temp[4] & 0x80) ^
									  (temp[4] & 0x04) ^ (temp[5] & 0x20) ^ (temp[5] & 0x01) ^ (temp[6] & 0x08) ^ (temp[7] & 0x40) ^
									  (temp[7] & 0x02)))
		return 0;

//11111111 11000110 00101111 10001100 01010011 00010000 00100101 00110110
//          DDDD DD DD DDDD  DDDD DDD D DDDD D DDD DDDD  DDDD DD DD DDDD
//          0000 00 00 1111  1111 222 2 2222 3 333 3333  4444 44 44

	d[0] = (temp[1] & 0x70) << 1 | (temp[1] & 0x03) << 2 | (temp[2] & 0xC0) >> 6;
	d[1] = (temp[2] & 0x1E) << 3 | (temp[3] & 0xF0) >> 4;
	d[2] = (temp[3] & 0x07) << 5 | (temp[4] & 0x80) >> 3 | (temp[4] & 0x3C) >> 2;
	d[3] = (temp[4] & 0x01) << 7 | (temp[5] & 0xE0) >> 1 | (temp[5] & 0x0F);
	d[4] = (temp[6] & 0x78) << 1 | (temp[6] & 0x03) << 2 | (temp[7] & 0xC0) >> 6;

	return 1;
}

