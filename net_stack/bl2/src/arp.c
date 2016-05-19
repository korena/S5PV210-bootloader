#include <stddef.h>
#include "arp.h"
#include "in.h"
#include "terminal.h"
#include "timer.h"
#include "io.h"

//#define ARP_DEBUG


#ifndef	CONFIG_ARP_TIMEOUT
/* Milliseconds before trying ARP again */
# define ARP_TIMEOUT		5000UL
#else
# define ARP_TIMEOUT		CONFIG_ARP_TIMEOUT
#endif


#ifndef	CONFIG_NET_RETRY_COUNT
# define ARP_TIMEOUT_COUNT	5	/* # of timeouts before giving up  */
#else
# define ARP_TIMEOUT_COUNT	CONFIG_NET_RETRY_COUNT
#endif

struct in_addr net_arp_wait_packet_ip;
static struct in_addr net_arp_wait_reply_ip;
/* MAC address of waiting packet's destination */
unsigned char	       *arp_wait_packet_ethaddr;
uint32_t	arp_wait_tx_packet_size;
unsigned long	arp_wait_timer_start;
uint32_t		arp_wait_try;

static unsigned char   *arp_tx_packet;	/* THE ARP transmit packet */
static unsigned char	arp_tx_packet_buf[PKTSIZE_ALIGN + PKTALIGN];

void arp_init(void){
	arp_wait_packet_ethaddr = NULL;	 
//	print_format("arp_wait_packet_ethaddr = null\n\r");
	net_arp_wait_packet_ip.s_addr = 0 ; 
//	print_format("net_arp_wait_packet_ip.s_addr = 0\n\r");
	net_arp_wait_reply_ip.s_addr = 0;
//	print_format("net_arp_wait_reply_ip.s_addr = 0\n\r");
	arp_wait_tx_packet_size = 0;
//	print_format("arp_wait_tx_packet_size = 0\n\r");
	arp_tx_packet = &arp_tx_packet_buf[0]+(PKTALIGN - 1);
//	print_format("arp_tx_packet = &arp_tx_packet_buf[0]+(PKTALIGN - 1)\n\r");
	arp_tx_packet -= (unsigned long) arp_tx_packet % PKTALIGN;
//	print_format("arp_tx_packet -= (unsigned long) arp_tx_packet % PKTALIGN\n\r");
}

void arp_request(void)
{
	if ((net_arp_wait_packet_ip.s_addr & net_netmask.s_addr) !=
			(net_ip.s_addr & net_netmask.s_addr)) {
		if (net_gateway.s_addr == 0) {
			print_format("## Warning: gatewayip needed but not set\n\r");
			net_arp_wait_reply_ip = net_arp_wait_packet_ip;
		} else {
			net_arp_wait_reply_ip = net_gateway;
		}
	} else {
		net_arp_wait_reply_ip = net_arp_wait_packet_ip;
	}

		arp_raw_request(net_ip, net_null_ethaddr, net_arp_wait_reply_ip);
}


void arp_raw_request(struct in_addr source_ip, const unsigned char *target_ethaddr,
		struct in_addr target_ip)
{
	unsigned char *pkt;
	struct arp_hdr *arp;
	int eth_hdr_size;

	print_format("ARP broadcast %d\n\r", arp_wait_try);

	pkt = arp_tx_packet;

	eth_hdr_size = net_set_ether(pkt, net_bcast_ethaddr, PROT_ARP);
	pkt += eth_hdr_size;

	arp = (struct arp_hdr *)pkt;

	arp->ar_hrd = htons(ARP_ETHER);
	arp->ar_pro = htons(PROT_IP);
	arp->ar_hln = ARP_HLEN;
	arp->ar_pln = ARP_PLEN;
	arp->ar_op = htons(ARPOP_REQUEST);

