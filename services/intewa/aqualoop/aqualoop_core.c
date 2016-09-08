/*
 * Copyright (c) 2014 by Harald W. Leschner <leschner@intewa.de>
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



#include <stdbool.h>

#include "config.h"
#include "core/debug.h"
#include "core/periodic.h"

#include "core/bit-macros.h"
#include "protocols/ecmd/ecmd-base.h"

#include "aqualoop.h"
#include "aqualoop_core.h"
#include "aqualoop_sensors.h"
//#include "aqualoop_gui.h"
#include "aqualoop_menu.h"
#include "services/lcd-menu/lcd_menu.h"

#include "hardware/pwm/pwm.h"


al_statbitflags_t al_statusbits;

/* PUMPS cycle timestamps and settings */
struct pumpcycle cycle_t1;      // Pumping cycle C1
struct pumpcycle cycle_t2;      // Pumping cycle C2

struct pumpcycle cycle_chem;    // Chemical cleaning cycle


/* AIR change timestamps */
static timestamp_t last_change_air = 0;
static uint8_t toggle_air = false;
static uint8_t state_air = false;

/* SLUDGE change timestamps */
static timestamp_t last_change_sludge = 0;
static uint8_t toggle_sludge = false;
static uint8_t state_sludge = false;

/* Pumps PWM duty cycle depending on MEM setting */
const uint8_t mem_pwm[] =
{
  0,       // Invalid
  102,     // MEM = 1
  127,     // MEM = 2
  152,     // MEM = 3
  203,     // MEM = 4
  225,     // MEM = 5
  255      // MEM = 6
};


/* Backflush duration in seconds depending on MEM setting */
const uint8_t mem_cleantime[] =
{
  0,      // Invalid
  20,     // MEM = 1
  24,     // MEM = 2
  30,     // MEM = 3
  40,     // MEM = 4
  60,     // MEM = 5
  120     // MEM = 6
};



void aqualoop_air_toggle_core(void) {

  /* Aktuellen timestamp holen (wird in Sekunden seit 1.1.1970 zurückgegeben */
  timestamp_t air_ts = clock_get_time();

  /* Falls im Menu ON = 0 gesetzt ist, wird der Ausgang dauerhaft ausgeschaltet */
  if (aqualoop_params_ram.air_on == 0) {
    state_air = false; //PIN_CLEAR(AL_COMPRESSOR_AIR);
    toggle_air = false;
    last_change_air = 0;
  }

  if (aqualoop_params_ram.air_on > 0) {

    if ( toggle_air == false) {

      /* Im Menu wird die Laufzeit in Minuten angegeben, daher müssen wir die Sekunden mit 60 multiplizieren */
      if ( air_ts - last_change_air >= (uint32_t)aqualoop_params_ram.air_off * 60 ) {
        last_change_air = air_ts;
        state_air = true; //PIN_SET(AL_COMPRESSOR_AIR);
        toggle_air = true;
      }

    } else {

      if ( air_ts - last_change_air >= (uint32_t)aqualoop_params_ram.air_on * 60 ) {
        last_change_air = air_ts;
        state_air = false; //PIN_CLEAR(AL_COMPRESSOR_AIR);
        toggle_air = false;
      }

    }

  }

  // Falls BRmin Schwimmschalter genügend Wasser meldet, dann aktiviere die Lüftung nach Bedarf
  if (al_sensors.SSBRmin && state_air)
    PIN_SET(AL_COMPRESSOR_AIR);
  else
    PIN_CLEAR(AL_COMPRESSOR_AIR);
}



