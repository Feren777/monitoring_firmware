#include <avr/io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "config.h"
#include "core/bit-macros.h"
#include "core/eeprom.h"
#include "core/debug.h"

#include "rainmaster.h"
#include "protocols/ecmd/ecmd-base.h"
#include "protocols/ecmd/sender/ecmd_sender_net.h"
#include "protocols/uip/uip.h"
#include "protocols/uip/parse.h"
#include "hardware/pwm/pwm.h"



uip_ipaddr_t remoteaddr;

int16_t parse_cmd_rainmaster_gettanklevel(char *cmd, char *output, uint16_t len) 
{

	uint16_t tankvalue;
	tankvalue = rainmaster_get_tanklevel();

	uip_ipaddr_t temp_ip_addr;
	uip_gethostaddr(&temp_ip_addr);
	uint8_t id = uip_ipaddr4(&temp_ip_addr);

	return ECMD_FINAL(snprintf_P(output, len, PSTR("%03d %04d %d"), id, tankvalue, rm_status.PumpenFlag));
}




int16_t parse_cmd_rainmaster_setvalve(char *cmd, char *output, uint16_t len)
{
	uint8_t value=0;
	uint16_t ret = 0; // must be 16 bit; because the answer length may be > 255
	if (cmd[0]!=0) ret = sscanf_P(cmd, PSTR("%u"),  &value);
	
	if (ret == 1)
	{
		value != 0 ? setpwm('b', 255) : setpwm('b', 0);
		return ECMD_FINAL_OK;
	}
	else
		return ECMD_ERR_PARSE_ERROR;
}



int16_t parse_cmd_rainmaster_request(char *cmd, char *output, uint16_t len)
{
	uint8_t value=0;
	uint16_t ret = 0; // must be 16 bit; because the answer length may be > 255
	if (cmd[0]!=0) ret = sscanf_P(cmd, PSTR("%u"),  &value);
	
	if (ret == 1)
	{
		// Wir haben keinen Eigenbedarf und genügend Wasser im Tank, können daher einspeisen
		if(/*rm_status.PumpenKontakt != true && */ my_tanklevel > TANK_MAX) {

			if(value != 0) {
				//setpwm('a', 255);
				//rm_status.PumpenFlag = true;  // Pumpenflag setzen
				// Eigenes Magnetventil öffnen
				setpwm('c', 255);
				rm_status.Magnetventil = 1;
			} else {
				//setpwm('a', 0);
				//rm_status.PumpenFlag = false;  // Pumpenflag setzen
				// Eigenes Magnetventil zu
				setpwm('c', 0);
				rm_status.Magnetventil = 0;
			}
		}

		return ECMD_FINAL_OK;
	}
	else
		return ECMD_ERR_PARSE_ERROR;
}


int16_t parse_cmd_rainmaster_getstatus(char *cmd, char *output, uint16_t len)
{
	//uip_ipaddr_t remoteaddr;

    while (*cmd == ' ')
	cmd++;

    if (*cmd != '\0') {
		/* try to parse ip */
		if (parse_ip(cmd, &remoteaddr))
		    return ECMD_ERR_PARSE_ERROR;

		ecmd_callback_blocking = 1;
		ecmd_sender_send_command_P(&remoteaddr, rainmaster_request_callback, PSTR("rainmaster_tanklevel\n"));

		return ECMD_FINAL_OK;
    }
    else {
		return ECMD_ERR_PARSE_ERROR;
    }
}







/*
-- Ethersex META --
block([[Application_Rainmaster]])
ecmd_feature(rainmaster_gettanklevel, "rainmaster_tanklevel",, Get percentage of tank level in X/1000)
ecmd_feature(rainmaster_setvalve, "rainmaster_valve",VALUE , Set position of valve 0 = close  1 = open)
ecmd_feature(rainmaster_request, "rainmaster_request",VALUE , Set pump value of valve 0 = off  1 = on)
ecmd_feature(rainmaster_getstatus, "rainmaster_getstatus",[IPADDR] , Get level of IP address)
*/