	ul_memcpy(&arp->ar_sha, net_ethaddr, ARP_HLEN);	/* source ET addr */
	net_write_ip(&arp->ar_spa, source_ip);		/* source IP addr */
	ul_memcpy(&arp->ar_tha, target_ethaddr, ARP_HLEN);	/* target ET addr */
	net_write_ip(&arp->ar_tpa, target_ip);		/* target IP addr */

	net_send_packet(arp_tx_packet, eth_hdr_size + ARP_HDR_SIZE);
}

uint32_t arp_timeout_check(void)
{
	unsigned long t;

	if (!net_arp_wait_packet_ip.s_addr)
		return 0;

	t = get_timer(0);

	/* check for arp timeout */
	if ((t - arp_wait_timer_start) > ARP_TIMEOUT) {
		arp_wait_try++;

		if (arp_wait_try >= ARP_TIMEOUT_COUNT) {
			print_format("\n\rARP Retry count exceeded; starting again\n\r");
			arp_wait_try = 0;
			net_set_state(NETLOOP_FAIL);
		} else {
			arp_wait_timer_start = t;
			arp_request();
		}
	}
	return 1;
}

void arp_receive(struct ethernet_hdr *et, struct ip_udp_hdr *ip, int len)
{
	struct arp_hdr *arp;
	struct in_addr reply_ip_addr;
	unsigned char *pkt;
	int eth_hdr_size;

	/*
	 * We have to deal with two types of ARP packets:
	 * - REQUEST packets will be answered by sending  our
	 *   IP address - if we know it.
	 * - REPLY packates are expected only after we asked
	 *   for the TFTP server's or the gateway's ethernet
	 *   address; so if we receive such a packet, we set
	 *   the server ethernet address
	 */
	print_format("Got ARP\n\r");
	arp = (struct arp_hdr *)ip;
	if (len < ARP_HDR_SIZE) {
		print_format("bad length %d < %d\n\r", len, ARP_HDR_SIZE);
		return;
	}
#ifdef ARP_DEBUG
	print_format("[DEBUG] ARP dump\n\r");
	print_format("ar_hdr : 	0x%x\n\r",ntohs(arp->ar_hrd));
	print_format("ar_pro : 	0x%x\n\r",ntohs(arp->ar_pro));
	print_format("ar_hln : 	0x%x\n\r",(uint8_t)arp->ar_hln);
	print_format("ar_pln : 	0x%x\n\r",(uint8_t)arp->ar_pln);
	print_format("ar_op  : 	0x%x\n\r",ntohs(arp->ar_op));
	print_format("ar_sha : 	0x%x\n\r",(uint8_t)arp->ar_sha); 
	print_format("ar_sha1 : 0x%x\n\r",(uint8_t) *(&(arp->ar_sha)+1)); 
	print_format("ar_sha2 : 0x%x\n\r",(uint8_t) *(&(arp->ar_sha)+2)); 
	print_format("ar_sha3 : 0x%x\n\r",(uint8_t) *(&(arp->ar_sha)+3)); 
	print_format("ar_sha4 : 0x%x\n\r",(uint8_t) *(&(arp->ar_sha)+4)); 
	print_format("ar_sha5 : 0x%x\n\r",(uint8_t) *(&(arp->ar_sha)+5)); 
	print_format("ar_spa : 	0x%x\n\r",net_read_ip(&arp->ar_spa).s_addr);
	print_format("ar_tha : 	0x%x\n\r",(uint8_t)arp->ar_tha);
	print_format("ar_tha1 : 0x%x\n\r",(uint8_t) *(&(arp->ar_tha)+1));
	print_format("ar_tha2 : 0x%x\n\r",(uint8_t) *(&(arp->ar_tha)+2));
	print_format("ar_tha3 : 0x%x\n\r",(uint8_t) *(&(arp->ar_tha)+3));
	print_format("ar_tha4 : 0x%x\n\r",(uint8_t) *(&(arp->ar_tha)+4));
	print_format("ar_tha5 : 0x%x\n\r",(uint8_t) *(&(arp->ar_tha)+5));
	print_format("ar_tpa : 0x%x\n\r",net_read_ip(&arp->ar_tpa).s_addr); 
	print_format("[DEBUG] ARP dump end\n\r");
	print_format("the registered IP address of this board is 0x%x\n\r",net_ip.s_addr);
	if(net_ip.s_addr == net_read_ip(&arp->ar_tpa).s_addr)
		print_format("This ARP request concerns us!\n\r");
	else
		print_format("This ARP request is not for us!\n\r");
#endif	
	if (ntohs(arp->ar_hrd) != ARP_ETHER)
		return;
	if (ntohs(arp->ar_pro) != PROT_IP)
		return;
	if (arp->ar_hln != ARP_HLEN)
		return;
	if (arp->ar_pln != ARP_PLEN)
		return;

	if (net_ip.s_addr == 0)
		return;

	if (net_read_ip(&arp->ar_tpa).s_addr != net_ip.s_addr)
		return;

	switch (ntohs(arp->ar_op)) {
		case ARPOP_REQUEST:
			/* reply with our IP address */
#ifdef ARP_DEBUG
			print_format("Got ARP REQUEST, return our IP\n\r");
#endif
			pkt = (unsigned char *)et;
			eth_hdr_size = net_update_ether(et, et->et_src, PROT_ARP);
			pkt += eth_hdr_size;
			arp->ar_op = htons(ARPOP_REPLY);
			ul_memcpy(&arp->ar_tha, &arp->ar_sha, ARP_HLEN);
			net_copy_ip(&arp->ar_tpa, &arp->ar_spa);
			ul_memcpy(&arp->ar_sha, net_ethaddr, ARP_HLEN);
			net_copy_ip(&arp->ar_spa, &net_ip);

			net_send_packet((unsigned char *)et, eth_hdr_size + ARP_HDR_SIZE);
			return;

		case ARPOP_REPLY:		/* arp reply */
			/* are we waiting for a reply */
			if (!net_arp_wait_packet_ip.s_addr)
				break;

//#ifdef CONFIG_KEEP_SERVERADDR
//			if (net_server_ip.s_addr == net_arp_wait_packet_ip.s_addr) {
//				unsigned char buf[20];
//				sprintf(buf, "%pM", &arp->ar_sha);
//				setenv("serveraddr", buf);
//			}
//#endif

			reply_ip_addr = net_read_ip(&arp->ar_spa);

			/* matched waiting packet's address */
			if (reply_ip_addr.s_addr == net_arp_wait_reply_ip.s_addr) {
				print_format("Got ARP REPLY, set eth addr (%x)\n\r",
						arp->ar_data);

				/* save address for later use */
				if (arp_wait_packet_ethaddr != NULL)
					ul_memcpy(arp_wait_packet_ethaddr,
							&arp->ar_sha, ARP_HLEN);

			arp_handler((unsigned char *)arp, 0, reply_ip_addr,0, len);

				/* set the mac address in the waiting packet's header
				   and transmit it */
				ul_memcpy(((struct ethernet_hdr *)net_tx_packet)->et_dest,
						&arp->ar_sha, ARP_HLEN);
				net_send_packet(net_tx_packet, arp_wait_tx_packet_size);

				/* no arp request pending now */
				net_arp_wait_packet_ip.s_addr = 0;
				arp_wait_tx_packet_size = 0;
				arp_wait_packet_ethaddr = NULL;
			}
			return;
		default:
			print_format("Unexpected ARP opcode 0x%x\n",
					ntohs(arp->ar_op));
			return;
	}
}

void arp_handler(unsigned char* packet,unsigned int dport,struct in_addr sip,unsigned int sport,unsigned int len)
{
	print_format("arp_handler called with info:...\n\r");
	print_format("packet:\t");
	print_format((char*)packet);
	print_format("\n\rdestination port:\t");
	print_format("%d\n\r",dport);
	print_format("IP address: 0x%x\t",sip);
	print_format("\n\rSource port used:\t");
	print_format("%d\n\r",sport);
	net_set_state(NETLOOP_SUCCESS);
}
