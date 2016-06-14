/*
* menu functions of the AQUALOOP Control Unit
* not intended for use from outside the control unit (monitoring)
* main menu functions
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

/**
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
**/

#include "aqualoop_menu.h"
#include "lcd_menu.h"
#include "myownfakes.h"
/**
#include "config.h"
#include "core/bit-macros.h"
#include "core/debug.h"
#include "core/eeprom.h"

#include "aqualoop.h"
#include "aqualoop_core.h"
#include "aqualoop_sensors.h"
//#include "aqualoop_gui.h"
#include "aqualoop_menu.h"
#include "services/lcd-menu/lcd_menu.h"

#include "hardware/lcd/hd44780.h"
#include "hardware/adc/adc.h"
#include "services/clock/clock.h"
#include "services/clock/clock_ecmd_date.h"

#include "protocols/uip/uip.h"
#include "protocols/uip/parse.h"
#include "protocols/dns/resolv.h"
#include "services/ntp/ntp.h"
**/


//#include "services/lcd-menu/menu_prompt.h"


// dk6yf@precision:~$ figlet -d ~/.figlet/ -f basic -w200 -k "Sections"

/*
.d8888. d8888b. d88888b  .o88b. d888888b  .d8b.  db         .o88b. db   db  .d8b.  d8888b. .d8888. 
88'  YP 88  `8D 88'     d8P  Y8   `88'   d8' `8b 88        d8P  Y8 88   88 d8' `8b 88  `8D 88'  YP 
`8bo.   88oodD' 88ooooo 8P         88    88ooo88 88        8P      88ooo88 88ooo88 88oobY' `8bo.   
  `Y8b. 88~~~   88~~~~~ 8b         88    88~~~88 88        8b      88~~~88 88~~~88 88`8b     `Y8b. 
db   8D 88      88.     Y8b  d8   .88.   88   88 88booo.   Y8b  d8 88   88 88   88 88 `88. db   8D 
`8888Y' 88      Y88888P  `Y88P' Y888888P YP   YP Y88888P    `Y88P' YP   YP YP   YP 88   YD `8888Y' 
*/

