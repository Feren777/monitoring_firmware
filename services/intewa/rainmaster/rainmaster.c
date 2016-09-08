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

#include "rainmaster.h"
//#include "rainmaster_core.h"
#include "rainmaster_menu.h"
//#include "rainmaster_sensors.h"
//#include "rainmaster_net.h"
//#include "rainmaster_gui.h"

#include "core/eeprom.h"
#include "protocols/ecmd/ecmd-base.h"
#include "services/lcd-menu/lcd_menu.h"
#include "hardware/input/buttons/buttons.h"
#include "hardware/lcd/hd44780.h"

#include "hardware/pwm/pwm.h"


#include "services/clock/clock.h"
#include "hardware/adc/adc.h"
#include "hardware/onewire/onewire.h"



clock_datetime_t date;

rainmaster_params_t rainmaster_params_ram;


volatile timestamp_t capture_ts;
volatile timestamp_t get_ts;
volatile uint16_t capt_adc_levelsensor;

uint16_t my_tanklevel = 0;

rm_statusbits_t rm_status = { .PumpenKontakt = false, .SchwimmSchalter = false };    // Eigene Zustände

rm_device_t rm_001 = { .device_id = 101, .request_state = 0 };           // Gerät Nr. 1
rm_device_t rm_002 = { .device_id = 102, .request_state = 0 };           // Gerät Nr. 2
rm_device_t rm_003 = { .device_id = 103, .request_state = 0 };           // Gerät Nr. 3




uint16_t rm_offcounter;

volatile uint8_t rainmaster_state = RAINMASTER_STATE_IDLE;


uint8_t ecmd_callback_blocking;






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

      case BTN_NEXT:  next_func();   break;   /* Pointer to function executing default next step defined in menu */
      case BTN_ENTER: enter_func();  break;   /* Pointer to function executing default enter sequence defined in menu */

      case SW1_1: break;
      case SW1_2: break;
   
      //case BTN_NEXT: gui_screen_set(AL_MENU_TOGGLE); break;
      //case BTN_ENTER: break;
    }

  }

}


uint16_t rainmaster_get_levelsensor(void)
{
  uint16_t adc;
  uint16_t level_average = 0;
  
  int8_t i;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    capture_ts = clock_get_time();
    get_ts = capture_ts;
    
    for (i=0; i<32; i++) {
      capt_adc_levelsensor = adc_get(RAINMASTER_LEVEL_SENSOR);
      
      level_average += capt_adc_levelsensor;
    }

  }

  adc = level_average / 32L;

  //long adc_waterlevel = (((long)(adc) * 5080)/1024L);
  //return (uint16_t)( adc_waterlevel );

  return adc;
}




void rainmaster_request_callback(char* text, uint8_t len)
{
  //RAINMASTERDEBUG ("%s\n", text);
  //RAINMASTERDEBUG ("%d\n", len);
  char results[12];

  uint8_t id = 0;
  uint16_t value = 0;
  uint8_t busy = 0;

  if(ecmd_callback_blocking == 1) {

    // Erst die ID parsen
    strncpy(results, &text[0], 3);
    results[3] = '\n';
    id = strtol(results, NULL, 10);
    //RAINMASTERDEBUG ("%d\n", id);
    memset(results, 0, sizeof(results));

    // Jetzt den Wert
    strncpy(results, &text[4], 4);
    results[8] = '\n';
    value = strtol(results, NULL, 10);
    //RAINMASTERDEBUG ("%d\n", value);
    memset(results, 0, sizeof(results));

    // Jetzt den Status
    strncpy(results, &text[9], 1);
    results[10] = '\n';
    busy = strtol(results, NULL, 10);
    //RAINMASTERDEBUG ("%d\n", value);
    memset(results, 0, sizeof(results));

    switch(id) {
      case RM_ID1:
        rm_001.device_id = id;
        rm_001.tanklevel = value;
        rm_001.busy = busy;
        break;

      case RM_ID2:
        rm_002.device_id = id;
        rm_002.tanklevel = value;
        rm_002.busy = busy;
        break;

      case RM_ID3:
        rm_003.device_id = id;
        rm_003.tanklevel = value;
        rm_003.busy = busy;
        break;

      default:
        break;

    }

    //RAINMASTERDEBUG ("%d %d %d\n", id, value, busy);

    ecmd_callback_blocking = 0;
  }
  
  //RAINMASTERDEBUG ("cb\n");
}




uint16_t rainmaster_get_tanklevel(void)
{

  uint32_t level_value;   // Raw ADC value from 0 to 1023
  uint32_t level_percent; // Expressed in Percent / Permill (0-1000%)

  level_value = (uint32_t)rainmaster_get_levelsensor();

  if (level_value > rainmaster_params_ram.levl_tank_max) level_value = rainmaster_params_ram.levl_tank_max;
  if (level_value < rainmaster_params_ram.levl_tank_min) level_value = rainmaster_params_ram.levl_tank_min;

  level_percent = (((level_value - rainmaster_params_ram.levl_tank_min) * 1000U) / (rainmaster_params_ram.levl_tank_max - rainmaster_params_ram.levl_tank_min));

  return (uint16_t)( level_percent ); 
}



