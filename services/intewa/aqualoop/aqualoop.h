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


#ifndef HAVE_AQUALOOP_H
#define HAVE_AQUALOOP_H

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



#define AQUALOOP_STATE_IDLE     0
#define AQUALOOP_STATE_AKKU     1
#define AQUALOOP_STATE_PUMP     2



extern volatile uint8_t aqualoop_state;

extern clock_datetime_t date;


/* Sämtliche im EEPROM gespeicherte Werte sind hier hinterlegt */
/* Anlagen-Parameter werden über menuconfig initialisiert */

typedef struct {

  uint8_t MEM_number;

  uint8_t suction_overflow;

  uint8_t sludge_pump_weeks;
  uint8_t sludge_pump_days;
  uint8_t sludge_pump_minutes;

  uint8_t suction_c1;
  uint8_t suction_c2;

  uint8_t air_on;
  uint8_t air_off;
  
  uint8_t t1_hour;
  uint8_t t1_min;

  uint8_t t2_hour;
  uint8_t t2_min;

  uint16_t levl_tank_min;
  uint16_t levl_tank_max;

} aqualoop_params_t;

extern aqualoop_params_t aqualoop_params_ram;



void default_button_handler(buttons_ButtonsType button, uint8_t status);

void default_gui_beep (void);



int16_t aqualoop_onrequest(char *cmd, char *output, uint16_t len);

int16_t aqualoop_init(void);

int16_t aqualoop_periodic(void);

void aqualoop_mainloop(void);





#include "config.h"
#ifdef DEBUG_AQUALOOP
# include "core/debug.h"
# define AQUALOOPDEBUG(a...)  debug_printf("al: " a)
#else
# define AQUALOOPDEBUG(a...)
#endif



#endif  /* HAVE_AQUALOOP_H */