uint8_t al_char_arrup[8] = {
	0b11111,
	0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

uint8_t al_char_arrdown[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100,
	0b11111
};

uint8_t al_char_triangelright[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00100,
	0b01110,
	0b11111,
	0b00000
};


/*
db    db  .d8b.  d8888b. d888888b  .d8b.  d8888b. db      d88888b .d8888. 
88    88 d8' `8b 88  `8D   `88'   d8' `8b 88  `8D 88      88'     88'  YP 
Y8    8P 88ooo88 88oobY'    88    88ooo88 88oooY' 88      88ooooo `8bo.   
`8b  d8' 88~~~88 88`8b      88    88~~~88 88~~~b. 88      88~~~~~   `Y8b. 
 `8bd8'  88   88 88 `88.   .88.   88   88 88   8D 88booo. 88.     db   8D 
   YP    YP   YP 88   YD Y888888P YP   YP Y8888P' Y88888P Y88888P `8888Y' 
*/


clock_datetime_t tmp_date_local;  // stores the manual set time/date values
timestamp_t tmp_ts_local;         // stores converted manual time to unix timestamp

uint8_t addr_blocks[4] = {0, 0, 0, 0};
uip_ipaddr_t temp_ip_addr;        // stores temporary ip_address     
uip_ipaddr_t hostaddr;            // holds current ip_address

/* Initialize the variables at menu startup */
void al_menu_init_variables(void) {

  // Start input of date at 01.01.2010
  tmp_date_local.day = 1;
  tmp_date_local.month = 1;
  tmp_date_local.year = 110;
  
  // Start with 00:00 hour
  tmp_date_local.hour = 0;
  tmp_date_local.min = 0;
}







void al_menu_akku_display (void) {
  hd44780_goto(0, 0);
  char akku1_buf[20];
  sprintf (akku1_buf, " --- ATTENTION! --- ");
  fputs (akku1_buf, &lcd);

  hd44780_goto(1, 0);
  char akku2_buf[20];
  sprintf (akku2_buf, "System Power FAILURE");
  fputs (akku2_buf, &lcd);

  hd44780_goto(2, 0);
  char akku3_buf[20];
  sprintf (akku3_buf, "All actions disabled");
  fputs (akku3_buf, &lcd);
/*
  hd44780_goto(3, 0);
  char volt9_buf[20];
  sprintf (volt9_buf, " -- Accu:%2d.%02d V -- ", measure_to_int (aqualoop_get_9v()) , measure_to_dec (aqualoop_get_9v())   );
  fputs (volt9_buf, &lcd);
*/
}




void al_menu_sensors_display (void) {
  hd44780_goto(0, 0);
  char volt24a_buf[20];
  sprintf (volt24a_buf, "   24V: %4d", aqualoop_get_24v()   );
  fputs (volt24a_buf, &lcd);

  hd44780_goto(1, 0);  char volt24_buf[20];
  sprintf (volt24_buf, "Supply: %2d.%02d V", measure_to_int (aqualoop_get_24v()) , measure_to_dec (aqualoop_get_24v())   );
  fputs (volt24_buf, &lcd);
/*
  hd44780_goto(2, 0);
  char volt9a_buf[20];
  sprintf (volt9a_buf, "    9V: %4d", aqualoop_get_9v()   );
  fputs (volt9a_buf, &lcd);

  hd44780_goto(3, 0);
  char volt9_buf[20];
  sprintf (volt9_buf,  "  Akku: %2d.%02d V", measure_to_int (aqualoop_get_9v()) , measure_to_dec (aqualoop_get_9v())   );
  fputs (volt9_buf, &lcd);
*/
}

void al_menu_sensors_current_display (void) {
  hd44780_goto(0, 0);  char ampsI0a_buf[20];
  //sprintf (ampsI0a_buf, "Isens0: %2d.%03u A", (aqualoop_get_current(AQUALOOP_ADC_CURR_SENS_0)/1000L), (aqualoop_get_current(AQUALOOP_ADC_CURR_SENS_0)%1000L) );
  sprintf (ampsI0a_buf, "Isens0: %6d mA", aqualoop_get_current(AQUALOOP_ADC_CURR_SENS_0,  -1) );
  fputs (ampsI0a_buf, &lcd);

  hd44780_goto(1, 0);  char ampsI0b_buf[20];

  //sprintf (ampsI0b_buf, "Isens1: %2d.%03u A", (aqualoop_get_current(AQUALOOP_ADC_CURR_SENS_1)/1000L), (aqualoop_get_current(AQUALOOP_ADC_CURR_SENS_1)%1000L) );
  sprintf (ampsI0b_buf, "Isens1: %6d mA", aqualoop_get_current(AQUALOOP_ADC_CURR_SENS_1, -6) );
  fputs (ampsI0b_buf, &lcd);


  hd44780_goto(2, 0);
  char time_buf [20];
  sprintf (time_buf,"%02d:%02d:%02dh %02d.%02d.%4d", date.hour, date.min, date.sec, date.day, date.month, 1900+date.year);
  fputs (time_buf, &lcd);

  hd44780_goto(3, 0);
  char ts_buf [20];
  sprintf (ts_buf,"Uptime: %lu", clock_get_uptime());
  fputs (ts_buf, &lcd);
}








/*
.88b  d88. d88888b d8b   db db    db   d88888b db    db d8b   db  .o88b. d888888b d888888b  .d88b.  d8b   db .d8888. 
88'YbdP`88 88'     888o  88 88    88   88'     88    88 888o  88 d8P  Y8 `~~88~~'   `88'   .8P  Y8. 888o  88 88'  YP 
88  88  88 88ooooo 88V8o 88 88    88   88ooo   88    88 88V8o 88 8P         88       88    88    88 88V8o 88 `8bo.   
88  88  88 88~~~~~ 88 V8o88 88    88   88~~~   88    88 88 V8o88 8b         88       88    88    88 88 V8o88   `Y8b. 
88  88  88 88.     88  V888 88b  d88   88      88b  d88 88  V888 Y8b  d8    88      .88.   `8b  d8' 88  V888 db   8D 
YP  YP  YP Y88888P VP   V8P ~Y8888P'   YP      ~Y8888P' VP   V8P  `Y88P'    YP    Y888888P  `Y88P'  VP   V8P `8888Y' 

####################################################################################################################
*/


/* DEFAULT or initial write function for menu-system.
 * Required only at startup for the first init of the menu.
 */
void generic_write(const char* text) {
  fputs_P (text, &lcd);
}


/* NAVIGATION functions assigned to the buttons */
void nav_next (void) { /*hd44780_clear();*/ lcdm_navigate(MENU_NEXT); }
void nav_down (void) { /*hd44780_clear();*/ lcdm_navigate(MENU_CHILD); }
void nav_up   (void) { /*hd44780_clear();*/ lcdm_navigate(MENU_PARENT); }
void nav_null (void) { lcdm_navigate(current_menu_item); }
void set_para (void) { eeprom_save(aqualoop_params, &aqualoop_params_ram, sizeof(aqualoop_params_t)); eeprom_update_chksum(); /*hd44780_clear();*/ lcdm_navigate(MENU_CHILD); }

/* Some special case functions for setting individual parameters */
void set_clock (void) { 
  tmp_ts_local = clock_mktime(&tmp_date_local, 1);
  clock_set_time(tmp_ts_local);
  /*hd44780_clear();*/ lcdm_navigate(MENU_CHILD);
}


/* DRAWING functions common to some menu features.
 * Only global and reusable functions are listed here
 */

/* Required to erase the content of a single row only */
void al_draw_menu_blankline(uint8_t line) {

  hd44780_goto(line, 0);
              //12345678901234567890
  hd44780_puts("                    ");  
}

/* Display the SAVE option at bottom */
void al_draw_submenu_save(void) {

  hd44780_goto(3, 0);
  //            12345678901234567890
  hd44780_puts("NEXT:Edit ENTER:Save");

}

/* Puts the name of the submenu in the header (first row) and 
 * the currently selected menu-item in the second row.
 * Static navigation hints are displayed at the bottom.
 */
void al_list_submenus_display(void) {

  if (lcdm_write_function) {
    hd44780_goto(0, 0);
    lcdm_write_function(lcdm_get_parent_menu()->text);
    //if (lcdm_write_function) lcdm_write_function(lcdm_get_parent_menu_text(current_menu_item));

    hd44780_goto(1, 0);
    lcdm_write_function(lcdm_get_current_menu()->text);

    //menu_item_t* next_item = lcdm_get_current_menu()->next;

    //menu_item_t* child_item = next_item->child;

    //hd44780_goto(2, 0);
    //if (lcdm_get_current_menu() ==  lcdm_get_child_menu() )
    //   hd44780_puts("TOP");

    al_draw_menu_blankline(2);

    hd44780_goto(3, 0);
    //            12345678901234567890
    hd44780_puts("NEXT:Next  ENTER:Set");

  }
 
}

/* PARAMETER setting: Displays the title of current parameter
 * in the first row.
 */
void al_draw_submenu_item(void) {
  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);
  al_draw_menu_blankline(1);
}

//UNUSED !!
void al_draw_submenu_title(void) {

  if (lcdm_write_function) {
    hd44780_goto(0, 0);
    lcdm_write_function(lcdm_get_parent_menu()->text);
    hd44780_goto(1, 0);
    lcdm_write_function(lcdm_get_current_menu()->text);
  }

}








/*
d8888b. db    db d8b   db  .d8b.  .88b  d88. d888888b  .o88b.   .d8888.  .o88b. d8888b. d88888b d88888b d8b   db .d8888. 
88  `8D `8b  d8' 888o  88 d8' `8b 88'YbdP`88   `88'   d8P  Y8   88'  YP d8P  Y8 88  `8D 88'     88'     888o  88 88'  YP 
88   88  `8bd8'  88V8o 88 88ooo88 88  88  88    88    8P        `8bo.   8P      88oobY' 88ooooo 88ooooo 88V8o 88 `8bo.   
88   88    88    88 V8o88 88~~~88 88  88  88    88    8b          `Y8b. 8b      88`8b   88~~~~~ 88~~~~~ 88 V8o88   `Y8b. 
88  .8D    88    88  V888 88   88 88  88  88   .88.   Y8b  d8   db   8D Y8b  d8 88 `88. 88.     88.     88  V888 db   8D 
Y8888D'    YP    VP   V8P YP   YP YP  YP  YP Y888888P  `Y88P'   `8888Y'  `Y88P' 88   YD Y88888P Y88888P VP   V8P `8888Y' 
*/


