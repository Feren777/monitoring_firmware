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

#include <avr/pgmspace.h>
#include <util/atomic.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "aqualoop.h"
#include "aqualoop_core.h"
#include "aqualoop_menu.h"
#include "aqualoop_sensors.h"
#include "aqualoop_net.h"
#include "aqualoop_gui.h"

#include "core/eeprom.h"
#include "protocols/ecmd/ecmd-base.h"
#include "services/lcd-menu/lcd_menu.h"
#include "hardware/input/buttons/buttons.h"
#include "hardware/lcd/hd44780.h"


volatile uint8_t aqualoop_state = AQUALOOP_STATE_IDLE;

clock_datetime_t date;

aqualoop_params_t aqualoop_params_ram;





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

      case BTN_NEXT:  next_func();  break;   /* Pointer to function executing default next step defined in menu */
      case BTN_ENTER: enter_func(); break;   /* Pointer to function executing default enter sequence defined in menu */

      case SW1_1: break;
      case SW1_2: break;
   
      //case BTN_NEXT: gui_screen_set(AL_MENU_TOGGLE); break;
      //case BTN_ENTER: break;
    }

  }

}


/*
  If enabled in menuconfig, this function is called during boot up of ethersex
*/
int16_t aqualoop_init(void) {
  AQUALOOPDEBUG ("init\n");

  // Handler für Taster initialisieren und gemäß Menu zuweisen
  hook_buttons_input_register(default_button_handler);
  
  // LCD Menu initialisieren und Startbildschirm anzeigen
  init_menu();
  
  // Sensoren initialisieren
  init_sensors();
  
  // Parameter aus dem EEPROM wiederherstellen
  eeprom_restore(aqualoop_params, &aqualoop_params_ram, sizeof(aqualoop_params_t));

  // Init all standard pumping cycles
  aqualoop_pumps_cycle_init();

  // Init chemical cleaning cycle
  aqualoop_cleaning_cycle_init();

  // Init MONITOR networking
  init_networking(); 

  return ECMD_FINAL_OK;
}


/*
  If enabled in menuconfig, this function is periodically called
  change "timer(100,app_sample_periodic)" if needed

  Wird ca alle 200ms aufgerufen
*/
int16_t aqualoop_periodic(void) {

  // Uhrzeit aktualisieren
  clock_current_localtime(&date);

  // GUI Anzeige abarbeiten (Pointer auf Anzeigefunktion im Menu)
  display_func();

  return ECMD_FINAL_OK;
}


/* Hauptroutine: Wird in der Hauptschleife abgerufen (sobald alle Interrupts und Timer abgearbeitet sind) - as fast as possible */
void aqualoop_mainloop(void) {

  // Menuscreens abfragen, um ggfls. Parameteränderungen vorher zu erfassen (Statusvariablen)
  al_menu_control();

  // Reinigungszyklus abarbeiten
  aqualoop_cleaning_toggle_core();

  // Membranpumpen abarbeiten
  aqualoop_pumps_toggle_core();

  // Luftpumpe abarbeiten
  aqualoop_air_toggle_core();

  // Schlammpumpe abarbeiten
  aqualoop_sludge_toggle_core();


/**
  switch (aqualoop_state) {

    case AQUALOOP_STATE_AKKU:

    	display_screen = 6;
    	break;

    case AQUALOOP_STATE_PUMP:
      break;

    default:
      aqualoop_state = AQUALOOP_STATE_IDLE;

  }
**/

}


/*
  This function will be called on request by menuconfig, if wanted...
  You need to enable ECMD_SUPPORT for this.
  Otherwise you can use this function for anything you like 
*/
int16_t aqualoop_onrequest(char *cmd, char *output, uint16_t len) {
  AQUALOOPDEBUG ("main\n");
  // enter your code here

  return ECMD_FINAL_OK;
}




/*
  -- Ethersex META --
  header(services/intewa/aqualoop/aqualoop.h)
  ifdef(`conf_AQUALOOP_INIT_AUTOSTART',`init(aqualoop_init)')
  ifdef(`conf_AQUALOOP_PERIODIC_AUTOSTART',`timer(10,aqualoop_periodic())')
*/
