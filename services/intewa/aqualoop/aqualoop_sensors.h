/*
*
* internal functions of the AQUALOOP Control Unit
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

#ifndef AQUALOOP_SENSORS_H
#define AQUALOOP_SENSORS_H

#include "services/clock/clock.h"

#include "hardware/onewire/onewire.h"



#define AQUALOOP_ADC_24V_CHANNEL	0		/* PORT PF0 (ADC0) connected to 24 V measurement voltage divider */
//#define AQUALOOP_ADC_9V_CHANNEL		3	/* PORT PF3 (ADC3) connected to  9 V measurement voltage divider */

#define AQUALOOP_ADC_CURR_SENS_0	5		/* PORT PF5 (ADC5) connected to current */
#define AQUALOOP_ADC_CURR_SENS_1	6


#define AQUALOOP_LEVEL_SENSOR		4		/* PORT PF4 (ADC4) connected to level sensor SIGNAL AD5 on board */


//#define CONF_SENSOR_VORLAUF	"282349d4050000d1" /* V1 Board mit Stromsensoren */ 
#define CONF_SENSOR_GELB	"28cd23d5050000c8" 	/* V2 Board bei Haraldo */
#define CONF_SENSOR_GRUEN	"282349d4050000d1" 	/* V2 Board bei Haraldo */

#define CONF_SENSOR_VORLAUF "28cffcd405000018"	/* Demo-Board im Office */




typedef struct ALsensorBits {
 unsigned int SSBRmin:1;		//0=Normalbetrieb, 1=Akku laden
 unsigned int SSKlarmin:1;		//0=trip OFF, 1=trip ON
 unsigned int SSKlarmax:1;

 //unsigned int akku:1;			//0=Netzbetrieb OK, 1=Akku-Notfallbetrieb



 unsigned int free1:1;
 unsigned int free2:1;		//1=mute
 unsigned int free3:1;		//1=diag mode on
 unsigned int free4:1;		//internal, in the confirm7241()
} al_sensorbits_t;

extern al_sensorbits_t al_sensors;



/* Werden für A/D Wandlung benötigt */
extern volatile timestamp_t capture_ts;
extern volatile timestamp_t get_ts;

/* Eingangsspannungen am A/D Wandler */
extern volatile uint16_t capt_adc_24v;
//extern volatile uint16_t capt_adc_9v;

/* 1W Temperatursensor */
#ifdef ONEWIRE_SUPPORT
ow_rom_code_t romcodeGelb;
ow_rom_code_t romcodeGruen;
ow_rom_code_t romcodeVorlauf;
int16_t aqualoop_get_temperature(ow_rom_code_t *rom);
#endif







void init_sensors(void);

void aqualoop_sensors_periodic(void);

void aqualoop_sensors_30sec(void);



uint16_t aqualoop_get_24v(void);
//uint16_t aqualoop_get_9v(void);

uint16_t aqualoop_get_levelsensor(void);
uint16_t aqualoop_get_tanklevel(void);


int16_t aqualoop_get_current(uint8_t channel, int8_t offset);



uint8_t measure_to_int (uint16_t value);
uint8_t measure_to_dec (uint16_t value);


#endif /* AQUALOOP_SENSORS_H */
