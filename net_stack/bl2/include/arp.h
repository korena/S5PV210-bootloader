/*
 *	Copied from Linux Monitor (LiMon) - Networking.
 *
 *	Copyright 1994 - 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000 Roland Borde
 *	Copyright 2000 Paolo Scaffardi
 *	Copyright 2000-2002 Wolfgang Denk, wd@denx.de
 *	SPDX-License-Identifier:	GPL-2.0
 */

#ifndef __ARP_H__
#define __ARP_H__

#include "net.h"


extern const uint8_t net_bcast_ethaddr[6];
 
#define PROT_ARP	0x0806		/* IP ARP protocol		*/
#define PROT_RARP	0x8035		/* IP ARP protocol		*/

extern struct in_addr net_arp_wait_packet_ip;
/* MAC address of waiting packet's destination */
extern unsigned char *arp_wait_packet_ethaddr;
extern uint32_t arp_wait_tx_packet_size;
extern uint32_t arp_wait_timer_start;
extern uint32_t arp_wait_try;
extern unsigned char *net_tx_packet;
void arp_init(void);
void arp_request(void);
void arp_raw_request(struct in_addr source_ip, const unsigned char *targetEther,
      	struct in_addr target_ip);
uint32_t arp_timeout_check(void);
void arp_receive(struct ethernet_hdr *et, struct ip_udp_hdr *ip, int len);
void arp_handler(unsigned char* packet,unsigned int dport,struct in_addr sip,unsigned int sport,unsigned int len);

/*
 *EXTERNS
 * */

static inline void net_write_ip(void *to, struct in_addr ip);

#endif /* __ARP_H__ */
