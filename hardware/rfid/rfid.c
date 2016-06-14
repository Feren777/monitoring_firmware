/*
 * rfid.c
 *
 *  Created on: 19.03.2010
 *      Author: Customer
 */

#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "rfid.h"
#include "em4095.h"


#ifdef DEBUG_RFID
# include "core/debug.h"
# define RFIDDEBUG(a...) debug_printf("rfid: " a)
# define RFIDDEBUG2(a...) debug_printf(a)
#else
# define RFIDDEBUG(a...) do { } while(0)
#endif /* DEBUG_RFID */


unsigned char rfid_tag[5];	// Current RFID Tag
uint8_t rfid_tid;		// Statuscode of Reader
uint8_t last_tag[5];

void rfid_init(void) {
	em4095_init();

}

uint8_t rfid_check_tag(uint8_t *id){

	uint8_t tags_total, sum = 0;
	unsigned char tpos;
	unsigned char tline;

	tags_total = pgm_read_byte(&valid_tags[0]);

	for(tline = 1; tline <= tags_total; tline++){

		sum = 0;
		for(tpos=0; tpos<=4; tpos++){

			if( id[tpos] == pgm_read_byte(&valid_tags[((tline*6)-4)+tpos]) ){
			sum++;
				//LOG_DEBUG("%X %X", id[tpos] , pgm_read_byte(&valid_tags[((tline*6)-4)+tpos]) );
				//LOG_DEBUG("%d %d %d", tline, tpos, sum);
			}

		}

		if(sum == 5){
			return pgm_read_byte(&valid_tags[(tline*6)-5] );
		}
	}
	return 0;
}


void rfid_mainloop(void){

	static uint8_t read_flag = 0;

	// Check for a new tag on reader
	if (em4095_read_tag(rfid_tag)) {

		// Get the ID of the current tag
		rfid_tid = rfid_check_tag(rfid_tag);

		// Compare current with last tag
		if ( (memcmp(last_tag, rfid_tag, sizeof(rfid_tag) )) ) {

			PIN_SET(AL_GUI_PIEZO);
			_delay_ms(1);
			PIN_CLEAR(AL_GUI_PIEZO);

			memcpy(&last_tag, &rfid_tag, sizeof(rfid_tag));

		} 





		if( rfid_tid != 0 ){
			//TODO Hier einbauen, was passieren soll...

			// Letzten Tag speichern

			// Wenn neuer Tag = alter Tag -> nicht mehr fragen
		}

	} else {

			//rfid_tid = 0x00;
			//memset(last_tag, 0, sizeof(rfid_tag));
			//memset(rfid_tag, 0, sizeof(rfid_tag));
			
	}

} 


//timer(100,rfid_clear_tag())
void rfid_clear_tag(void){

		rfid_tid = 0x00;
		memset(last_tag, 0, sizeof(rfid_tag));
		memset(rfid_tag, 0, sizeof(rfid_tag));
}




/*
  -- Ethersex META --
  header(hardware/rfid/rfid.h)
  init(rfid_init)

*/