void aqualoop_sludge_toggle_core(void) {

  /* Aktuellen timestamp holen (wird in Sekunden seit 1.1.1970 zurückgegeben */
  timestamp_t sludge_ts = clock_get_time();

  /* Falls im Menu ON = 0 gesetzt ist, wird der Ausgang dauerhaft ausgeschaltet */
  if (aqualoop_params_ram.sludge_pump_days == 0) {
    state_sludge = false; //PIN_CLEAR(AL_SLUDGE_PUMP);
    toggle_sludge = false;
    last_change_sludge = 0;
  }

  if (aqualoop_params_ram.sludge_pump_days > 0) {

    if ( toggle_sludge == false) {

      /* Im Menu wird die Laufzeit in Tage angegeben, daher müssen wir die Sekunden mit 86400 multiplizieren (1d = 86400s) */
      if ( sludge_ts - last_change_sludge >= (uint32_t)aqualoop_params_ram.sludge_pump_days * 86400 ) {
        last_change_sludge = sludge_ts;
        state_sludge = true; //PIN_SET(AL_SLUDGE_PUMP);
        toggle_sludge = true;
      }

    } else {

      if ( sludge_ts - last_change_sludge >= (uint32_t)aqualoop_params_ram.sludge_pump_minutes * 60 ) {
        last_change_sludge = sludge_ts;
        state_sludge = false; //PIN_CLEAR(AL_SLUDGE_PUMP);
        toggle_sludge = false;
      }

    }

  }

  // Falls BRmin Schwimmschalter genügend Wasser meldet, dann aktiviere die Schlammpumpe nach Bedarf
  if (al_sensors.SSBRmin && state_sludge)
    PIN_SET(AL_SLUDGE_PUMP);
  else
    PIN_CLEAR(AL_SLUDGE_PUMP);
}








/*
d8888b. db    db .88b  d88. d8888b.  .o88b. db    db  .o88b. db      d88888b 
88  `8D 88    88 88'YbdP`88 88  `8D d8P  Y8 `8b  d8' d8P  Y8 88      88'     
88oodD' 88    88 88  88  88 88oodD' 8P       `8bd8'  8P      88      88ooooo 
88~~~   88    88 88  88  88 88~~~   8b         88    8b      88      88~~~~~ 
88      88b  d88 88  88  88 88      Y8b  d8    88    Y8b  d8 88booo. 88.     
88      ~Y8888P' YP  YP  YP 88       `Y88P'    YP     `Y88P' Y88888P Y88888P 
*/

void aqualoop_setsuction(uint8_t pwm) {

  (al_sensors.SSBRmin && !al_sensors.SSKlarmax) ? setpwm('a', pwm) : setpwm('a', 0);
}

void aqualoop_setbackflush(uint8_t pwm) {

  (al_sensors.SSBRmin && !al_sensors.SSKlarmax) ? setpwm('b', pwm) : setpwm('b', 0);

}


timestamp_t day_seconds(void) {

  timestamp_t t1, t2;
  clock_datetime_t tms;
  t1 = clock_get_time();
  clock_current_localtime(&tms);
  tms.hour = 0;
  tms.min = 0;
  tms.sec = 0;
  t2 = clock_mktime(&tms, 1);
  return t1 - t2;

}

uint32_t aqualoop_pumpcycle_calc_countdown(struct pumpcycle *cycle) {

#if 0
  timestamp_t liftoff   /* Timestamp to next sceduled pumpcycle */
  clock_datetime_t c;   /* Struct of countdown timer */

  c.sec;                   /* Sekunden 0-59 */
  c.min;                   /* Minuten 0-59 */
  c.hour;                  /* Stunden 0-23 */
  c.day;                   /* Tag des Monats 1-31 */
  c.month;                 /* Monat 1-12 */
  c.dow;                   /* Tag der Woche 0-6, So-Sa */
  c.year;                  /* Jahr seit 1900 */
  c.yday;                  /* Tag des Jahres 0-365 */
  c.isdst = date.isdst;    /* Sommerzeit */


  /* Is it today or "tomorrow"?  (we have only 24 hour timer-based events */
  if(cycle-hour > date.hour) {
    c.hour = cycle->hour;  /* Stunden of CYCLE */
    c.isdst = date.isdst;  /* Sommerzeit */
    c.yday = date.yday;    /* Tag des Jahres 0-365 */
    c.year = date.year;    /* Jahr seit 1900 */
    c.dow = date.dow;      /* Tag der Woche 0-6, So-Sa */
    c.month = date.month;  /* Monat 1-12 */
    c.day = date.day;      /* Tag des Monats 1-31 */
    c.sec = 0;             /* Sekunden 0-59 */
    c.min = cycle->min;    /* Minuten of CYCLE */
  } else if (cycle-hour < date.hour)
#endif

  return 0;

}

