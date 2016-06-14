/*
 * Copyright (c) 2014 by Harald W. Leschner <leschner@intewa.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#include <stdbool.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "core/bit-macros.h"

//#include "aqualoop.h"
//#include "aqualoop_core.h"
//#include "aqualoop_menu.h"
//#include "aqualoop_sensors.h"
//#include "aqualoop_net.h"
//#include "aqualoop_gui.h"

#include "core/eeprom.h"
#include "protocols/ecmd/ecmd-base.h"
#include "services/lcd-menu/lcd_menu.h"
#include "hardware/input/buttons/buttons.h"
#include "hardware/lcd/hd44780.h"

#include "hardware/pwm/pwm.h"

rm_statusbits_t rm_status;

rainmaster_params_t rainmaster_params_ram;

clock_datetime_t date;

uint16_t rm_offcounter;



//volatile uint8_t rainmaster_state = RAINMASTER_STATE_IDLE;






/*!
 * Standard Pieps 
 */
void default_gui_beep (void) {
  PIN_SET(AL_GUI_PIEZO);
  _delay_ms(1);
  PIN_CLEAR(AL_GUI_PIEZO);
}

/*!
 * Standard Button-Zuordnung bei Initialisierung.
 * \param button Welcher Taster wurde gerade gedrückt.
 * \param status Zustand des Tasters (losgelassen, gedrückt, gehalten, etc..)
 */
void default_button_handler(buttons_ButtonsType button, uint8_t status) {

  if ( (BUTTON_REPEAT == status || BUTTON_PRESS == status) && (button == BTN_NEXT || button == BTN_ENTER) )
  {

    default_gui_beep();

    switch (button)
    {

      case BTN_NEXT:  /*next_func();  */ break;   /* Pointer to function executing default next step defined in menu */
      case BTN_ENTER: /*enter_func(); */ break;   /* Pointer to function executing default enter sequence defined in menu */

      case SW1_1: break;
      case SW1_2: break;
   
      //case BTN_NEXT: gui_screen_set(AL_MENU_TOGGLE); break;
      //case BTN_ENTER: break;
    }

  }

}



void rainmaster_check_inputs(void) {

	if(!PIN_HIGH(RM_PUMPKONTAKT)) rm_status.PumpenKontakt   = true; else rm_status.PumpenKontakt   = false;
	if(!PIN_HIGH(RM_SCHWIMMER))   rm_status.SchwimmSchalter = true; else rm_status.SchwimmSchalter = false;
	if(!PIN_HIGH(RM_POSITION1))   rm_status.Pos1Schalter    = true; else rm_status.Pos1Schalter    = false;

	if(rm_status.PumpenFlag == true) rm_offcounter++; else rm_offcounter = 0;
}


/*
  If enabled in menuconfig, this function is called during boot up of ethersex
*/
int16_t rainmaster_init(void) {
  RAINMASTERDEBUG ("init\n");

  // Handler für Taster initialisieren und gemäß Menu zuweisen
  hook_buttons_input_register(default_button_handler);
  
  // LCD Menu initialisieren und Startbildschirm anzeigen
  //init_menu();
  
  // Sensoren initialisieren
  //init_sensors();
  
  // Parameter aus dem EEPROM wiederherstellen
  eeprom_restore(rainmaster_params, &rainmaster_params_ram, sizeof(rainmaster_params_t));

  // Init all standard pumping cycles
  //aqualoop_pumps_cycle_init();

  // Init chemical cleaning cycle
  //aqualoop_cleaning_cycle_init();

  // Init MONITOR networking
  //init_networking(); 

  return ECMD_FINAL_OK;
}


/*
  If enabled in menuconfig, this function is periodically called
  change "timer(100,app_sample_periodic)" if needed

  Wird ca alle 200ms aufgerufen
*/
int16_t rainmaster_periodic(void) {

  // Uhrzeit aktualisieren
  clock_current_localtime(&date);

  // Eingänge und Sensore abfragen
  rainmaster_check_inputs();
  
  // GUI Anzeige abarbeiten (Pointer auf Anzeigefunktion im Menu)
  display_func();

  return ECMD_FINAL_OK;
}


/* Hauptroutine: Wird in der Hauptschleife abgerufen (sobald alle Interrupts und Timer abgearbeitet sind) - as fast as possible */
void rainmaster_mainloop(void) {

  // Menuscreens abfragen, um ggfls. Parameteränderungen vorher zu erfassen (Statusvariablen)
  //al_menu_control();

  if(rm_status.PumpenKontakt == true) {

  	setpwm('a', 255);				// Pumpe einschalten
  	rm_status.PumpenFlag = true;	// Pumpenflag setzen
  	rm_status.Pos1Schalter ? setpwm('b', 255) : setpwm('b', 0);	// Ladepumpe ein bzw. ausschalten

  } else {

  	setpwm('a', 0);					// Pumpe ausschalten
  	rm_status.PumpenFlag = false;	// PumpenFlag setzen
  	setpwm('b', 0);					// Ladepumpe ausschalten
  	rm_offcounter = 0;				// Pumpe läuft nicht, also counter auf 0

  }


  if(rm_status.PumpenFlag == true) {

  	if(rm_offcounter > 14400) {

		setpwm('a', 0);				// Pumpe ausschalten
		setpwm('b', 0);				// Ladepumpe ausschalten

  		while (true) {
		  	PIN_TOGGLE(STATUSLED_HB_ACT);
 			_delay_ms(500);
  		}

  	}

  } else

  	rm_offcounter = 0;


}









 /*
  -- Ethersex META --
  header(services/intewa/rainmaster/rainmaster.h)
  ifdef(`conf_RAINMASTER_INIT_AUTOSTART',`init(rainmaster_init)')
  ifdef(`conf_RAINMASTER_PERIODIC_AUTOSTART',`timer(10,rainmaster_periodic())')
*/

