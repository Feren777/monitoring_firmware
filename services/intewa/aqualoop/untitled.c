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
#include "aqualoop_net.h"

#include "aqualoop_menu.h"
#include "services/lcd-menu/lcd_menu.h"
#include "hardware/lcd/hd44780.h"

#include "protocols/uip/uip.h"
#include "protocols/uip/parse.h"
#include "protocols/dns/resolv.h"




enum MonDnsStates
{
  MONDNS_HOSTNAME,		// 0
  MONDNS_WAIT,			// 1
  MONDNS_UPDATE,		// 2
  MONDNS_IP,			// 3
  MONDNS_USERNAME,		// 4
  MONDNS_PASSWORD,		// 5
  MONDNS_READY,			// 6
  MONDNS_CANCEL,		// 7
};


uip_ipaddr_t extaddr;
uint8_t iplength = 0;



enum state { ST_NEW, ST_OPEN, ST_SHIFT, ST_END };

enum state current_state = ST_NEW;

while (current_state != ST_END)
{
    input = get_input();

    switch (current_state)
    {
        case ST_NEW:
        /* Do something with input and set current_state */
        break;

        case ST_OPEN:
        /* Do something different and set current_state */
        break;

        /* ... etc ... */
    }
}