void al_basis_info_display(void) {

//                 "13:24:59z |T1=00:00h"
//                 " A= 4 / 9 |T2=00:00h"
//                 "AIR ON    |T3=00:00h"
//                 "BREAK     |A1=2     "
//                 "SO = ON (no suction)"

//                 "13:00:00h |T1=00:00h"
//                 "AIR ON    |T2=00:00h"
//                 "Break A1=2|T3=00:00h"
//                 "BRmin ^   |CLmax -  "


  hd44780_define_char(1, al_char_arrup, 1);
  hd44780_define_char(2, al_char_arrdown, 1);

  hd44780_goto(0, 0);
  char time_buf [20];
  sprintf (time_buf,"%02d:%02d:%02dh | C=%2d /%2d", date.hour, date.min, date.sec, aqualoop_params_ram.suction_c1, aqualoop_params_ram.suction_c2);
  fputs (time_buf, &lcd);

  hd44780_goto(1, 0);
  char tmr_buf [20];
  sprintf (tmr_buf,"T1=%02d:%02dh |T2=%02d:%02dh", aqualoop_params_ram.t1_hour, aqualoop_params_ram.t1_min, aqualoop_params_ram.t2_hour, aqualoop_params_ram.t2_min);
  fputs (tmr_buf, &lcd);

  hd44780_goto(2, 0);
  char air_buf [20];
                // "AIR ON    |T3=00:00h"
  sprintf (air_buf,"AIR %s             ", (PIN_HIGH(AL_COMPRESSOR_AIR) ? "ON " : "OFF") );
  fputs (air_buf, &lcd);

  hd44780_goto(3, 0);
  char val_buf [20];
  sprintf (val_buf,"BRmin %c   |CLmax %c  ", (al_sensors.SSBRmin ? 2:1), (al_sensors.SSKlarmax ? 2:1) );
  fputs (val_buf, &lcd);
}





void al_menu_timers_display(void) {
  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);

  al_draw_menu_blankline(1);
  al_draw_menu_blankline(2);
  al_draw_menu_blankline(3);

}




void al_menu_cycles_display(void) {
  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);

  al_draw_menu_blankline(1);
  al_draw_menu_blankline(2);
  al_draw_menu_blankline(3);

}





void al_menu_time_display(void) {
  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);

  char date_output[30];
  parse_cmd_date(NULL_FUNC, date_output, 30);

  hd44780_goto(1, 0);
  hd44780_puts("Date: ");
  char date_buf [15];
  memcpy(date_buf, &date_output[0], 14 );
  date_buf[14] = '\0';
  fputs (date_buf, &lcd);

  hd44780_goto(2, 0);
  hd44780_puts("Time: ");
  char time_buf [15];
  memcpy(time_buf, &date_output[15], 15 );
  time_buf[14] = '\0';
  fputs (time_buf, &lcd);

  hd44780_goto(3, 0);
  char ts_buf [20];
  sprintf (ts_buf,"Unix: %-14lu", clock_get_time());
  fputs (ts_buf, &lcd);
}







void al_menu_network_1_display (void) {

  //uip_ipaddr_t hostaddr;
  //uip_gethostaddr(&hostaddr);

  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);

  hd44780_goto(1, 0);
  char net1_buf[20];
  memset(&net1_buf[0], ' ', sizeof(net1_buf));
  sprintf(net1_buf, " IP:                ");
  //uint8_t len1 = print_ipaddr(&hostaddr, net1_buf + 5, 16);
  uint8_t len1 = print_ipaddr(&uip_hostaddr, net1_buf + 5, 16);
  if (len1 < 15) net1_buf[len1 + 5] = ' ';
  fputs (net1_buf, &lcd);

  hd44780_goto(2, 0);
  char net2_buf[20];
  memset(&net2_buf[0], ' ', sizeof(net2_buf));
  sprintf(net2_buf, "GWY:                ");
  uint8_t len2 = print_ipaddr(&uip_draddr, net2_buf + 5, 16);
  if (len2 < 15) net2_buf[len2 + 5] = ' ';
  fputs (net2_buf, &lcd);

  al_draw_menu_blankline(3);
}

void al_menu_network_2_display (void) {
  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);

  hd44780_goto(1, 0);
  char net3_buf[20];
  memset(&net3_buf[0], ' ', sizeof(net3_buf));
  sprintf(net3_buf, "DNS:                ");
  uint8_t len3 = print_ipaddr(resolv_getserver(), net3_buf + 5, 16);
  if (len3 < 15) net3_buf[len3 + 5] = ' ';
  fputs (net3_buf, &lcd);

  hd44780_goto(2, 0);
  char net4_buf[20];
  memset(&net4_buf[0], ' ', sizeof(net4_buf));
  sprintf(net4_buf, "NTP:                ");
  uint8_t len4 = print_ipaddr(ntp_getserver(), net4_buf + 5, 16);
  if (len4 < 15) net4_buf[len4 + 5] = ' ';
  fputs (net4_buf, &lcd);

  al_draw_menu_blankline(3);
}




void al_menu_monitor_display (void) {

  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);

  hd44780_goto(3, 0);
  char mon_buf [20];
                   //12345678901234567890
  sprintf (mon_buf, "Telematics=[OFF]    " );
  fputs (mon_buf, &lcd);


}






/*

.##.....##.########.##....##.##.....##.....######..########.########.########.####.##....##..######....######.
.###...###.##.......###...##.##.....##....##....##.##..........##.......##.....##..###...##.##....##..##....##
.####.####.##.......####..##.##.....##....##.......##..........##.......##.....##..####..##.##........##......
.##.###.##.######...##.##.##.##.....##.....######..######......##.......##.....##..##.##.##.##...####..######.
.##.....##.##.......##..####.##.....##..........##.##..........##.......##.....##..##..####.##....##........##
.##.....##.##.......##...###.##.....##....##....##.##..........##.......##.....##..##...###.##....##..##....##
.##.....##.########.##....##..#######......######..########....##.......##....####.##....##..######....######.

##############################################################################################################
*/

/*
 .d8b.  db             d8888b.  .d8b.  d8888b.  .d8b.  .88b  d88. d88888b d888888b d88888b d8888b. .d8888. 
d8' `8b 88             88  `8D d8' `8b 88  `8D d8' `8b 88'YbdP`88 88'     `~~88~~' 88'     88  `8D 88'  YP 
88ooo88 88             88oodD' 88ooo88 88oobY' 88ooo88 88  88  88 88ooooo    88    88ooooo 88oobY' `8bo.   
88~~~88 88      C8888D 88~~~   88~~~88 88`8b   88~~~88 88  88  88 88~~~~~    88    88~~~~~ 88`8b     `Y8b. 
88   88 88booo.        88      88   88 88 `88. 88   88 88  88  88 88.        88    88.     88 `88. db   8D 
YP   YP Y88888P        88      YP   YP 88   YD YP   YP YP  YP  YP Y88888P    YP    Y88888P 88   YD `8888Y' 
*/

