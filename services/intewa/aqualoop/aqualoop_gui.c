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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "core/bit-macros.h"
#include "core/debug.h"

#include "aqualoop.h"
#include "aqualoop_core.h"
#include "aqualoop_sensors.h"
#include "aqualoop_gui.h"
#include "aqualoop_menu.h"

#include "hardware/lcd/hd44780.h"
//#include "hardware/input/buttons/buttons.h"

//#include "services/lcd-menu/lcd_menu.h"

int8_t max_screens = 0;										/*!< Anzahl der zurzeit registrierten Screens */
uint8_t display_screen = 0;									/*!< Derzeit aktives Menu- bzw. Display screen-nr. */

void (* screen_functions[AL_MENU_SCREENS])(void) = {NULL}; 	/*!< hier liegen die Zeiger auf die Display-Funktionen */






/*!
 * Setzt das Display auf eine andere Ausgabe.
 * \param screen Parameter mit dem zu setzenden Screen.
 */
void gui_screen_set(uint8_t screen) {

	if (screen == AL_MENU_TOGGLE) {
		display_screen++; // zappen
	} else {
		display_screen = screen; // Direktwahl
	}

	if (display_screen >= max_screens) {
		display_screen = 0; // endliche Screenanzahl
	}

	hd44780_clear(); // alten Screen loeschen, das Zeichnen uerbernimmt GUI-Handler
	default_gui_beep();
}

/*!
 * \brief Display-Screen Registrierung
 * \param fkt 	Zeiger auf eine Funktion, die den Display-Screen anzeigt
 *
 * Legt einen neuen Display-Screen an und haengt eine Anzeigefunktion ein.
 * Diese Funktion kann auch RC5-Kommandos behandeln. Wurde eine Taste ausgewertet, setzt man RC5_Code auf 0.
 */
static int8_t register_screen(uint8_t id, void (* fkt)(void)) {
	if (max_screens == AL_MENU_SCREENS) {
		return -1; // sorry, aber fuer dich ist kein Platz mehr da :(
	}
	int8_t screen_nr = max_screens++; // neuen Screen hinten anfuegen
	screen_functions[id] = fkt; // Pointer im Array speichern
	return screen_nr;
}

/*!
 * Display-Screen Anzeige
 *
 * Zeigt einen Screen an und fuehrt die RC5-Kommandoauswertung aus, falls noch nicht geschehen.
 * \param screen Nummer des Screens, der angezeigt werden soll
 */
void aqualoop_gui_display(uint8_t screen) {

	/* Gueltigkeit der Screen-Nr. pruefen und Anzeigefunktion aufrufen, falls Screen belegt ist */
	if (screen < max_screens && screen_functions[screen] != NULL) {
		screen_functions[screen]();
	}

}

/*!
 * Display-Screen Initialisierung
 * Traegt die Anzeige-Funktionen in das Array ein.
 */
void aqualoop_gui_init(void) {

	hook_buttons_input_register(default_button_handler);


//	register_screen(NULL); // erzeugt einen leeren Display-Screen an erster Position

#ifdef AL_BASIS_INFO_AVAILABLE
	register_screen(0, &al_basis_info_display);
#endif



}







