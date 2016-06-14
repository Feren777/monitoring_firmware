/*                     -*- mode: C; c-file-style: "stroustrup"; -*-
 *
 * Copyright (c) 2007 by Stefan Siegl <stesie@brokenpipe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or
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

#include "udpintewa_net.h"
#include "protocols/uip/uip.h"
#include "protocols/uip/uip_router.h"
#include "services/intewa/aqualoop/aqualoop.h"
#include "core/portio/portio.h"
#include "config.h"


#define BUF ((struct uip_udpip_hdr *) (uip_appdata - UIP_IPUDPH_LEN))

struct udpintewa_packet
{
	uint8_t target; 		// 255=get data
	//uip_ipaddr_t address;	// Device address
};

void
udpintewa_net_init (void)
{
    uip_ipaddr_t ip;
    uip_ipaddr_copy (&ip, all_ones_addr);

    uip_udp_conn_t *udp_echo_conn = uip_udp_new (&ip, 0, udpintewa_net_main);

    if (!udp_echo_conn) 
	return; /* dammit. */

    uip_udp_bind (udp_echo_conn, HTONS (UDP_INTEWAMONITOR_PORT));
}


void
udpintewa_net_main(void)
{
    if (!uip_newdata ())
	return;

/*
	AQUALOOPDEBUG ("udp hello\n");

    uip_slen = 0;
    uint16_t len = uip_len;
    uint8_t buffer[uip_len];
    memcpy(buffer, uip_appdata, uip_len);

    struct udpintewa_packet* packet = (struct udpintewa_packet*)buffer;

    uint8_t* answer = uip_appdata;

    while (len>=sizeof(struct udpintewa_packet)) {

		answer[0] = 'i';
		answer[1] = 'n';
		answer[2] = 't';
		answer[3] = 'e';
		answer[4] = 'w';
		answer[5] = 'a';


     	packet++;
    	len-=sizeof(struct udpintewa_packet);
     }

    if (uip_slen == 0) return;
	// Sent data out
*/
    AQUALOOPDEBUG ("udp echo\n");

    uip_udp_conn_t echo_conn;
    uip_ipaddr_copy(echo_conn.ripaddr, BUF->srcipaddr);
    echo_conn.rport = BUF->srcport;
    echo_conn.lport = HTONS(UDP_INTEWAMONITOR_PORT);

    uip_udp_conn = &echo_conn;

    uip_slen = uip_len;
    uip_process(UIP_UDP_SEND_CONN); 
    router_output();

    uip_slen = 0;

}

/*
  -- Ethersex META --
  header(protocols/udpintewa/udpintewa_net.h)
  net_init(udpintewa_net_init)
*/