uint32_t aqualoop_pumpcycle_calc_runtime(struct pumpcycle *cycle) {

  return (uint32_t)cycle->duration_start + (((uint32_t)cycle->duration_suction) + (uint32_t)cycle->duration_pause1 + (uint32_t)cycle->duration_backflush + (uint32_t)cycle->duration_pause2) * (uint32_t)cycle->cycles_total;

}

void aqualoop_pumps_cycle(struct pumpcycle *cycle) {

  timestamp_t pumps_ts = clock_get_time();
  //timestamp_t pumps_ts = 100;

  /* Checks if we need to start a new cycle sequence programmed at this time */
  //                            if ( (date.min == cycle->min) && (date.sec = 00) && (date.sec <= 01) ) {
  if ( (date.hour == cycle->hour) && (date.min == cycle->min) && (date.sec >= 0) && (date.sec < 2) && (cycle->state == AL_PUMPCYCLE_STOP) && (al_statusbits.menu_halt != true) ) {

    cycle->state = AL_PUMPCYCLE_START;
    cycle->start_ts = pumps_ts;
    cycle->next_ts = pumps_ts;
    cycle->active = true;
    cycle->cycles_count = 1;  // Start with first cycle immediately (we count in natural numbers ;)

    al_menuscreen = AL_MENUSCREEN_CYCLES;

    AQUALOOPDEBUG("%lu Go..\n", cycle->next_ts);

  }


  if (cycle->active == true)
  {

    /* Initialize cycle: start delay - suction - pause - backflush - pause */
    if (cycle->state == AL_PUMPCYCLE_START) {
      if ( pumps_ts - cycle->next_ts >= (uint32_t)cycle->duration_start ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_PUMPCYCLE_PUMPING;
        AQUALOOPDEBUG("%lu Begin Suction..\n", cycle->next_ts);
      }

      /* STOP */
      //setpwm('a', 0);
      //setpwm('b', 0);
      aqualoop_setsuction(0);
      aqualoop_setbackflush(0);
    }



    if (cycle->state == AL_PUMPCYCLE_PUMPING) {
      if ( pumps_ts - cycle->next_ts >= (uint32_t)cycle->duration_suction ) {  // Suction time is in minutes
        cycle->next_ts = pumps_ts;

        cycle->state = AL_PUMPCYCLE_PAUSE_1;
        AQUALOOPDEBUG("%lu Pause 1..\n", cycle->next_ts);
      }

      /* PUMPING SUCTION */
      //setpwm('a', mem_pwm[aqualoop_params_ram.MEM_number]);
      aqualoop_setsuction(mem_pwm[aqualoop_params_ram.MEM_number]);
      aqualoop_setbackflush(0);
    }



    if ( cycle->state == AL_PUMPCYCLE_PAUSE_1) {
      if ( pumps_ts - cycle->next_ts >= (uint32_t)cycle->duration_pause1 ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_PUMPCYCLE_BACKFLUSH;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* STOP */
      //setpwm('a', 0);
      //setpwm('b', 0);
      aqualoop_setsuction(0);
      aqualoop_setbackflush(0);
    }



    if (cycle->state == AL_PUMPCYCLE_BACKFLUSH) {
      if ( pumps_ts - cycle->next_ts >= (uint32_t)cycle->duration_backflush ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_PUMPCYCLE_PAUSE_2;
        AQUALOOPDEBUG("%lu Pause 2..\n", cycle->next_ts);
      }

      /* BACKFLUSH */
      //setpwm('b', 255);
      aqualoop_setsuction(0);
      aqualoop_setbackflush(255);
    }



    if ( cycle->state == AL_PUMPCYCLE_PAUSE_2) {
      if ( pumps_ts - cycle->next_ts >= (uint32_t)cycle->duration_pause2 ) {
        cycle->next_ts = pumps_ts;

        cycle->cycles_count++;    // One cycle is finished, increase counter 

        cycle->state = AL_PUMPCYCLE_PUMPING;
        AQUALOOPDEBUG("%lu Next Suction..\n", cycle->next_ts);
      }

      /* STOP */
      //setpwm('a', 0);
      //setpwm('b', 0);
      aqualoop_setsuction(0);
      aqualoop_setbackflush(0);
    }



    if (cycle->cycles_count > cycle->cycles_total) {
      cycle->cycles_count = cycle->cycles_total;
      cycle->state = AL_PUMPCYCLE_STOP; 
      AQUALOOPDEBUG("%lu Finish! \n", cycle->next_ts);
    }



    if (cycle->state == AL_PUMPCYCLE_STOP) {
      cycle->active = false;
      cycle->last_ts = clock_get_time();
      al_menuscreen = AL_MENUSCREEN_RETURN_INIT;
      AQUALOOPDEBUG("Total: %lu \n", cycle->last_ts - cycle->start_ts);
      AQUALOOPDEBUG("Estim: %lu \n", aqualoop_pumpcycle_calc_runtime(cycle) );

    }


  } /* if active */ 




}


void aqualoop_pumps_cycle_init(void) {

  /* Set parameter values from menu / EEPROM to current working variables */

  cycle_t1.state = AL_PUMPCYCLE_STOP;
  cycle_t1.active = false;

  cycle_t1.hour = aqualoop_params_ram.t1_hour;
  cycle_t1.min =  aqualoop_params_ram.t1_min;
  // TODO: Check if new setting is bigger than currently running value !!
  cycle_t1.cycles_total = aqualoop_params_ram.suction_c1;
  cycle_t1.cycles_count = 0;

  /* DURATIONS are NOT user-configurable: Define a fixed cycle duration here */
  cycle_t1.duration_start = 10;
  cycle_t1.duration_suction = 900;
  cycle_t1.duration_pause1 = 2;
  cycle_t1.duration_backflush = 15;
  cycle_t1.duration_pause2 = 5;



  cycle_t2.state = AL_PUMPCYCLE_STOP;
  cycle_t2.active = false;

  cycle_t2.hour = aqualoop_params_ram.t2_hour;
  cycle_t2.min =  aqualoop_params_ram.t2_min;
  // TODO: Check if new setting is bigger than currently running value !!
  cycle_t2.cycles_total = aqualoop_params_ram.suction_c2;
  cycle_t2.cycles_count = 0;

  /* DURATIONS are NOT user-configurable: Define a fixed cycle duration here */
  cycle_t2.duration_start = 10;
  cycle_t2.duration_suction = 900;  //15min
  cycle_t2.duration_pause1 = 2;
  cycle_t2.duration_backflush = 15; //15sec
  cycle_t2.duration_pause2 = 5;


  al_statusbits.menu_chemical = false;    // Chemiereinigung ist erstmal deaktiviert

}


void aqualoop_pumps_toggle_core(void)
{

  /* Set parameter values from menu / EEPROM to current working variables */
  cycle_t1.hour = aqualoop_params_ram.t1_hour;
  cycle_t1.min =  aqualoop_params_ram.t1_min;
  // TODO: Check if new setting is bigger than currently running value !!
  cycle_t1.cycles_total = aqualoop_params_ram.suction_c1;

  cycle_t2.hour = aqualoop_params_ram.t2_hour;
  cycle_t2.min =  aqualoop_params_ram.t2_min;
  // TODO: Check if new setting is bigger than currently running value !!
  cycle_t2.cycles_total = aqualoop_params_ram.suction_c2;


  /* Perform pumping cycles depending on parameter settings */

  // TODO: Conflicts if timers are changed and running processes
  if ( ( al_statusbits.menu_chemical != true ) /* && 
       ( al_statusbits.menu_halt != true )     */ ) {

    if (cycle_t2.state == AL_PUMPCYCLE_STOP )
      aqualoop_pumps_cycle(&cycle_t1);
    
    if (cycle_t1.state == AL_PUMPCYCLE_STOP )
      aqualoop_pumps_cycle(&cycle_t2);
    
  }



}














/*
 .o88b. db   db d88888b .88b  d88. d888888b  .o88b.  .d8b.  db      
d8P  Y8 88   88 88'     88'YbdP`88   `88'   d8P  Y8 d8' `8b 88      
8P      88ooo88 88ooooo 88  88  88    88    8P      88ooo88 88      
8b      88~~~88 88~~~~~ 88  88  88    88    8b      88~~~88 88      
Y8b  d8 88   88 88.     88  88  88   .88.   Y8b  d8 88   88 88booo. 
 `Y88P' YP   YP Y88888P YP  YP  YP Y888888P  `Y88P' YP   YP Y88888P 
*/

void aqualoop_cleaning_cycle(struct pumpcycle *cycle) {

  timestamp_t pumps_ts = clock_get_time();
  //timestamp_t pumps_ts = 100;

  /* Checks if we need to start a new cycle sequence programmed at this time */
  //                            if ( (date.min == cycle->min) && (date.sec = 00) && (date.sec <= 01) ) {
  if ( ( al_statusbits.menu_chemical == true ) && (cycle->state == AL_PUMPCYCLE_STOP) ) {

    cycle->state = AL_PUMPCYCLE_START;
    cycle->start_ts = pumps_ts;
    cycle->next_ts = pumps_ts;
    cycle->active = true;
    cycle->cycles_count = 1;  // Start with first cycle immediately (we count in natural numbers ;)

    cycle_t1.state = AL_PUMPCYCLE_STOP;
    cycle_t2.state = AL_PUMPCYCLE_STOP;
    cycle_t1.active = false;
    cycle_t2.active = false;

    al_menuscreen = AL_MENUSCREEN_CHEMRUNPROC;
    AQUALOOPDEBUG("%lu Go..\n", cycle->next_ts);

  }


  if (cycle->active == true)
  {

    /* Running cycle: Wait for 10s */
    if (cycle->state == AL_PUMPCYCLE_START) {
      if ( pumps_ts - cycle->next_ts >= 10 ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_001;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* Do something else ... */
      //setpwm('a', 0);
      //setpwm('b', 0);
      aqualoop_setsuction(0);
      aqualoop_setbackflush(0);
    }

    /* PART 1 */

    /* Running cycle: BACKFLUSH 120s ... 20s */
    if (cycle->state == AL_CLEANCYCLE_001) {
      if ( pumps_ts - cycle->next_ts >= mem_cleantime[aqualoop_params_ram.MEM_number] ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_002;
        al_menuscreen = AL_MENUSCREEN_WAITWATERFILL;
        AQUALOOPDEBUG("%lu Waiting for user confirm..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }


    /* Running cycle: WAIT for USER CONFIRM to fill in CHEMICAL */
    if (cycle->state == AL_CLEANCYCLE_002) {
      if ( al_statusbits.menu_input == true ) {  // user has pressed a button to confirm liquid 
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_003;
        al_menuscreen = AL_MENUSCREEN_CHEMRUNPROC;
        al_statusbits.menu_input = false;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* Do something else ... */
      //setpwm('a', mem_pwm[aqualoop_params_ram.MEM_number]);
    }


    /* Running cycle: 10s BACKFLUSH */
    if ( cycle->state == AL_CLEANCYCLE_003) {
      if ( pumps_ts - cycle->next_ts >= 10 ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_004;
        AQUALOOPDEBUG("%lu Pause..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }

// 5 TIMES
    /* Running cycle: 10min PAUSE */
    if (cycle->state == AL_CLEANCYCLE_004) {
      if ( pumps_ts - cycle->next_ts >= 600 ) { // 600 = 10 min, 5x 600 = 3000
        cycle->next_ts = pumps_ts;

        cycle->cycles_count++;

        cycle->state = AL_CLEANCYCLE_005;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }


    /* Running cycle: 3sec BACKFLUSH */
    if ( cycle->state == AL_CLEANCYCLE_005) {
      if ( pumps_ts - cycle->next_ts >= 3 ) {
        cycle->next_ts = pumps_ts;

        if (cycle->cycles_count > 5) {
          cycle->state = AL_CLEANCYCLE_006;
          cycle->cycles_count = 1;
          AQUALOOPDEBUG("%lu Pause..\n", cycle->next_ts);
        }
        else
          cycle->state = AL_CLEANCYCLE_004;
      }

      /* Do something else ... */

    }

// END 5 TIMES

    /* Running cycle: 5min PAUSE */
    if ( cycle->state == AL_CLEANCYCLE_006) {
      if ( pumps_ts - cycle->next_ts >= 300 ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_007;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }


    /* Running cycle: 10sec BACKFLUSH */
    if ( cycle->state == AL_CLEANCYCLE_007) {
      if ( pumps_ts - cycle->next_ts >= 10 ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_008;
        AQUALOOPDEBUG("%lu Suction..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }

// 6 TIMES

    /* Running cycle: 60sec SUCTION */
    if ( cycle->state == AL_CLEANCYCLE_008) {
      if ( pumps_ts - cycle->next_ts >= 60 ) {
        cycle->next_ts = pumps_ts;

        cycle->cycles_count++;

        cycle->state = AL_CLEANCYCLE_009;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }


    /* Running cycle: 10sec BACKFLUSH */
    if ( cycle->state == AL_CLEANCYCLE_009) {
      if ( pumps_ts - cycle->next_ts >= 10 ) {
        cycle->next_ts = pumps_ts;

        if (cycle->cycles_count > 6) {
          cycle->state = AL_CLEANCYCLE_010;
          al_menuscreen = AL_MENUSCREEN_WAITWATERFILL;
          cycle->cycles_count = 1;
          AQUALOOPDEBUG("%lu Waiting for user confirm..\n", cycle->next_ts);
        }
        else
          cycle->state = AL_CLEANCYCLE_008;
      }

      /* Do something else ... */

    }

// END 6 TIMES

    /* PART 2 */

    /* Running cycle: CLEANING COMPLETE: WAIT for USER CONFIRM to fill in CLEAN WATER */

    /* Running cycle: WAIT for USER CONFIRM to fill in CLEAN WATER */
    if (cycle->state == AL_CLEANCYCLE_010) {
      if ( al_statusbits.menu_input == true ) {  // user has pressed a button to confirm liquid 
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_011;
        al_menuscreen = AL_MENUSCREEN_CHEMRUNPROC;
        al_statusbits.menu_input = false;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* Do something else ... */
      //setpwm('a', mem_pwm[aqualoop_params_ram.MEM_number]);
    }


    /* Running cycle: BACKFLUSH 60s .. 10s */
    if ( cycle->state == AL_CLEANCYCLE_011) {
      if ( pumps_ts - cycle->next_ts >= mem_cleantime[aqualoop_params_ram.MEM_number] / 2 ) {
        cycle->next_ts = pumps_ts;

        cycle->state = AL_CLEANCYCLE_012;
        AQUALOOPDEBUG("%lu Suction..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }

// 5 TIMES

    /* Running cycle: 60s SUCTION */
    if ( cycle->state == AL_CLEANCYCLE_012) {
      if ( pumps_ts - cycle->next_ts >= 60 ) {
        cycle->next_ts = pumps_ts;

        cycle->cycles_count++;

        cycle->state = AL_CLEANCYCLE_013;
        AQUALOOPDEBUG("%lu Backflush..\n", cycle->next_ts);
      }

      /* Do something else ... */

    }


    /* Running cycle: 10s BACKFLUSH */
    if ( cycle->state == AL_CLEANCYCLE_013) {
      if ( pumps_ts - cycle->next_ts >= 10 ) {
        cycle->next_ts = pumps_ts;

        if (cycle->cycles_count > 5) {
          cycle->cycles_count = 1;
          cycle->state = AL_CLEANCYCLE_014;
          al_menuscreen = AL_MENUSCREEN_WAITWATERFILL;
          AQUALOOPDEBUG("%lu Wait for ok..\n", cycle->next_ts);
        }
        else
          cycle->state = AL_CLEANCYCLE_012;

      }

      /* Do something else ... */

    }

// END 5 TIMES

    /* Running cycle: WAIT for USER CONFIRM to finish cleaning cycle */
    if (cycle->state == AL_CLEANCYCLE_014) {
      if ( al_statusbits.menu_input == true ) {  // user has pressed a button to confirm liquid 
        cycle->next_ts = pumps_ts;

        cycle->state = AL_PUMPCYCLE_STOP;
        al_menuscreen = AL_MENUSCREEN_RETURN_INIT;
        al_statusbits.menu_input = false;
        AQUALOOPDEBUG("%lu Finish..\n", cycle->next_ts);
      }

      /* Do something else ... */
      //setpwm('a', mem_pwm[aqualoop_params_ram.MEM_number]);
    }



    /* Running cycle: START DELAY */
    if (cycle->state == AL_PUMPCYCLE_STOP) {
      cycle->active = false;
      cycle->last_ts = clock_get_time();

      al_statusbits.menu_chemical = false;

      AQUALOOPDEBUG("Total: %lu \n", cycle->last_ts - cycle->start_ts);
      //AQUALOOPDEBUG("Estim: %lu \n", aqualoop_pumpcycle_calc_runtime(cycle) );

      /* Do something else ... */
      //setpwm('a', 0);
      //setpwm('b', 0);
      aqualoop_setsuction(0);
      aqualoop_setbackflush(0);
    }


  } /* if active */ 


}



void aqualoop_cleaning_cycle_init(void) {

  cycle_chem.state = AL_PUMPCYCLE_STOP;
  al_statusbits.menu_chemical = false;      // NO running chemical at the moment
  al_statusbits.menu_input = false;         // NO user input wait at the moment


}


void aqualoop_cleaning_toggle_core(void) {

  aqualoop_cleaning_cycle(&cycle_chem);

}
























/*
void check_akku(void) {

  // Falls Akkuspannung < 9,6 Volt ist, dann soll der Akku geladen werden und die LED blinkt
  if ((aqualoop_get_9v() < 961) && !al_sensors.akku ) {

	al_statusbits.lade_akku = true;    // Status speichern in Flag
	PIN_SET(LADE_AKKU);			           // Aktiviere den Ladecontroller
  } else {
	al_statusbits.lade_akku = false;
	PIN_CLEAR(LADE_AKKU);
  }

}
*/

void aqualoop_heartbeat_timer(void) {

  /* LED leuchtet oder blinkt je nach Status der Ladecontrolle */
  //if (al_statusbits.lade_akku == true) {
	PIN_TOGGLE(STATUSLED_HB_ACT);
  //} else {
	//PIN_SET(STATUSLED_HB_ACT);
  //}

}





/* Wird einmal pro Sekunde aufgerufen */
void aqualoop_long_timer(void) {

  static uint16_t lt_counter = 0;

  lt_counter++;

  /* alle 20 Sekunden */
  if (lt_counter % 20 == 0) {
	 //check_akku();
  }

  /* ca alle 10 Minuten */
  if (lt_counter % 600 == 0) {
  	lt_counter = 0;
  }

}










/*
  -- Ethersex META --
   header(services/intewa/aqualoop/aqualoop_core.h)
   timer(25, aqualoop_heartbeat_timer())
   timer(50, aqualoop_long_timer())
*/