/* MEM */
void al_menu_change_MEM_number(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.MEM_number > 6) aqualoop_params_ram.MEM_number = 1;
                  //12345678901234567890
  sprintf (tmp_buf,"Set MEM=[%u]         ", aqualoop_params_ram.MEM_number);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_MEM (void) { aqualoop_params_ram.MEM_number++; }

/* SO */
void al_menu_change_SO_number(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.suction_overflow > 4) aqualoop_params_ram.suction_overflow = 0;
                  //12345678901234567890
  sprintf (tmp_buf,"Set SO=[%u]          ", aqualoop_params_ram.suction_overflow);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_SO (void) { aqualoop_params_ram.suction_overflow++; }

/* SLPw */
void al_menu_change_SLP_week(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.sludge_pump_weeks > 15) aqualoop_params_ram.sludge_pump_weeks = 0;
  sprintf (tmp_buf,"Set SLP/t=[%2u]/%02u   ", aqualoop_params_ram.sludge_pump_weeks, aqualoop_params_ram.sludge_pump_minutes);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_SLPw (void) { aqualoop_params_ram.sludge_pump_weeks++; }

/* SLPw */
void al_menu_change_SLP_day(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.sludge_pump_days > 7) aqualoop_params_ram.sludge_pump_days = 0;
  sprintf (tmp_buf,"Set SLP/t=[%2u]/%02u   ", aqualoop_params_ram.sludge_pump_days, aqualoop_params_ram.sludge_pump_minutes);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_SLPd (void) { aqualoop_params_ram.sludge_pump_days++; }

/* SLPm */
void al_menu_change_SLP_min(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.sludge_pump_minutes > 60) aqualoop_params_ram.sludge_pump_minutes = 0;
  sprintf (tmp_buf,"Set SLP/t=%2u/[%02u]   ", aqualoop_params_ram.sludge_pump_days, aqualoop_params_ram.sludge_pump_minutes);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_SLPm (void) { aqualoop_params_ram.sludge_pump_minutes++; }

/* C1 */
void al_menu_change_C1_number(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.suction_c1 > 25) aqualoop_params_ram.suction_c1 = 1;
  sprintf (tmp_buf,"Set C1=[%2u]/%2u      ", aqualoop_params_ram.suction_c1, aqualoop_params_ram.suction_c2);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_C1 (void) { aqualoop_params_ram.suction_c1++; }

/* C2 */
void al_menu_change_C2_number(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.suction_c2 > 25) aqualoop_params_ram.suction_c2 = 1;
  sprintf (tmp_buf,"Set C2=%2u/[%2u]      ", aqualoop_params_ram.suction_c1, aqualoop_params_ram.suction_c2);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_C2 (void) { aqualoop_params_ram.suction_c2++; }

/* AIR ON */
void al_menu_change_AIR_on(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.air_on > 15) aqualoop_params_ram.air_on = 0;
  sprintf (tmp_buf,"Set AIR ON=[%02u]/%02u  ", aqualoop_params_ram.air_on, aqualoop_params_ram.air_off);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_AIRon (void) { aqualoop_params_ram.air_on++; }

/* AIR OFF */
void al_menu_change_AIR_off(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.air_off > 15) aqualoop_params_ram.air_off = 0;
                  //12345678901234567890
                  //Set AIR OFF=[00]:00
  sprintf (tmp_buf,"Set AIR OFF=%02u/[%02u] ", aqualoop_params_ram.air_on, aqualoop_params_ram.air_off);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_AIRoff (void) { aqualoop_params_ram.air_off++; }

/* T1 hour */
void al_menu_change_T1_hour(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.t1_hour > 23) aqualoop_params_ram.t1_hour = 0;
  sprintf (tmp_buf,"Set T1 hour=[%02u]:%02uh", aqualoop_params_ram.t1_hour, aqualoop_params_ram.t1_min);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_T1h (void) { aqualoop_params_ram.t1_hour++; }

/* T1 min */
void al_menu_change_T1_min(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.t1_min > 59) aqualoop_params_ram.t1_min = 0;
  sprintf (tmp_buf,"Set T1 min=%02u:[%02u]h ", aqualoop_params_ram.t1_hour, aqualoop_params_ram.t1_min);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_T1m (void) { aqualoop_params_ram.t1_min++; }

/* T2 hour */
void al_menu_change_T2_hour(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.t2_hour > 23) aqualoop_params_ram.t2_hour = 0;
  sprintf (tmp_buf,"Set T2 hour=[%02u]:%02uh", aqualoop_params_ram.t2_hour, aqualoop_params_ram.t2_min);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_T2h (void) { aqualoop_params_ram.t2_hour++; }

