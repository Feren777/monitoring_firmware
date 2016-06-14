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


static void mondns_query_cb(char *name, uip_ipaddr_t * ipaddr);
static void mondns_net_main(void);
static uint8_t mondns_extract_ip(void);




void mondns_update(void) {

  uint8_t i;
  /* Request to close all other mondns connections */
  for (i = 0; i < UIP_CONNS; i++)
    if (uip_conns[i].callback == mondns_net_main) {
      uip_conns[i].appstate.mondns.state = MONDNS_CANCEL;
      AQUALOOPDEBUG("rm conn\n");
    }

  /* Resolve IP of our monitoring DYNDNS server, we will later use */
  uip_ipaddr_t *ipaddr;
  if (!(ipaddr = resolv_lookup("monitor.intewa.net"))) {
    resolv_query("monitor.intewa.net", mondns_query_cb);
  	AQUALOOPDEBUG("nslookup\n");
  }
  else {
  	uip_conn_t *conn = uip_connect(ipaddr, HTONS(8080), mondns_net_main);
	  if (conn) {
	  	AQUALOOPDEBUG("conn cb\n");
	  }
  }

}

static void mondns_query_cb(char *name, uip_ipaddr_t * ipaddr)
{
  uip_conn_t *conn = uip_connect(ipaddr, HTONS(8080), mondns_net_main);
  if (conn) {
    conn->appstate.mondns.state = MONDNS_HOSTNAME;
  	AQUALOOPDEBUG("conn cb\n");
  }
}


/* Copy IP from incoming <id ip="ip"> message to our STATE. */
static uint8_t mondns_extract_ip(void)
{
  // Pointer to result in received webpage where <id="ip">192.168.1.1</ip> is located
  char *ipptr = strstr_P(uip_appdata, PSTR("id=\"ip\">"));
  if (ipptr)
  {
  	// Skip to start of ip address
    ipptr += 8;
    //AQUALOOPDEBUG("ip=%i\n", ipptr);

    // Ending tag
    char *ipendptr = strchr(ipptr, '<');
    if (ipendptr) {
      iplength = ipendptr - ipptr;

      if (iplength > 15)
        AQUALOOPDEBUG("ip too long: %i\n", iplength);

      else {
        //AQUALOOPDEBUG("endquote found %i\n", ipendptr);
        parse_ip(ipptr, &extaddr);
        return iplength;
      }
    }

    return 1;
  }

  return 0;                     /* Failed. */
}







static void mondns_net_main(void) {

  /* Close connection on ready an when cancel was requested */
  if (uip_conn->appstate.mondns.state >= MONDNS_READY) {
    uip_abort();
    AQUALOOPDEBUG("skip\n");
    return;
  }

  if (uip_acked()) {
    uip_conn->appstate.mondns.state++;
    if (uip_conn->appstate.mondns.state == MONDNS_READY)
      uip_close();

    AQUALOOPDEBUG("uip_acked: %d\n", uip_conn->appstate.mondns.state);
  }



  if (uip_newdata() && uip_len) {

  	if (uip_conn->appstate.mondns.state == MONDNS_WAIT) {

		iplength = mondns_extract_ip();

		if (iplength > 1) {
			uip_conn->appstate.mondns.state = MONDNS_UPDATE;

			// Print debug info about our IP...
			char result[16];
			print_ipaddr(&extaddr, result, iplength);
			
	        char debug_buf[20];
	      	hd44780_goto(0, 0);
	  		snprintf (debug_buf, 20, "%s", result );
	  		fputs (debug_buf, &lcd);

	        AQUALOOPDEBUG("s:IP:%s\n", result);

		} else
			uip_conn->appstate.mondns.state = MONDNS_READY;
  	}

  }

  if (uip_rexmit() || uip_connected() || uip_acked() || uip_newdata() ) {
    uint8_t len;

#ifdef IPV6_SUPPORT
    uint16_t *ip6;
#else
    uint8_t *ip;
#endif


    switch (uip_conn->appstate.mondns.state) {

      case MONDNS_HOSTNAME:
        len = sprintf_P(uip_sappdata, PSTR("GET /myip.php HTTP/1.1\r\nHost: monitor.intewa.net\r\n\r\n"));

    	uip_send(uip_sappdata, len);
    	AQUALOOPDEBUG("s:GET\n");
        break;

      case MONDNS_WAIT:
      	AQUALOOPDEBUG("s:wait\n");
      	break;

      case MONDNS_UPDATE:
        mondns_update();
      	// http://monitor.intewa.net:8080/cgi-bin/update?ip=123.123.123.123&user=haraldo&pass=iKEYPASS&host=office.monitor.intewa.net
      	len = sprintf_P(uip_sappdata, PSTR("GET /cgi-bin/setip?"));
      	uip_send(uip_sappdata, len);
      	AQUALOOPDEBUG("s:update\n");
      	break;

      case MONDNS_IP:

#ifdef IPV6_SUPPORT
        ip6 = (uint16_t *) & extaddr;
        len = sprintf_P(uip_sappdata,
                        PSTR("ip=%x%%3A%x%%3A%x%%3A%x%%3A%x%%3A%x%%3A%x%%3A%x&"), 
                        HTONS(ip6[0]), HTONS(ip6[1]), HTONS(ip6[2]), HTONS(ip6[3]), 
                        HTONS(ip6[4]), HTONS(ip6[5]), HTONS(ip6[6]), HTONS(ip6[7]));
        uip_send(uip_sappdata, len);
#else
        ip = (uint8_t *) & extaddr;
        len = sprintf_P(uip_sappdata,
                        PSTR("ip=%u.%u.%u.%u&"),
                        ip[0], ip[1], ip[2], ip[3]);
        uip_send(uip_sappdata, len);
#endif
      	AQUALOOPDEBUG("s:ip\n");
        break;

      case MONDNS_USERNAME:
        len = sprintf_P(uip_sappdata,
                        PSTR("id=1 HTTP/1.1\r\nHost: monitor.intewa.net\r\n\r\n"));

        uip_send(uip_sappdata, len);
      	AQUALOOPDEBUG("s:usern\n");
      	break;

  	  case MONDNS_READY:
  		AQUALOOPDEBUG("s:ready\n");
  		break;

      case MONDNS_CANCEL:
      	AQUALOOPDEBUG("s:cancel\n");
      	break;

    }
   

  }





}













void init_networking(void) {

	//mondns_update();

}











  //timer(500, mondns_update())

/*
  -- Ethersex META --
  header(services/intewa/aqualoop/aqualoop_net.h)

  state_header(services/intewa/aqualoop/aqualoop_net_state.h)
  state_tcp(`
#   if defined(TCP_SUPPORT) && !defined(TEENSY_SUPPORT)
       struct mondns_connection_state_t mondns;
#   endif
  ')
*/