void rainmaster_check_inputs(void) {

  //if(!PIN_HIGH(RM_POSITION1))

	if(!PIN_HIGH(RM_PUMPKONTAKT)) rm_status.PumpenKontakt   = true; else rm_status.PumpenKontakt   = false;    // Anschluss AL_SSKLARMIN
	// if(!PIN_HIGH(RM_SCHWIMMER))   rm_status.SchwimmSchalter = true; else rm_status.SchwimmSchalter = false;    // Schwimmer AL_SSKLARMAX
	// if(!PIN_HIGH(RM_POSITION1))   rm_status.Pos1Schalter    = true; else rm_status.Pos1Schalter    = false;

	if(rm_status.PumpenFlag == true) rm_offcounter++; else rm_offcounter = 0;

  my_tanklevel = rainmaster_get_tanklevel();
}


/*
  If enabled in menuconfig, this function is called during boot up of ethersex
*/
int16_t rainmaster_init(void) {
  RAINMASTERDEBUG ("init\n");

  // Handler für Taster initialisieren und gemäß Menu zuweisen
  hook_buttons_input_register(default_button_handler);
  
  // LCD Menu initialisieren und Startbildschirm anzeigen
  init_menu();
  
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

  // Druckabfall in Leitung erkennen und Pumpe bei Bedarf einschalten
  if(rm_status.PumpenKontakt == true) {
    setpwm('a', 255);             // Pumpe einschalten
    rm_status.PumpenFlag = true;  // Pumpenflag setzen
  } else {
    setpwm('a', 0);                // Pumpe ausschalten
    rm_status.PumpenFlag = false;  // PumpenFlag setzen
    rm_offcounter = 0;             // Pumpe läuft nicht, also counter auf 0  
  }


  uip_ipaddr_t ip_addr;


  if(my_tanklevel < TANK_MIN) {

    // TODO:
    uip_ipaddr(ip_addr, 192, 168, 1, 101);

    // Kugelhahn umstellen auf Trinkwasserbetrieb
    PIN_CLEAR(RM_KUGELHAHN);
    rm_status.Kugelhahn = 0; 

    // Eigenes Magnetventil schließen
    setpwm('c', 0);
    rm_status.Magnetventil = 0;

    // TODO:
    if(rm_001.request_state == 0) {
      // Fremdgerät kann Magnetventil öffnen u Pumpe starten, falls kein Eigenbedarf u genügend Wasser bei sich
      //ecmd_sender_send_command_P(&ip_addr, NULL, PSTR("rainmaster_request 1\n"));
      rm_001.request_state = 1;
      //rm_001.busy = 1;
    }

  }


  if(my_tanklevel > TANK_MAX) {

      if(rm_001.request_state == 1) {
        // Fremdgerät kann Magnetventil öffnen u Pumpe starten, falls kein Eigenbedarf u genügend Wasser bei sich
        //ecmd_sender_send_command_P(&ip_addr, NULL, PSTR("rainmaster_request 1\n"));
        rm_001.request_state = 0;
      }

      // Kugelhahn umstellen auf Regenwasserbetrieb
      PIN_SET(RM_KUGELHAHN);
      rm_status.Kugelhahn = 1; 

    }


} /* End rainmaster_mainloop */




/* Wird einmal pro Sekunde aufgerufen */
void rainmaster_long_timer(void) {

  static uint16_t lt_counter = 0;

  lt_counter++;

  /* alle 5 Sekunden */
  if (lt_counter % 5 == 0) {
   //RAINMASTERDEBUG ("%d %d\n", rainmaster_params_ram.levl_tank_min, rainmaster_params_ram.levl_tank_max);
   //RAINMASTERDEBUG ("%d\n", my_tanklevel);

  }

  /* ca alle 10 Minuten */
  if (lt_counter % 600 == 0) {
    lt_counter = 0;
  }


}




/* TODO:

- Pumpe anschließen an PWM
- Kugelhahn anschließen an PWM
- Magnetventil anschließen an PWM

- Druckschalter an EINGANG
- 


*/

 /*
  -- Ethersex META --
  header(services/intewa/rainmaster/rainmaster.h)
  ifdef(`conf_RAINMASTER_INIT_AUTOSTART',`init(rainmaster_init)')
  ifdef(`conf_RAINMASTER_PERIODIC_AUTOSTART',`timer(10,rainmaster_periodic())')
  timer(50, rainmaster_long_timer());
*/
