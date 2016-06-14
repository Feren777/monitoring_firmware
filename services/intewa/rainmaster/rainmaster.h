/*
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

 /*! 
 *  \brief     Pretty nice class.
 *  \details   This class is used to demonstrate a number of section commands.
 *  \author    John Doe
 *  \author    Jan Doe
 *  \version   4.1a
 *  \date      1990-2011
 *  \pre       First initialize the system.
 *  \bug       Not all memory is freed when deleting an object of this class.
 *  \warning   Improper use can crash your application
 *  \copyright GNU Public License.
 */


#ifndef HAVE_RAINMASTER_H
#define HAVE_RAINMASTER_H

#include "config.h"
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "services/clock/clock.h"
#include "hardware/input/buttons/buttons.h"



/* Seriennummer des Produkts */
/* Kann per ECMD gesetzt werden: setserialno KEY SERIALNR */

#define INTEWA_PRODUCT_ID       0x10          /* Definiert ein Produkt als TYPUS      */
#define INTEWA_MAJOR_ID         0x20          /* Definiert eine Variante des Produkts */
#define INTEWA_MINOR_ID         0x30          /* Definiert eine Unterversion der Variante */



#define RAINMASTER_STATE_IDLE     0
#define RAINMASTER_STATE_AKKU     1
#define RAINMASTER_STATE_PUMP     2


/* Allgemeine Statusvariablen und Zustände der Ein-/Ausgänge */
typedef struct RMstates {
  unsigned int PumpenFlag:1;        // 0=aus, 1=an
  unsigned int SchwimmSchalter:1;   // 0=trip OFF, 1=trip ON
  unsigned int Pos1Schalter:1;
  unsigned int PumpenKontakt:1;

  unsigned int free1:1;    //0=Netzbetrieb OK, 1=Akku-Notfallbetrieb
  unsigned int free2:1;    //1=mute
  unsigned int free3:1;    //1=diag mode on
  unsigned int free4:1;    //internal, in the confirm7241()
} rm_statusbits_t;

extern rm_statusbits_t rm_status;


/* Sämtliche im EEPROM gespeicherte Werte sind hier hinterlegt */
/* Anlagen-Parameter werden über menuconfig initialisiert */
typedef struct RMparams {

  uint8_t MEM_number;       /* Anzahl der verfügbaren Membranen */

} rainmaster_params_t;

extern rainmaster_params_t rainmaster_params_ram;


/* Current time stamp */
extern clock_datetime_t date;

/* Timer for continuing running */
extern uint16_t rm_offcounter;


//extern volatile uint8_t rainmaster_state;






void default_button_handler(buttons_ButtonsType button, uint8_t status);

void default_gui_beep (void);



int16_t rainmaster_onrequest(char *cmd, char *output, uint16_t len);

int16_t rainmaster_init(void);

int16_t rainmaster_periodic(void);

void rainmaster_mainloop(void);





#include "config.h"
#ifdef DEBUG_RAINMASTER
# include "core/debug.h"
# define RAINMASTERDEBUG(a...)  debug_printf("rm: " a)
#else
# define RAINMASTERDEBUG(a...)
#endif



#endif  /* HAVE_RAINMASTER_H */
