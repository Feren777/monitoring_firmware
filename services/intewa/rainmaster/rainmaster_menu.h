/*
* menu functions of the AQUALOOP Control Unit
* not intended for use from outside the control unit (monitoring)
* main menu functions
*
* Copyright (c) 2014 by Harald W. Leschner <leschner@intewa.de>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
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

#include <stddef.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>


#include "config.h"
#include "core/bit-macros.h"
#include "core/debug.h"
//#include "services/lcd-menu/lcd_menu.h"

//#include "aqualoop.h"
//#include "aqualoop_sensors.h"
//#include "aqualoop_gui.h"




#define AL_MENU_SCREENS 20 			/*!< max. Anzahl an Screens */


#define AL_BASIS_INFO_AVAILABLE
#define AL_MENU_TIME_AVAILABLE			/*!< zeigt die Uhrzeit an */
#define AL_MENU_AKKU_AVAILABLE
#define AL_MENU_SENSOR_AVAILABLE		/*!< zeigt die Sensordaten an */
#define AL_MENU_NETWORK_AVAILABLE		/*!< zeigt die Sensordaten an */
#define AL_MENU_TIMERS_AVAILABLE		/*!< zeigt die Sensordaten an */
#define AL_MENU_CYCLES_AVAILABLE		/*!< zeigt die Sensordaten an */


#define AL_MENU_1_AVAILABLE			/*!< zeigt die Menu 1 an */
#define AL_MENU_2_AVAILABLE			/*!< zeigt die Menu 2 an */
#define AL_MENU_3_AVAILABLE			/*!< zeigt die Menu 3 an */
#define AL_MENU_4_AVAILABLE			/*!< zeigt die Menu 4 an */
#define AL_MENU_5_AVAILABLE			/*!< zeigt die Menu 5 an */
#define AL_MENU_6_AVAILABLE			/*!< zeigt die Menu 6 an */
#define AL_MENU_7_AVAILABLE			/*!< zeigt die Menu 7 an */
#define AL_MENU_8_AVAILABLE			/*!< zeigt die Menu 8 an */
#define AL_MENU_9_AVAILABLE			/*!< zeigt die Menu 9 an */
#define AL_MENU_10_AVAILABLE		/*!< zeigt die Menu 10 an */
#define AL_MENU_11_AVAILABLE		/*!< zeigt die Menu 11 an */
#define AL_MENU_12_AVAILABLE		/*!< zeigt die Menu 12 an */



//#define DISPLAY_REMOTECALL_AVAILABLE		/*!< Steuerung der Verhalten inkl. Parametereingabe */
//#define DISPLAY_BEHAVIOUR_AVAILABLE		/*!< zeigt den Status aller Verhalten an */

//#define DISPLAY_MISC_AVAILABLE		/*!< aehm ja, der Rest irgendwie... */
//#define DISPLAY_MMC_INFO			/*!< Zeigt die Daten der MMC-Karte an */
//#define DISPLAY_MINIFAT_INFO			/*!< Zeigt Ausgaben des MiniFAT-Treibers an */
//#define DISPLAY_RESET_INFO_AVAILABLE		/*!< Zeigt Informationen ueber Resets an */


//extern uint8_t al_menuscreen;

#define AL_MENUSCREEN_NULL				0x00
#define AL_MENUSCREEN_CHEMRUNPROC		0x01
#define AL_MENUSCREEN_WAITWATERFILL		0x02

#define AL_MENUSCREEN_CYCLES			0x10

#define AL_MENUSCREEN_RETURN_INIT		0xFF



void init_menu(void);
void rm_menu_control(void);


/**
void al_basis_info_display(void);
void al_menu_network_1_display (void);
void al_menu_network_2_display (void);
void al_menu_akku_display (void);
void al_menu_sensors_display(void);
void al_menu_sensors_current_display(void);
void al_menu_time_display(void);
void al_menu_timers_display(void);
**/
