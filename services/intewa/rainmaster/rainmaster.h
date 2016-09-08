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
#include "protocols/uip/uip.h"


/* Seriennummer des Produkts */
/* Kann per ECMD gesetzt werden: setserialno KEY SERIALNR */

#define INTEWA_PRODUCT_ID       0x10          /* Definiert ein Produkt als TYPUS      */
#define INTEWA_MAJOR_ID         0x20          /* Definiert eine Variante des Produkts */
#define INTEWA_MINOR_ID         0x30          /* Definiert eine Unterversion der Variante */



#define RAINMASTER_STATE_IDLE        0   // Initialzustand
#define RAINMASTER_STATE_REQUESTED   1   // Haben genügend Regenwasser im eigenen Tank
#define RAINMASTER_STATE_SELF        2   //




#define RAINMASTER_LEVEL_SENSOR		  4		/* PORT PF4 (ADC4) connected to level sensor SIGNAL AD5 on board */


#define RM_COUNT_DEVICES            3
#define RM_ID1                    101
#define RM_ID2                    102
#define RM_ID3                    103


#define IP_RM1 = 192.168.1.101
#define IP_RM2 = 192.168.1.102
#define IP_RM3 = 192.168.1.103


#define TANK_MIN      100
#define TANK_MAX      250



/* Sämtliche im EEPROM gespeicherte Werte sind hier hinterlegt */
/* Anlagen-Parameter werden über menuconfig initialisiert */
typedef struct {

  uint8_t MEM_number;       /* Anzahl der verfügbaren Membranen */

  uint16_t levl_tank_min;
  uint16_t levl_tank_max;

} rainmaster_params_t;

extern rainmaster_params_t rainmaster_params_ram;



typedef struct RMdevices {
  uint8_t       device_id;
  uint16_t      tanklevel;
  uint8_t       request_state;
  uint8_t       busy;
//  uip_ipaddr_t  device_ip;
} rm_device_t;

extern rm_device_t rm_001;
extern rm_device_t rm_002;
extern rm_device_t rm_003;




/* Allgemeine Statusvariablen und Zustände der Ein-/Ausgänge */
typedef struct RMstates {
  unsigned int PumpenFlag:1;        // 0=aus, 1=an
  unsigned int SchwimmSchalter:1;   // 0=trip OFF, 1=trip ON
  unsigned int Pos1Schalter:1;
  unsigned int PumpenKontakt:1;

  //uint16_t     tanklevel;         // eigener Vorrat an Regenwasser 

  unsigned int Kugelhahn:1;       //0=Regenwasser, 1=Klarwasser/Nachspeisung
  unsigned int Magnetventil:1;    //0=zu, 1=auf
  unsigned int free3:1;    //1=diag mode on
  unsigned int free4:1;    //internal
} rm_statusbits_t;

extern rm_statusbits_t rm_status;

extern uint16_t my_tanklevel;



/* Current time stamp */
extern clock_datetime_t date;

/* Timer for continuing running */
extern uint16_t rm_offcounter;


//extern volatile uint8_t rainmaster_state;




extern uint8_t ecmd_callback_blocking;


void default_button_handler(buttons_ButtonsType button, uint8_t status);

void default_gui_beep (void);


uint16_t rainmaster_get_levelsensor(void);
uint16_t rainmaster_get_tanklevel(void);
void rainmaster_request_callback(char* text, uint8_t len);

int16_t rainmaster_onrequest(char *cmd, char *output, uint16_t len);

int16_t rainmaster_init(void);

int16_t rainmaster_periodic(void);

void rainmaster_mainloop(void);

void rainmaster_long_timer(void);



#include "config.h"
#ifdef DEBUG_RAINMASTER
# include "core/debug.h"
# define RAINMASTERDEBUG(a...)  debug_printf("rm: " a)
#else
# define RAINMASTERDEBUG(a...)
#endif



#endif  /* HAVE_RAINMASTER_H */
