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
 
#ifndef _EM4095_H_
#define _EM4095_H_

#include "config.h"



//#define F_RFID 			125000UL		//RFID carrier frequency
#define F_RFID 				123966UL		//RFID carrier frequency
#define TIMER0_PRESCALER 	64 				//Needed for manchester decoding


//#define EM_DDR		DDRE
//#define EM_LED		PE0
//#define EM_RDY		PE1
//#define EM_SHD		PE2
//#define EM_MOD		PE3
//#define EM_DEMOD		PE4



void em4095_init(void);
uint8_t em4095_read_tag(uint8_t *d);



#endif /* EM4095_H_ */
