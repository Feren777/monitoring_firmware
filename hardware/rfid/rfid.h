/*
 * rfid.h
 *
 *  Created on: 19.03.2010
 *      Author: Customer
 */

#ifndef _RFID_H_
#define _RFID_H_

#include "config.h"


extern unsigned char rfid_tag[5];
extern uint8_t rfid_tid;


void rfid_init(void);

uint8_t rfid_check_tag(uint8_t *id);

void rfid_mainloop(void);
void rfid_clear_tag(void);


// List of valid tags

static const unsigned char __attribute__ ((progmem)) valid_tags[] = {

	//  Total number of tags registered in database
		0x03,									// Nr. of lines of Card/Tag list

	//  ID    TAG0  TAG1  TAG2  TAG3  TAG4
	// ----------------------------------------------------
		0x01, 0x01, 0x08, 0x71, 0xFC, 0x98,		// Card Nr. 1 01:08:71:FC:98
		0x02, 0x01, 0x07, 0xAC, 0x05, 0xC6,
		0x03, 0x04, 0x60, 0x17, 0x79, 0x38

};

#endif /* _RFID_H_ */
