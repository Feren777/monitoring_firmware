/*
*
* GUI functions of the AQUALOOP Control Unit
* not intended for use from outside the control unit (monitoring)
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

#ifndef AQUALOOP_GUI_H
#define AQUALOOP_GUI_H

#include "aqualoop_menu.h"
//#include "hardware/input/buttons/buttons.h"


#define	AL_MENU_TOGGLE	255	/*!< Virtueller Screen zum Toggeln des Menus */


extern int8_t max_screens;					/*!< Anzahl der zurzeit registrierten Screens */
extern void (* screen_functions[AL_MENU_SCREENS])(void);	/*!< hier liegen die Zeiger auf die Display-Funktionen */

extern uint8_t display_screen;					/*!< Derzeit aktives Menu- bzw. Display screen-nr. */




/*!
 * Setzt das Display auf eine andere Ausgabe.
 * \param screen Parameter mit dem zu setzenden Screen.
 */
void gui_screen_set(uint8_t screen);

/*!
 * Display-Screen Anzeige
 *
 * Zeigt einen Screen an und fuehrt die RC5-Kommandoauswertung aus, falls noch nicht geschehen.
 * \param screen Nummer des Screens, der angezeigt werden soll
 */
void aqualoop_gui_display(uint8_t screen);

/*!
 * Display-Screen Initialisierung
 * Traegt die Anzeige-Funktionen in das Array ein.
 */
void aqualoop_gui_init(void);




#endif /* AQUALOOP_GUI_H */