/* T2 min */
void al_menu_change_T2_min(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (aqualoop_params_ram.t2_min > 59) aqualoop_params_ram.t2_min = 0;
                    //12345678901234567890
                    //Set T2 min=[00]:00h
  sprintf (tmp_buf,"Set T2 min=%02u:[%02u]h ", aqualoop_params_ram.t2_hour, aqualoop_params_ram.t2_min);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_T2m (void) { aqualoop_params_ram.t2_min++; }


/*
 .o88b. db       .d88b.   .o88b. db   dD   .d888b.    d8888b.  .d8b.  d888888b d88888b 
d8P  Y8 88      .8P  Y8. d8P  Y8 88 ,8P'   8P   8D    88  `8D d8' `8b `~~88~~' 88'     
8P      88      88    88 8P      88,8P     `Vb d8'    88   88 88ooo88    88    88ooooo 
8b      88      88    88 8b      88`8b      d88C dD   88   88 88~~~88    88    88~~~~~ 
Y8b  d8 88booo. `8b  d8' Y8b  d8 88 `88.   C8' d8D    88  .8D 88   88    88    88.     
 `Y88P' Y88888P  `Y88P'   `Y88P' YP   YD   `888P Yb   Y8888D' YP   YP    YP    Y88888P 
*/

// Preload current values first
// TODO: Adjust Timezone Settings here, otherwise hour is NOT correct !!
void nav_CL (void) { 
  tmp_date_local.hour = date.hour;
  tmp_date_local.min = date.min;
  tmp_date_local.day = date.day;
  tmp_date_local.month = date.month;
  tmp_date_local.year = date.year;
  lcdm_navigate(MENU_CHILD);
}

/* Clock hour */
void al_menu_change_LT_hour(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (tmp_date_local.hour > 23) tmp_date_local.hour = 0;
                  //12345678901234567890
                  //Set hour=[00]:00h   
  sprintf (tmp_buf,"Set hour=[%02u]:%02uh   ", tmp_date_local.hour, tmp_date_local.min);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_LThh (void) { tmp_date_local.hour++; }

/* Clock minutes */
void al_menu_change_LT_min(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (tmp_date_local.min > 59) tmp_date_local.min = 0;
                  //12345678901234567890
  sprintf (tmp_buf,"Set min=%02u:[%02u]h    ", tmp_date_local.hour, tmp_date_local.min);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_LTmm (void) { tmp_date_local.min++; }

/* Clock day */
void al_menu_change_LT_day(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (tmp_date_local.day > 31) tmp_date_local.day = 1;
                  //12345678901234567890
                  //Set day=00.00.[1234]
  sprintf (tmp_buf,"Set day=[%02u].%02u.%04u", tmp_date_local.day, tmp_date_local.month, 1900+tmp_date_local.year);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_LTdy (void) { tmp_date_local.day++; }

/* Clock month */
void al_menu_change_LT_month(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (tmp_date_local.month > 12) tmp_date_local.month = 1;
                  //12345678901234567890
  sprintf (tmp_buf,"Set mon=%02u.[%02u].%04u", tmp_date_local.day, tmp_date_local.month, 1900+tmp_date_local.year);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_LTmo (void) { tmp_date_local.month++; }

/* Clock year */
void al_menu_change_LT_year(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (tmp_date_local.year > 130) tmp_date_local.year = 110;
                  //12345678901234567890
  sprintf (tmp_buf,"Set yr=%02u.%02u.[%04u] ", tmp_date_local.day, tmp_date_local.month, 1900+tmp_date_local.year);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_LTyr (void) { tmp_date_local.year++; }


/*
d8b   db d88888b d888888b        d8888b.  .d8b.  d8888b.  .d8b.  .88b  d88. .d8888. 
888o  88 88'     `~~88~~'        88  `8D d8' `8b 88  `8D d8' `8b 88'YbdP`88 88'  YP 
88V8o 88 88ooooo    88           88oodD' 88ooo88 88oobY' 88ooo88 88  88  88 `8bo.   
88 V8o88 88~~~~~    88    C8888D 88~~~   88~~~88 88`8b   88~~~88 88  88  88   `Y8b. 
88  V888 88.        88           88      88   88 88 `88. 88   88 88  88  88 db   8D 
VP   V8P Y88888P    YP           88      YP   YP 88   YD YP   YP YP  YP  YP `8888Y' 
*/


/*** OWN IP ADDRESS ***/

// Preload current values first
void nav_IPA (void) { 
  uip_gethostaddr(&hostaddr);
  addr_blocks[0] = uip_ipaddr1(&hostaddr);
  addr_blocks[1] = uip_ipaddr2(&hostaddr);
  addr_blocks[2] = uip_ipaddr3(&hostaddr);
  addr_blocks[3] = uip_ipaddr4(&hostaddr);
  lcdm_navigate(MENU_CHILD);
}

/* IP.A */
void al_menu_change_IPA_1(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[0] > 254) addr_blocks[0] = 1;
  sprintf (tmp_buf,"IP=[%03u].%03u.%03u.%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_IPA1 (void) { addr_blocks[0]++; }

/* IP.A */
void al_menu_change_IPA_2(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[1] > 254) addr_blocks[1] = 1;
  sprintf (tmp_buf,"IP=%03u.[%03u].%03u.%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_IPA2 (void) { addr_blocks[1]++; }

/* IP.A */
void al_menu_change_IPA_3(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[2] > 254) addr_blocks[2] = 1;
  sprintf (tmp_buf,"IP=%03u.%03u.[%03u].%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_IPA3 (void) { addr_blocks[2]++; }

/* IP.A */
void al_menu_change_IPA_4(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[3] > 254) addr_blocks[3] = 1;
                  //12345678901234567890
                  //IP=[192].168.100.100
  sprintf (tmp_buf,"IP=%03u.%03u.%03u.[%03u]", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_IPA4 (void) { addr_blocks[3]++; }

void set_IPA (void) {
  uip_ipaddr(&temp_ip_addr,  addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  uip_sethostaddr(&temp_ip_addr);
  eeprom_save(ip, &temp_ip_addr, IPADDR_LEN);
  eeprom_update_chksum();
  lcdm_navigate(MENU_CHILD);
}


/*** GATEWAY ***/

// Preload current values first
void nav_GWY (void) { 
  uip_getdraddr(&hostaddr);
  addr_blocks[0] = uip_ipaddr1(&hostaddr);
  addr_blocks[1] = uip_ipaddr2(&hostaddr);
  addr_blocks[2] = uip_ipaddr3(&hostaddr);
  addr_blocks[3] = uip_ipaddr4(&hostaddr);
  lcdm_navigate(MENU_CHILD);
}

/* IP.A */
void al_menu_change_GWY_1(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[0] > 254) addr_blocks[0] = 1;
  sprintf (tmp_buf,"GW=[%03u].%03u.%03u.%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_GWY1 (void) { addr_blocks[0]++; }

/* IP.A */
void al_menu_change_GWY_2(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[1] > 254) addr_blocks[1] = 1;
  sprintf (tmp_buf,"GW=%03u.[%03u].%03u.%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_GWY2 (void) { addr_blocks[1]++; }

/* IP.A */
void al_menu_change_GWY_3(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[2] > 254) addr_blocks[2] = 1;
  sprintf (tmp_buf,"GW=%03u.%03u.[%03u].%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_GWY3 (void) { addr_blocks[2]++; }

/* IP.A */
void al_menu_change_GWY_4(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[3] > 254) addr_blocks[3] = 1;
                  //12345678901234567890
                  //IP=[192].168.100.100
  sprintf (tmp_buf,"GW=%03u.%03u.%03u.[%03u]", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_GWY4 (void) { addr_blocks[3]++; }

void set_GWY (void) {
  uip_ipaddr(&temp_ip_addr,  addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  uip_setdraddr(&temp_ip_addr);
  eeprom_save(gateway, &temp_ip_addr, IPADDR_LEN);
  eeprom_update_chksum();
  lcdm_navigate(MENU_CHILD);
}


/*** DOMAIN NAME SERVER ***/

// Preload current values first
void nav_DNS (void) { 
  uip_getdraddr(&hostaddr);     // Take the Gateway as Nameserver !!!
  addr_blocks[0] = uip_ipaddr1(&hostaddr);
  addr_blocks[1] = uip_ipaddr2(&hostaddr);
  addr_blocks[2] = uip_ipaddr3(&hostaddr);
  addr_blocks[3] = uip_ipaddr4(&hostaddr);
  lcdm_navigate(MENU_CHILD);
}

/* IP.A */
void al_menu_change_DNS_1(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[0] > 254) addr_blocks[0] = 1;
  sprintf (tmp_buf,"NS=[%03u].%03u.%03u.%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_DNS1 (void) { addr_blocks[0]++; }

/* IP.A */
void al_menu_change_DNS_2(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[1] > 254) addr_blocks[1] = 1;
  sprintf (tmp_buf,"NS=%03u.[%03u].%03u.%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_DNS2 (void) { addr_blocks[1]++; }

/* IP.A */
void al_menu_change_DNS_3(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[2] > 254) addr_blocks[2] = 1;
  sprintf (tmp_buf,"NS=%03u.%03u.[%03u].%03u", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_DNS3 (void) { addr_blocks[2]++; }

/* IP.A */
void al_menu_change_DNS_4(void) {

  al_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  if (addr_blocks[3] > 254) addr_blocks[3] = 1;
                  //12345678901234567890
                  //IP=[192].168.100.100
  sprintf (tmp_buf,"NS=%03u.%03u.%03u.[%03u]", addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  fputs (tmp_buf, &lcd);

  al_draw_submenu_save();
}
void inc_DNS4 (void) { addr_blocks[3]++; }

void set_DNS (void) {
  uip_ipaddr(&temp_ip_addr,  addr_blocks[0], addr_blocks[1], addr_blocks[2], addr_blocks[3]);
  resolv_conf(&temp_ip_addr);
  eeprom_save(dns_server, &temp_ip_addr, IPADDR_LEN);
  eeprom_update_chksum();
  lcdm_navigate(MENU_CHILD);
}
























/*

.##.....##.########.##....##.##.....##.....######...#######..##....##.########.####..######..
.###...###.##.......###...##.##.....##....##....##.##.....##.###...##.##........##..##....##.
.####.####.##.......####..##.##.....##....##.......##.....##.####..##.##........##..##.......
.##.###.##.######...##.##.##.##.....##....##.......##.....##.##.##.##.######....##..##...####
.##.....##.##.......##..####.##.....##....##.......##.....##.##..####.##........##..##....##.
.##.....##.##.......##...###.##.....##....##....##.##.....##.##...###.##........##..##....##.
.##.....##.########.##....##..#######......######...#######..##....##.##.......####..######..

#############################################################################################


 ## INFO-TOP-LEVEL ##
  
          NAME              NEXT                PARENT      CHILD                    NEXT       ENTER      DISPLAY                    TEXT
*/                                                                                                                                   //12345678901234567890
MENU_ITEM(mi_system_info,   mi_timers_info,     NULL_MENU,  mi_config_mem_set_menu,  nav_next,  nav_down,  al_basis_info_display,     "System Info");
MENU_ITEM(mi_timers_info,   mi_acycle_info,     NULL_MENU,  mi_timers_set_menu,      nav_next,  nav_down,  al_menu_timers_display,    "=[ TIMERS T1/T1 ]===");
MENU_ITEM(mi_acycle_info,   mi_monitor_info,    NULL_MENU,  mi_acycle_set_menu,      nav_next,  nav_down,  al_menu_cycles_display,    "=[ CYCLES C1/C2 ]===");

MENU_ITEM(mi_monitor_info,   mi_clock_info,     NULL_MENU,  mi_acycle_set_menu,      nav_next,  nav_down,  al_menu_monitor_display,    "=[ MONITORING ]=====");

MENU_ITEM(mi_clock_info,    mi_network1_info,   NULL_MENU,  mi_clock_cl_set_menu,    nav_next,  nav_down,  al_menu_time_display,      "=[ Info CLOCK ]=====");
MENU_ITEM(mi_network1_info, mi_network2_info,   NULL_MENU,  mi_network_ip_set_menu,  nav_next,  nav_down,  al_menu_network_1_display, "=[ Info NETWORK 1 ]=");
MENU_ITEM(mi_network2_info, mi_config_info,     NULL_MENU,  mi_network_dns_set_menu, nav_next,  nav_down,  al_menu_network_2_display, "=[ Info NETWORK 2 ]=");

MENU_ITEM(mi_test_info,     mi_clock_info,      NULL_MENU,  mi_acycle_set_menu,      nav_next,  nav_down,  al_menu_cycles_display,    "=[ TEST MODE ]======");



MENU_ITEM(mi_config_info,   mi_test_info,       NULL_MENU,  mi_config_mem_set_menu,  nav_next,  nav_down,  al_menu_akku_display,      "Configuration");
//MENU_ITEM(mi_test_info,     mi_system_info,     NULL_MENU,  mi_test_set_menu,        nav_next,  nav_down,  al_menu_sensors_display,   "Test Mode");
MENU_ITEM(mi_current_info,  mi_system_info,     NULL_MENU,  mi_test_set_menu,        nav_next,  nav_down,  al_menu_sensors_current_display,   "Current Sensors");









/*
 ## Settings MENU-LEVEL ##
         NAME                       NEXT                       PARENT               CHILD/GROUP            NEXT        ENTER     DISPLAY                     TEXT
*/                                                                                                                                                         //12345678901234567890
MENU_ITEM(mi_config_set_menu,       mi_config_mem_set_menu,    mi_config_set_menu,  mi_config_set_menu,    nav_next,   nav_up,   al_list_submenus_display,  "=[ Menu AQUALOOP ]==");
MENU_ITEM(mi_config_mem_set_menu,   mi_config_sof_set_menu,    mi_config_set_menu,  mi_config_set_mem,     nav_next,   nav_down, al_list_submenus_display,  "1: Membranes config ");
MENU_ITEM(mi_config_sof_set_menu,   mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_set_sof,     nav_next,   nav_down, al_list_submenus_display,  "2: Suction overflow ");
MENU_ITEM(mi_config_slp_set_menu,   mi_config_c12_set_menu,    mi_config_set_menu,  mi_config_set_slpd,    nav_next,   nav_down, al_list_submenus_display,  "3: Sludge pump      ");
MENU_ITEM(mi_config_c12_set_menu,   mi_config_air_set_menu,    mi_config_set_menu,  mi_config_set_c1 ,     nav_next,   nav_down, al_list_submenus_display,  "4: C1/C2 suction    ");
MENU_ITEM(mi_config_air_set_menu,   mi_config_t1_set_menu,     mi_config_set_menu,  mi_config_set_airon,   nav_next,   nav_down, al_list_submenus_display,  "5: AIR on/off timer ");
MENU_ITEM(mi_config_t1_set_menu,    mi_config_t2_set_menu,     mi_config_set_menu,  mi_config_set_t1h,     nav_next,   nav_down, al_list_submenus_display,  "6: T1 starting time ");
MENU_ITEM(mi_config_t2_set_menu,    mi_config_back_set_menu,   mi_config_set_menu,  mi_config_set_t2h,     nav_next,   nav_down, al_list_submenus_display,  "7: T2 starting time ");
MENU_ITEM(mi_config_back_set_menu,  mi_config_mem_set_menu,    mi_config_set_menu,  mi_system_info,        nav_next,   nav_down, al_list_submenus_display,  "Return              ");

                                                                                                                                                            //12345678901234567890
MENU_ITEM(mi_config_set_mem,        mi_config_mem_set_menu,    mi_config_set_menu,  mi_config_mem_set_menu, inc_MEM,    set_para, al_menu_change_MEM_number, "Change MEM (number) ");
MENU_ITEM(mi_config_set_sof,        mi_config_sof_set_menu,    mi_config_set_menu,  mi_config_sof_set_menu, inc_SO,     set_para, al_menu_change_SO_number,  "Change SO (weeks)   ");
MENU_ITEM(mi_config_set_slpw,       mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_set_slpm,     inc_SLPw,   set_para, al_menu_change_SLP_week,   "Change SLP (weeks)  ");
MENU_ITEM(mi_config_set_slpd,       mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_set_slpm,     inc_SLPd,   set_para, al_menu_change_SLP_day,    "Change SLP (days)   ");
MENU_ITEM(mi_config_set_slpm,       mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_slp_set_menu, inc_SLPm,   set_para, al_menu_change_SLP_min,    "Change SLP (minutes)");
MENU_ITEM(mi_config_set_c1,         mi_config_c12_set_menu,    mi_config_set_menu,  mi_config_set_c2,       inc_C1,     set_para, al_menu_change_C1_number,  "Change C1 (cycles)  ");
MENU_ITEM(mi_config_set_c2,         mi_config_c12_set_menu,    mi_config_set_menu,  mi_config_c12_set_menu, inc_C2,     set_para, al_menu_change_C2_number,  "Change C2 (cycles)  ");
MENU_ITEM(mi_config_set_airon,      mi_config_air_set_menu,    mi_config_set_menu,  mi_config_set_airoff,   inc_AIRon,  set_para, al_menu_change_AIR_on,     "Change AIR ON (min) ");
MENU_ITEM(mi_config_set_airoff,     mi_config_air_set_menu,    mi_config_set_menu,  mi_config_air_set_menu, inc_AIRoff, set_para, al_menu_change_AIR_off,    "Change AIR OFF (min)");
MENU_ITEM(mi_config_set_t1h,        mi_config_t1_set_menu,     mi_config_set_menu,  mi_config_set_t1m,      inc_T1h,    set_para, al_menu_change_T1_hour,    "Change T1 (hour)    ");
MENU_ITEM(mi_config_set_t1m,        mi_config_t1_set_menu,     mi_config_set_menu,  mi_config_t1_set_menu,  inc_T1m,    set_para, al_menu_change_T1_min,     "Change T1 (minutes) ");
MENU_ITEM(mi_config_set_t2h,        mi_config_t2_set_menu,     mi_config_set_menu,  mi_config_set_t2m,      inc_T2h,    set_para, al_menu_change_T2_hour,    "Change T2 (hour)    ");
MENU_ITEM(mi_config_set_t2m,        mi_config_t2_set_menu,     mi_config_set_menu,  mi_config_t2_set_menu,  inc_T2m,    set_para, al_menu_change_T2_min,     "Change T2 (minutes) ");

/*
 ## CLOCK MENU-LEVEL ##
         NAME                       NEXT                    PARENT                CHILD/GROUP         NEXT         ENTER     DISPLAY                       TEXT
*/                                                                                                                                                       //12345678901234567890
MENU_ITEM(mi_clock_set_menu,        mi_clock_cl_set_menu,   mi_clock_set_menu,    mi_clock_set_menu,  nav_next,    nav_up,   al_list_submenus_display,    "=[ Menu CLOCK ]=====");
MENU_ITEM(mi_clock_cl_set_menu,     mi_clock_da_set_menu,   mi_clock_set_menu,    mi_clock_set_thh,   nav_next,    nav_CL,   al_list_submenus_display,    "1: Set Time         ");
MENU_ITEM(mi_clock_da_set_menu,     mi_clock_back_set_menu, mi_clock_set_menu,    mi_clock_set_tdd,   nav_next,    nav_CL,   al_list_submenus_display,    "2: Set Date         ");
//MENU_ITEM(mi_clock_tz_set_menu,     mi_clock_back_set_menu, mi_clock_set_menu,    mi_clock_set_tzh,   nav_next,    nav_down, al_list_submenus_display,    "3: Set Timezone     ");
MENU_ITEM(mi_clock_back_set_menu,   mi_clock_cl_set_menu,   mi_clock_set_menu,    mi_clock_info,      nav_next,    nav_down, al_list_submenus_display,    "Return              ");

MENU_ITEM(mi_clock_set_thh,         mi_clock_cl_set_menu,   mi_clock_set_menu,    mi_clock_set_tmm,     inc_LThh,  set_clock,   al_menu_change_LT_hour,   "Set Time (hour)     ");
MENU_ITEM(mi_clock_set_tmm,         mi_clock_cl_set_menu,   mi_clock_set_menu,    mi_clock_cl_set_menu, inc_LTmm,  set_clock,   al_menu_change_LT_min,    "Set Time (minutes)  ");
MENU_ITEM(mi_clock_set_tdd,         mi_clock_da_set_menu,   mi_clock_set_menu,    mi_clock_set_tmo,     inc_LTdy,  set_clock,   al_menu_change_LT_day,    "Set Date (day)      ");
MENU_ITEM(mi_clock_set_tmo,         mi_clock_da_set_menu,   mi_clock_set_menu,    mi_clock_set_tyr,     inc_LTmo,  set_clock,   al_menu_change_LT_month,  "Set Date (month)    ");
MENU_ITEM(mi_clock_set_tyr,         mi_clock_da_set_menu,   mi_clock_set_menu,    mi_clock_da_set_menu, inc_LTyr,  set_clock,   al_menu_change_LT_year,   "Set Date (year)     ");
//MENU_ITEM(mi_clock_set_tzh,         mi_clock_tz_set_menu,   mi_clock_set_menu,    mi_clock_set_tzh,     inc_LTyr,  set_clock,   al_menu_change_LT_year,   "Set Timezone (hour) ");
//MENU_ITEM(mi_clock_set_tzm,         mi_clock_tz_set_menu,   mi_clock_set_menu,    mi_clock_tz_set_menu, inc_LTyr,  set_clock,   al_menu_change_LT_year,   "Set Timezone (min)  ");

/*
 ## NETWORK MENU-LEVEL ##
         NAME                        NEXT                       PARENT                CHILD/GROUP            NEXT        ENTER     DISPLAY                    TEXT
*/                                                                                                                                                           //12345678901234567890
MENU_ITEM(mi_network1_set_menu,      mi_network_ip_set_menu,    mi_network1_set_menu, mi_network1_set_menu,  nav_next,   nav_up,   al_list_submenus_display,  "=[ Menu NETWORK ]===");
MENU_ITEM(mi_network_ip_set_menu,    mi_network_gwy_set_menu,   mi_network1_set_menu, mi_network1_set_ip1,   nav_next,   nav_IPA,  al_list_submenus_display,  "1: IP Address       ");
MENU_ITEM(mi_network_gwy_set_menu,   mi_network_dns_set_menu,   mi_network1_set_menu, mi_network1_set_gw1,   nav_next,   nav_GWY,  al_list_submenus_display,  "2: GATEWAY Address  ");
MENU_ITEM(mi_network_dns_set_menu,   mi_network_ntp_set_menu,   mi_network1_set_menu, mi_network2_set_ns1,   nav_next,   nav_DNS,  al_list_submenus_display,  "3: DNS Address      ");
MENU_ITEM(mi_network_ntp_set_menu,   mi_network1_back_set_menu, mi_network1_set_menu, mi_network1_set_ntp,   nav_next,   nav_down, al_list_submenus_display,  "4: NTP Address      ");
MENU_ITEM(mi_network1_back_set_menu, mi_network_ip_set_menu,    mi_network1_set_menu, mi_network1_info,      nav_next,   nav_down, al_list_submenus_display,  "Return              ");

MENU_ITEM(mi_network1_set_ip1,       mi_network_ip_set_menu,    mi_network1_set_menu, mi_network1_set_ip2,    inc_IPA1,  set_IPA,  al_menu_change_IPA_1,      "Set IP Address (A)  ");
MENU_ITEM(mi_network1_set_ip2,       mi_network_ip_set_menu,    mi_network1_set_menu, mi_network1_set_ip3,    inc_IPA2,  set_IPA,  al_menu_change_IPA_2,      "Set IP Address (B)  ");
MENU_ITEM(mi_network1_set_ip3,       mi_network_ip_set_menu,    mi_network1_set_menu, mi_network1_set_ip4,    inc_IPA3,  set_IPA,  al_menu_change_IPA_3,      "Set IP Address (C)  ");
MENU_ITEM(mi_network1_set_ip4,       mi_network_ip_set_menu,    mi_network1_set_menu, mi_network_ip_set_menu, inc_IPA4,  set_IPA,  al_menu_change_IPA_4,      "Set IP Address (D)  ");

MENU_ITEM(mi_network1_set_gw1,       mi_network_gwy_set_menu,    mi_network1_set_menu, mi_network1_set_gw2,     inc_GWY1,  set_GWY,  al_menu_change_GWY_1,    "Set GWY Address (A)  ");
MENU_ITEM(mi_network1_set_gw2,       mi_network_gwy_set_menu,    mi_network1_set_menu, mi_network1_set_gw3,     inc_GWY2,  set_GWY,  al_menu_change_GWY_2,    "Set GWY Address (B)  ");
MENU_ITEM(mi_network1_set_gw3,       mi_network_gwy_set_menu,    mi_network1_set_menu, mi_network1_set_gw4,     inc_GWY3,  set_GWY,  al_menu_change_GWY_3,    "Set GWY Address (C)  ");
MENU_ITEM(mi_network1_set_gw4,       mi_network_gwy_set_menu,    mi_network1_set_menu, mi_network_gwy_set_menu, inc_GWY4,  set_GWY,  al_menu_change_GWY_4,    "Set GWY Address (D)  ");

MENU_ITEM(mi_network2_set_ns1,       mi_network_dns_set_menu,    mi_network1_set_menu, mi_network2_set_ns2,     inc_DNS1,  set_DNS,  al_menu_change_DNS_1,    "Set DNS Address (A)  ");
MENU_ITEM(mi_network2_set_ns2,       mi_network_dns_set_menu,    mi_network1_set_menu, mi_network2_set_ns3,     inc_DNS2,  set_DNS,  al_menu_change_DNS_2,    "Set DNS Address (B)  ");
MENU_ITEM(mi_network2_set_ns3,       mi_network_dns_set_menu,    mi_network1_set_menu, mi_network2_set_ns4,     inc_DNS3,  set_DNS,  al_menu_change_DNS_3,    "Set DNS Address (C)  ");
MENU_ITEM(mi_network2_set_ns4,       mi_network_dns_set_menu,    mi_network1_set_menu, mi_network_dns_set_menu, inc_DNS4,  set_DNS,  al_menu_change_DNS_4,    "Set DNS Address (D)  ");


MENU_ITEM(mi_network1_set_ntp,      mi_network_ntp_set_menu,   mi_network1_set_menu, mi_network1_set_menu,  nav_next,   nav_up,   al_draw_submenu_title,      "Set NTP Address     ");



/*
 ## TIMERS MENU-LEVEL ##
         NAME                     NEXT            PARENT            CHILD/GROUP         NEXT         ENTER        DISPLAY                    TEXT
*/ 
MENU_ITEM(mi_timers_set_menu,     NULL_MENU,      mi_timers_info,   NULL_MENU,          NULL_FUNC,   NULL_FUNC,   NULL_FUNC,                 "Timer Settings");

/*
 ## CYCLES MENU-LEVEL ##
         NAME                     NEXT            PARENT            CHILD/GROUP         NEXT        ENTER         DISPLAY                    TEXT
*/ 
MENU_ITEM(mi_acycle_set_menu,     NULL_MENU,      mi_acycle_info,   NULL_MENU,          NULL_FUNC,   NULL_FUNC,   NULL_FUNC,                 "Cycle Settings");



/*
 ## CONFIG MENU-LEVEL ##
         NAME                        NEXT                       PARENT            CHILD/GROUP              NEXT        ENTER     DISPLAY                    TEXT
*/ 
//MENU_ITEM(mi_config_set_menu,   NULL_MENU,      mi_config_info,   NULL_MENU,    NULL_FUNC,   NULL_FUNC,   NULL_FUNC,                    "Config Settings");
MENU_ITEM(mi_test_set_menu,     NULL_MENU,      mi_test_info,     NULL_MENU,      NULL_FUNC,   NULL_FUNC,   NULL_FUNC,                  "Test Modes");






void init_menu(void) {

  al_menu_init_variables();

  lcdm_set_default_write_callback(generic_write);
  lcdm_navigate(&mi_system_info);

  //lcdm_navigate(&mi_current_info);
}




