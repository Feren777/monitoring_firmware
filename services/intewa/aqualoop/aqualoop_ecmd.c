/*
 * Copyright (c) 2009 by Harald W. Leschner <leschner@intewa.de>
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

#include <avr/io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "config.h"
#include "core/eeprom.h"
#include "core/debug.h"

#include "aqualoop.h"
#include "aqualoop_sensors.h"
#include "protocols/ecmd/ecmd-base.h"




/*
  Abfrage der Seriennummer des Geräts
  Neue Seriennummer kann mit Angabe eines Passworts neu einprogrammiert werden

  ecmd?aqualoop_serial iKEYPASS 12345678
*/
int16_t parse_cmd_aqualoop_serial(char *cmd, char *output, uint16_t len) 
{
    char new_serial[sizeof(((struct eeprom_config_t * )0x0000)->aqualoop_serial) + 1];
    //char new_serial[9];

    memset(new_serial, 0, sizeof(new_serial));

    while (*cmd == ' ') cmd ++;
    if (! *cmd ) { /* No argument */

display_serial:

      eeprom_restore(aqualoop_serial, new_serial, sizeof(new_serial));
      //strcpy(new_serial, "00000000");

      /* Wird nur einmal am Anfang ausgeführt, falls im EEPROM "Datenmüll" steht: 0xFF */
      if ( new_serial[0] && new_serial[1] && new_serial[2] && new_serial[3] && new_serial[4] && new_serial[5] && new_serial[6] && new_serial[7] == 0xFF ) {
      	//strcpy(new_serial, "00000000");
      	  strcpy(new_serial, "NOT SET!");
      	//eeprom_save(aqualoop_serial, new_serial, sizeof(new_serial));
      	//eeprom_update_chksum();
      }
	  
      return ECMD_FINAL(snprintf_P(output, len, PSTR("%s"), new_serial));

    } else {

      /* Pointer auf den Anfang der Seriennummer setzen ... */
      char *serial = strchr(cmd, ' ');
      if (!serial) return ECMD_ERR_PARSE_ERROR;
      *serial = 0;
      serial ++;
      while (*serial == ' ') serial ++;

      /* ... und ersetzen Leerstellen mit einer Null ... */
      char *s = strchr(serial, ' ');
      if (s) *s = 0;

      /* ... und kopieren die Zeichen aus cmd in unsere Variable MINUS der (beiden) Leerzeichen... */
      strncpy(new_serial, serial, sizeof(new_serial) - 2);

      /* ... und fügen die NULL (\0) Terminierung ein. */
      new_serial[sizeof(new_serial) - 2] = 0;


      /* Geheimes ARGUMENT - Passwort um die Seriennummer fest einzuprogrammieren */
      if (strncmp_P(cmd, PSTR("iKEYPASS"), 8) == 0) {
	      eeprom_save(aqualoop_serial, new_serial, sizeof(new_serial));
	      eeprom_update_chksum();
	  }

      goto display_serial;
    }
}



int16_t parse_cmd_aqualoop_gettanklevel(char *cmd, char *output, uint16_t len) 
{

  uint16_t tankvalue;
  tankvalue = aqualoop_get_tanklevel();

  return ECMD_FINAL(snprintf_P(output, len, PSTR("%d"), tankvalue));
}


/**
int16_t parse_cmd_aqualoop_setuuid(char *cmd, char *output, uint16_t len) 
{

  uint16_t uuid;
  uuid = aqualoop_get_tanklevel();

  return ECMD_FINAL(snprintf_P(output, len, PSTR("%d"), uuid));
}
**/






















int16_t parse_cmd_aqualoop_command(char *cmd, char *output, uint16_t len) 
{
  return aqualoop_onrequest(cmd, output, len);
}

int16_t parse_cmd_aqualoop_init(char *cmd, char *output, uint16_t len) 
{
  return aqualoop_init();
}

int16_t parse_cmd_aqualoop_periodic(char *cmd, char *output, uint16_t len) 
{
  return aqualoop_periodic();
}

/*
-- Ethersex META --
block([[Application_Aqualoop]])
ecmd_feature(aqualoop_command, "aqualoop_cmd",, Manually call application aqualoop commands)
ecmd_feature(aqualoop_init, "aqualoop_init",, Manually call application aqualoop init method)
ecmd_feature(aqualoop_periodic, "aqualoop_periodic",, Manually call application aqualoop periodic method)
ecmd_feature(aqualoop_serial, "aqualoop_serial", KEY SERIAL, Initial factory setting of internal serial number)
ecmd_feature(aqualoop_gettanklevel, "aqualoop_tanklevel",, Get percentage of tank level in X/1000)
*/
