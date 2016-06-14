/*
* measurement functions of the AQUALOOP Control Unit
* not intended for use from outside the control unit (monitoring)
* main measurement functions
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
#include <util/atomic.h>

#include <stdbool.h>

#include "config.h"
#include "core/debug.h"
#include "core/periodic.h"

#include "core/bit-macros.h"
#include "protocols/ecmd/ecmd-base.h"

#include "services/clock/clock.h"
#include "hardware/adc/adc.h"
#include "hardware/onewire/onewire.h"

#include "aqualoop.h"
#include "aqualoop_core.h"
#include "aqualoop_sensors.h"


al_sensorbits_t al_sensors;



volatile timestamp_t capture_ts;
volatile timestamp_t get_ts;

volatile uint16_t capt_adc_24v;
//volatile uint16_t capt_adc_9v;

volatile uint16_t capt_adc_levelsensor;



/*
* Spannungsteiler auf Platine ist 91k:10k @ 24V = 2.376237
* 24 V / 2.376237 = 10.10016
* 5 Volt Referenzspannung * 10.1 * 1000 (Angabe in mV) = 5050
* 5.03 Volt Referenzspannung * 10.1 * 1000 (Angabe in mV) = 5080
*
* Ausgabe in mV * 10 !!
*/

uint16_t aqualoop_get_24v(void)
{
  uint16_t adc;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    capture_ts = clock_get_time();
    get_ts = capture_ts;
    capt_adc_24v = adc_get(AQUALOOP_ADC_24V_CHANNEL);
    adc = capt_adc_24v;
  }

  long adc_volt = (((long)(adc) * 5080)/1024L);


  return (uint16_t)( adc_volt );
}

/*
* Spannungsteiler auf Platine ist 91k:10k @ 9V = 3.0397350
* 9 V / 3.0397350 = 2.9607844
* 5 Volt Referenzspannung * 2.96 * 100 (Angabe in mV) = 1484
* 5.03 Volt Referenzspannung * 2.96 * 100 (Angabe in mV) = 1489
*
* Ausgabe in mV * 10 !!
*/
/*
uint16_t aqualoop_get_9v(void)
{
  uint16_t adc;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    capture_ts = clock_get_time();
    get_ts = capture_ts;
    capt_adc_9v = adc_get(AQUALOOP_ADC_9V_CHANNEL);
    adc = capt_adc_9v;
  }

  long adc_volt = (((long)(adc) * 1489)/1024L);


  return (uint16_t)( adc_volt );
}
*/


/*
 * Referenzspannung ist ca 5030 mV
 * Sensor liefert 0A bei 5030/2 (in der Mitte des Messwerts für +- Erfassung)
 * Sensor hat Auflösung von 185mV/A
 * 
 */

/**
int16_t aqualoop_get_I0(void)
{
  uint16_t adc;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    capture_ts = clock_get_time();
    get_ts = capture_ts;

    int8_t i;
    int32_t adc_average = 0;

    for(i=0; i<64; i++) {
      capt_adc_I0 = adc_get(AQUALOOP_ADC_I0_CHANNEL);
      adc_average = adc_average + capt_adc_I0;
    }
    
    adc = adc_average / 64L;
  }

  int32_t adc_sensed_volts = (((int32_t)(adc) * 5030L) / 1024L);

  int32_t adc_amperes = ((adc_sensed_volts - 2515L) * 1000L);  //Ausgabe in mV
  adc_amperes = adc_amperes / 185L;

  return (int16_t)(adc_amperes);
  //return (int16_t)(adc_sensed_volts);
}


int16_t aqualoop_get_I1(void)
{
  uint16_t adc;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    capture_ts = clock_get_time();
    get_ts = capture_ts;

    int8_t i;
    int32_t adc_average = 0;

    for(i=0; i<64; i++) {
      capt_adc_I0 = adc_get(AQUALOOP_ADC_I1_CHANNEL);
      adc_average = adc_average + capt_adc_I0;
    }
    
    adc = adc_average / 64L;
  }

  int32_t adc_sensed_volts = (((int32_t)(adc) * 5030L) / 1024L);

  int32_t adc_amperes = ((adc_sensed_volts - 2515L) * 1000L);  //Ausgabe in mV
  adc_amperes = adc_amperes / 185L;

  return (int16_t)(adc_amperes);
  //return (int16_t)(adc_sensed_volts);
}

**/



/*
* Spannungsteiler auf Platine ist 91k:10k @ 24V = 2.376237
* 24 V / 2.376237 = 10.10016
* 5 Volt Referenzspannung * 10.1 * 1000 (Angabe in mV) = 5050
* 5.03 Volt Referenzspannung * 10.1 * 1000 (Angabe in mV) = 5080
*
* Ausgabe in mV * 10 !!
*/

