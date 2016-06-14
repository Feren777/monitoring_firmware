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


#include "services/clock/clock.h"




typedef struct ALstatusBits {    
  unsigned int lade_akku:1;		    // 0=Normalbetrieb, 1=Akku laden
  unsigned int pumping:1;         // 0=All off, 1=Pumps running (any of them)
  unsigned int menu_halt:1;       // 1=Menu Setting activity, do not perform actions on sensible (time) routines 
  unsigned int menu_input:1;      // 1=Wait for user input from menu
  unsigned int menu_chemical:1;   // 1=Start of chemical cleaning
  unsigned int free5:1;
  unsigned int free6:1;
  unsigned int free7:1;
} al_statbitflags_t;

extern al_statbitflags_t al_statusbits;




#define AL_PUMPCYCLE_STOP       0xFF 
#define AL_PUMPCYCLE_START      0xEE
#define AL_PUMPCYCLE_PUMPING    0x02 
#define AL_PUMPCYCLE_BACKFLUSH  0x04
#define AL_PUMPCYCLE_PAUSE_1    0x08 
#define AL_PUMPCYCLE_PAUSE_2    0x10
#define AL_PUMPCYCLE_PAUSE_3    0x20


#define AL_CLEANCYCLE_001       0x01  // XXs backflush depending on amount of membranes
#define AL_CLEANCYCLE_002       0x02  // Wait for CHEMICAL liquid fill in by user
#define AL_CLEANCYCLE_003       0x03  // 10s backflush
#define AL_CLEANCYCLE_004       0x04  // 10m pause to let liquid do its job inside membrane
#define AL_CLEANCYCLE_005       0x05  // 3s backflush
#define AL_CLEANCYCLE_006       0x06  // 5m pause
#define AL_CLEANCYCLE_007       0x07  // 10s backflush
#define AL_CLEANCYCLE_008       0x08  // 60s suction
#define AL_CLEANCYCLE_009       0x09  // 10s backflush

#define AL_CLEANCYCLE_010       0x0A  // Wait for CLEANWATER liquid fill in by user
#define AL_CLEANCYCLE_011       0x0B  // XXs backflush depending on amount of membranes
#define AL_CLEANCYCLE_012       0x0C  // 60s suction
#define AL_CLEANCYCLE_013       0x0D  // 10s backflush

#define AL_CLEANCYCLE_014       0x0E  // Wait for ok to return to initial screen (when finished)

#define AL_CLEANCYCLE_015       0x0F
#define AL_CLEANCYCLE_016       0x10


struct pumpcycle {
  uint8_t state;      /* Aktueller Status des Pumpzyklus */

  uint8_t hour;       /* Startzeit */
  uint8_t min;

  uint16_t duration_start;
  uint16_t duration_suction;
  uint16_t duration_pause1;
  uint16_t duration_backflush;
  uint16_t duration_pause2;

  uint8_t cycles_count;   /* Laufendes Intervall */
  uint8_t cycles_total;   /* Anzahl der Intervalle pro Pumpzyklus */

  timestamp_t start_ts;   /* UNIX Timestamp des STARTS eines neuen Zyklus */
  timestamp_t next_ts;    /* UNIX Timestamp des WECHSELS eines Zustands innerhalb des Zyklus */
  timestamp_t last_ts;    /* UNIX Timestamp des ENDE eines Zyklus */

  unsigned int active:1;	/* 0=Aus, 1=Pumpzyklus aktiv */

};


extern struct pumpcycle cycle_t1;
extern struct pumpcycle cycle_t2;

extern struct pumpcycle cycle_chem;


extern const uint8_t mem_cleantime[];



/* Pumpen periodisch an/aus schalten */
void aqualoop_pumps_cycle_init(void);
void aqualoop_pumps_cycle(struct pumpcycle *cycle);
void aqualoop_pumps_toggle_core(void);
uint32_t aqualoop_pumpcycle_calc_runtime(struct pumpcycle *cycle);

/* Reinigungszyklus mit Chemie */
void aqualoop_cleaning_cycle_init(void);
void aqualoop_cleaning_cycle(struct pumpcycle *cycle);
void aqualoop_cleaning_toggle_core(void);

/* Luftpumpe periodisch an/aus schalten */
void aqualoop_air_toggle_core(void);

/* Schlammpumpe periodisch an/aus schalten */
void aqualoop_sludge_toggle_core(void);

/* Ladecontrolle über LED1*/
void aqualoop_heartbeat_timer(void);

/* Interner Timer wird 1x pro Sekunde aufgerufen */
void aqualoop_long_timer(void);





