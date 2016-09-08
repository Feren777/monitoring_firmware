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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include <stdbool.h>

#include "config.h"
#include "core/bit-macros.h"
#include "core/debug.h"
#include "core/eeprom.h"

#include "rainmaster.h"
//#include "aqualoop_core.h"
//#include "aqualoop_sensors.h"
//#include "aqualoop_gui.h"
#include "rainmaster_menu.h"
#include "services/lcd-menu/lcd_menu.h"
#include "core/util/fixedpoint.h"

#include "hardware/lcd/hd44780.h"
#include "hardware/adc/adc.h"
#include "services/clock/clock.h"
#include "services/clock/clock_ecmd_date.h"

#include "protocols/uip/uip.h"
#include "protocols/uip/parse.h"
#include "protocols/dns/resolv.h"
#include "services/ntp/ntp.h"

#include "hardware/rfid/rfid.h"
#include "hardware/rfid/em4095.h"

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

uint8_t rm_char_arrup[8] = {
	0b11111,
	0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

uint8_t rm_char_arrdown[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100,
	0b11111
};

uint8_t rm_char_triangelright[8] = {
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
timestamp_t 	 tmp_ts_local;         // stores converted manual time to unix timestamp

/* Initialize the variables at menu startup */
void rm_menu_init_variables(void) {

  // Start input of date at 01.01.2010
  tmp_date_local.day = 1;
  tmp_date_local.month = 1;
  tmp_date_local.year = 110;
  
  // Start with 00:00 hour
  tmp_date_local.hour = 0;
  tmp_date_local.min = 0;
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
void set_para (void) { 
  
  RAINMASTERDEBUG("%d\n", rainmaster_params_ram.levl_tank_max);

  eeprom_save(rainmaster_params, &rainmaster_params_ram, sizeof(rainmaster_params_t));
  eeprom_update_chksum(); /*hd44780_clear();*/ lcdm_navigate(MENU_CHILD);

  RAINMASTERDEBUG("%d\n", rainmaster_get_levelsensor());
}

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
void rm_draw_menu_blankline(uint8_t line) {

  hd44780_goto(line, 0);
              //12345678901234567890
  hd44780_puts("                    ");  
}

/* Display the SAVE option at bottom */
void rm_draw_submenu_save(void) {

  hd44780_goto(3, 0);
  //            12345678901234567890
  hd44780_puts("NEXT:Edit ENTER:Save");

}

/* Puts the name of the submenu in the header (first row) and 
 * the currently selected menu-item in the second row.
 * Static navigation hints are displayed at the bottom.
 */
void rm_list_submenus_display(void) {

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

    rm_draw_menu_blankline(2);

    hd44780_goto(3, 0);
    //            12345678901234567890
    hd44780_puts("NEXT:Next  ENTER:Set");

  }
 
}

/* PARAMETER setting: Displays the title of current parameter
 * in the first row.
 */
void rm_draw_submenu_item(void) {
  hd44780_goto(0, 0);
  lcdm_write_function(lcdm_get_current_menu()->text);
  rm_draw_menu_blankline(1);
}

//UNUSED !!
void rm_draw_submenu_title(void) {

  if (lcdm_write_function) {
    hd44780_goto(0, 0);
    lcdm_write_function(lcdm_get_parent_menu()->text);
    hd44780_goto(1, 0);
    lcdm_write_function(lcdm_get_current_menu()->text);
  }

}






/*
d8888b.  .d8b.  .d8888. d888888b .d8888.   d888888b d8b   db d88888b  .d88b.  
88  `8D d8' `8b 88'  YP   `88'   88'  YP     `88'   888o  88 88'     .8P  Y8. 
88oooY' 88ooo88 `8bo.      88    `8bo.        88    88V8o 88 88ooo   88    88 
88~~~b. 88~~~88   `Y8b.    88      `Y8b.      88    88 V8o88 88~~~   88    88 
88   8D 88   88 db   8D   .88.   db   8D     .88.   88  V888 88      `8b  d8' 
Y8888P' YP   YP `8888Y' Y888888P `8888Y'   Y888888P VP   V8P YP       `Y88P'  
*/

void rm_basis_info_display(void) {
  hd44780_goto(0, 0);

  char net1_buf[20];
  memset(&net1_buf[0], ' ', sizeof(net1_buf));
  sprintf(net1_buf, " IP:                ");
  //uint8_t len1 = print_ipaddr(&hostaddr, net1_buf + 5, 16);
  uint8_t len1 = print_ipaddr(&uip_hostaddr, net1_buf + 5, 16);
  if (len1 < 15) net1_buf[len1 + 5] = ' ';
  fputs (net1_buf, &lcd);

  rm_draw_menu_blankline(1);
  rm_draw_menu_blankline(2);
  rm_draw_menu_blankline(3);


}




void rm_menu_sens_level_display (void) {
  hd44780_goto(0, 0);
  //lcdm_write_function(lcdm_get_current_menu()->text);

  //rm_draw_menu_blankline(1);

  //hd44780_goto(2, 0);
  char tank_buf [20];
                     //12345678901234567890
    sprintf (tank_buf,"Tanklevel: %3d.%01d%c   ", rainmaster_get_tanklevel() / 10 , rainmaster_get_tanklevel() % 10 , 0x25 );
  fputs (tank_buf, &lcd);

  //rm_draw_menu_blankline(3);

  rm_draw_menu_blankline(1);
  rm_draw_menu_blankline(2);
  rm_draw_menu_blankline(3);
}






/*
db      d88888b db    db d88888b db        .d8888. d88888b d888888b d888888b d888888b d8b   db  d888b  .d8888. 
88      88'     88    88 88'     88        88'  YP 88'     `~~88~~' `~~88~~'   `88'   888o  88 88' Y8b 88'  YP 
88      88ooooo Y8    8P 88ooooo 88        `8bo.   88ooooo    88       88       88    88V8o 88 88      `8bo.   
88      88~~~~~ `8b  d8' 88~~~~~ 88          `Y8b. 88~~~~~    88       88       88    88 V8o88 88  ooo   `Y8b. 
88booo. 88.      `8bd8'  88.     88booo.   db   8D 88.        88       88      .88.   88  V888 88. ~8~ db   8D 
Y88888P Y88888P    YP    Y88888P Y88888P   `8888Y' Y88888P    YP       YP    Y888888P VP   V8P  Y888P  `8888Y' 
*/

/* LEVEL min */
void dm_menu_change_level_min(void) {

  rm_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  //if (aqualoop_params_ram.t2_min > 59) aqualoop_params_ram.t2_min = 0;
                  //12345678901234567890
                  //Set LEVEL [MIN]     
  sprintf (tmp_buf,"Set LEVEL [MIN]:%04d", rainmaster_get_levelsensor() );
  fputs (tmp_buf, &lcd);

  rm_draw_submenu_save();
}
void set_LVLmin (void) { rainmaster_params_ram.levl_tank_min = rainmaster_get_levelsensor(); }

/* LEVEL max */
void dm_menu_change_level_max(void) {

  rm_draw_submenu_item();

  hd44780_goto(2, 0);
  char tmp_buf [20];
  //if (aqualoop_params_ram.t2_min > 59) aqualoop_params_ram.t2_min = 0;
                  //12345678901234567890
                  //Set LEVEL [MIN]
  sprintf (tmp_buf,"Set LEVEL [MAX]:%04d", rainmaster_get_levelsensor() );
  fputs (tmp_buf, &lcd);

  rm_draw_submenu_save();
}
void set_LVLmax (void) { rainmaster_params_ram.levl_tank_max = rainmaster_get_levelsensor(); }








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
  
          NAME              NEXT              PARENT      CHILD                   NEXT       ENTER      DISPLAY                    TEXT
*/                                                                                                                                   //12345678901234567890
MENU_ITEM(mi_system_info,   mi_level_info,    NULL_MENU,  NULL_MENU,              nav_next,  nav_down,  rm_basis_info_display,            "System Info");

MENU_ITEM(mi_level_info,    mi_system_info,   NULL_MENU,  mi_level_min_set_menu,  nav_next,  nav_down,  rm_menu_sens_level_display,    "=[ LEVEL SENSOR ]===");


/*
 ## Settings MENU-LEVEL ##
         NAME                       NEXT                       PARENT               CHILD/GROUP          NEXT        ENTER     DISPLAY                     TEXT
*/                                                                                                                                                        //12345678901234567890
MENU_ITEM(mi_level_set_menu,        mi_level_min_set_menu,     mi_level_set_menu,  mi_level_set_menu,    nav_next,   nav_up,    rm_list_submenus_display,  "=[ Menu LEVEL SENS ]");
MENU_ITEM(mi_level_min_set_menu,    mi_level_max_set_menu,     mi_level_set_menu,  mi_level_set_min,     nav_next,   nav_down,  rm_list_submenus_display,  "1: MINIMUM level    ");
MENU_ITEM(mi_level_max_set_menu,    mi_level_back_set_menu,    mi_level_set_menu,  mi_level_set_max,     nav_next,   nav_down,  rm_list_submenus_display,  "2: MAXIMUM level    ");
MENU_ITEM(mi_level_back_set_menu,   mi_level_min_set_menu,     mi_level_set_menu,  mi_level_info,        nav_next,   nav_down,  rm_list_submenus_display,  "Return              ");
                                                                                                                                                             //12345678901234567890
MENU_ITEM(mi_level_set_min,        mi_level_min_set_menu,    mi_level_set_menu,  mi_level_min_set_menu,  set_LVLmin,    set_para,   dm_menu_change_level_min, "Store MIN (level)   ");
MENU_ITEM(mi_level_set_max,        mi_level_max_set_menu,    mi_level_set_menu,  mi_level_max_set_menu,  set_LVLmax,    set_para,   dm_menu_change_level_max, "Store MAX (level)   ");



/*
.88b  d88. d88888b d8b   db db    db    .o88b.  .d88b.  d8b   db d888888b d8888b.  .d88b.  db      
88'YbdP`88 88'     888o  88 88    88   d8P  Y8 .8P  Y8. 888o  88 `~~88~~' 88  `8D .8P  Y8. 88      
88  88  88 88ooooo 88V8o 88 88    88   8P      88    88 88V8o 88    88    88oobY' 88    88 88      
88  88  88 88~~~~~ 88 V8o88 88    88   8b      88    88 88 V8o88    88    88`8b   88    88 88      
88  88  88 88.     88  V888 88b  d88   Y8b  d8 `8b  d8' 88  V888    88    88 `88. `8b  d8' 88booo. 
YP  YP  YP Y88888P VP   V8P ~Y8888P'    `Y88P'  `Y88P'  VP   V8P    YP    88   YD  `Y88P'  Y88888P 
*/

void init_menu(void) {

  rm_menu_init_variables();

  lcdm_set_default_write_callback(generic_write);
  lcdm_navigate(&mi_system_info);

  //al_statusbits.menu_halt = false; 
  //lcdm_navigate(&mi_current_info);
  //lcdm_navigate(&mi_rfid_info);
  //lcdm_navigate(&mi_DEBUG_info);
}


/*
!! SEHR SPARSAM HIER ANWENDEN !! - SONST CHAOS !!
*/

void rm_menu_control(void) {

/**
  // Falls im Zeiteinstellungs-Menu...
  if (  (lcdm_get_current_menu() == &mi_config_set_t1h) ||
        (lcdm_get_current_menu() == &mi_config_set_t1m) ||
        (lcdm_get_current_menu() == &mi_config_set_t2h) ||
        (lcdm_get_current_menu() == &mi_config_set_t2m) ) {
      
      al_statusbits.menu_halt = true;
      PIN_SET(STATUSLED_ERROR);    
  } else {
      al_statusbits.menu_halt = false;
      PIN_CLEAR(STATUSLED_ERROR); 
  }



  switch (rm_menuscreen) {


    default:
          break;
  }

  al_menuscreen = AL_MENUSCREEN_NULL;

**/

}