uint16_t aqualoop_get_levelsensor(void)
{
  uint16_t adc;
  uint16_t level_average = 0;
  
  int8_t i;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    capture_ts = clock_get_time();
    get_ts = capture_ts;
    
    for (i=0; i<32; i++) {
      capt_adc_levelsensor = adc_get(AQUALOOP_LEVEL_SENSOR);
      
      level_average = level_average + capt_adc_levelsensor;
    }

  }

  adc = level_average / 32L;

  //long adc_waterlevel = (((long)(adc) * 5080)/1024L);
  //return (uint16_t)( adc_waterlevel );

  return adc;
}


uint16_t aqualoop_get_tanklevel(void)
{

  uint32_t level_value;   // Raw ADC value from 0 to 1023
  uint32_t level_percent; // Expressed in Percent / Permill (0-1000%)

  level_value = (uint32_t)aqualoop_get_levelsensor();

  if (level_value > aqualoop_params_ram.levl_tank_max) level_value = aqualoop_params_ram.levl_tank_max;
  if (level_value < aqualoop_params_ram.levl_tank_min) level_value = aqualoop_params_ram.levl_tank_min;

  level_percent = ((level_value - aqualoop_params_ram.levl_tank_min) * 1000U) / (aqualoop_params_ram.levl_tank_max - aqualoop_params_ram.levl_tank_min);

  return (uint16_t)( level_percent ); 
}





int16_t aqualoop_get_current(uint8_t channel, int8_t offset)
{
  uint16_t adc;
  uint32_t adc_average = 0;
  uint16_t adc_temp = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    capture_ts = clock_get_time();
    get_ts = capture_ts;

    int8_t i;

    for(i=0; i<64; i++) {
      adc_temp = adc_get(channel) - offset;
      adc_average = adc_average + adc_temp;
    }
    
    adc = adc_average / 64L;
  }

  int32_t adc_sensed_volts = (((int32_t)(adc) * 5030L) / 1024L);

  int32_t adc_amperes = ((adc_sensed_volts - 2515L) * 1000L);  //Ausgabe in mV
  adc_amperes = adc_amperes / 185L;

  return (int16_t)(adc_amperes);  // Ausgabe in mA
  //return (int16_t)(adc);        // Ausgabe des ADC Werts für Debug
}









#ifdef ONEWIRE_SUPPORT
int16_t aqualoop_get_temperature(ow_rom_code_t * rom) {
  int16_t retval = 0x7FFF;      /* error */
  ow_temp_scratchpad_t sp;
  int8_t ret;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    ret = ow_temp_read_scratchpad(rom, &sp);
  }

  if (ret == 1) {
    ow_temp_t temp = ow_temp_normalize(rom, &sp);
    //retval = (temp.twodigits ? temp.val / 10 : temp.val);
    retval = temp.val;
    ret = temp.twodigits;
  }

  //debug_printf("temp: %d digits: %d \n", retval, ret);

  return retval;
}
#endif








void init_sensors(void) {

  #ifdef ONEWIRE_SUPPORT
  if (parse_ow_rom(CONF_SENSOR_GELB, &romcodeGelb) == -1) {
    AQUALOOPDEBUG("parse error ow GELB\n");
  }
  if (parse_ow_rom(CONF_SENSOR_GRUEN, &romcodeGruen) == -1) {
    AQUALOOPDEBUG("parse error ow GRUEN\n");
  }
  if (parse_ow_rom(CONF_SENSOR_VORLAUF, &romcodeVorlauf) == -1) {
    AQUALOOPDEBUG("parse error ow GRUEN\n");
  }

  #endif

}



void aqualoop_sensors_periodic(void) {

  /* Schwimmschalter erfassen */
  if(!PIN_HIGH(AL_SSBRMIN))   al_sensors.SSBRmin   = 1; else al_sensors.SSBRmin   = 0;
  if(!PIN_HIGH(AL_SSKLARMIN)) al_sensors.SSKlarmin = 1; else al_sensors.SSKlarmin = 0;
  if(!PIN_HIGH(AL_SSKLARMAX)) al_sensors.SSKlarmax = 1; else al_sensors.SSKlarmax = 0;

  /* Eingangsspannung überwachen und auf Akkubetrieb umschalten */
  /*
  if(aqualoop_get_24v() < aqualoop_get_9v()) {
  aqualoop_state = AQUALOOP_STATE_AKKU;
  al_sensors.akku = 1;
  } else {
  al_sensors.akku = 0;
  }
  */
}


void aqualoop_sensors_30sec(void) {

  #ifdef ONEWIRE_SUPPORT
  aqualoop_get_temperature(&romcodeGelb);
  aqualoop_get_temperature(&romcodeGruen);
  aqualoop_get_temperature(&romcodeVorlauf);
  #endif
  
}











/* Hilfsfunktionen für die Umrechnung diverser Messwerte */

uint8_t measure_to_int (uint16_t value) {

  value = value / 100;

  return (uint8_t)(value);
}


uint8_t measure_to_dec (uint16_t value) {

  value = value % 100;

  return (uint8_t)(value); 

}


/*
  -- Ethersex META --
  header(services/intewa/aqualoop/aqualoop_sensors.h)
  timer(10, aqualoop_sensors_periodic())

*/

