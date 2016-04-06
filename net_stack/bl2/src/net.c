#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "terminal.h"
#include "net.h"
#include "arp.h"
#include "in.h"

static unsigned char net_pkt_buf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];
/* Current receive packet */
unsigned char 	*net_rx_packet;
/* Current rx packet length */
int	net_rx_packet_len;
/* IP packet ID */
static unsigned	net_ip_id;
/* THE transmit packet */
unsigned char *net_tx_packet;

/* in both little & big endian machines 0xFFFF == ntohs(-1) */
/* default is without VLAN */
unsigned short		net_our_vlan = 0xFFFF;
/* ditto */
unsigned short		net_native_vlan = 0xFFFF;

const uint8_t net_ethaddr[6] = {0x00,0x12,0x34,0x56,0x80,0x49};

/* Ethernet bcast address */
const uint8_t net_bcast_ethaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/* Network loop state */
enum net_loop_state net_state;

static unsigned long net_try_count = 0;
static unsigned long net_boot_file_size= 0;
static unsigned long net_restarted = 0;
static unsigned long net_dev_exists = 0;

struct in_addr string_to_ip(const char *s)
{
	struct in_addr addr;
	//	unsigned char *e;
	char *e;
	int i;

	addr.s_addr = 0;
	if (s == NULL)
		return addr;

	for (addr.s_addr = 0, i = 0; i < 4; ++i) {
		uint32_t val = s ? simple_strtoul(s, &e, 10) : 0;
		addr.s_addr <<= 8;
		addr.s_addr |= (val & 0xFF);
		if (s) {
			s = (*e) ? e+1 : e;
		}
	}

	addr.s_addr = htonl(addr.s_addr);
	return addr;
}


void ip_to_string(struct in_addr x, char *s)
{
	char temp_dest[20];
	char temp_num[4];
	x.s_addr = ntohl(x.s_addr);
	printnum(temp_num,4,"%d",(int) ((x.s_addr >> 24) & 0xff));
	strcat(temp_dest,temp_num);
	strcat(temp_dest,".");	
	printnum(temp_num,4,"%d",(int) ((x.s_addr >> 16) & 0xff));
	strcat(temp_dest,temp_num);
	strcat(temp_dest,".");	
	printnum(temp_num,4,"%d",(int) ((x.s_addr >> 8) & 0xff));
	strcat(temp_dest,temp_num);
	strcat(temp_dest,".");	
	printnum(temp_num,4,"%d",(int) ((x.s_addr >> 0) & 0xff));
	strcat(temp_dest,temp_num);
}


static int net_check_prereq(enum proto_t protocol)
{
	switch (protocol) {
		/* Fall through */
	case PING:
//		if (net_ping_ip.s_addr == 0) {
//			print_format("*** ERROR: ping address not given\n");
			return 1;
//		}
		goto common;
	case SNTP:
//		if (net_ntp_server.s_addr == 0) {
//			print_format("*** ERROR: NTP server address not given\n");
			return 1;
//		}
		goto common;
		/* Fall through */
	case TFTPGET:
	case TFTPPUT:
		if (net_server_ip.s_addr == 0) {
			print_format("*** ERROR: `serverip' not set\n\r");
			return 1;
		}
common:
		/* Fall through */
	case NETCONS:
	case TFTPSRV:
		if (net_ip.s_addr == 0) {
			print_format("*** ERROR: `ipaddr' not set\n\r");
			return 1;
		}
		/* Fall through */
	default:
		return 0;
	}
	return 0;		/* OK */
}



void net_init(void)
{
	static int first_call = 1;
	if (first_call) {
		/*
		 *	Setup packet buffers, aligned correctly.
		 */
		int i;

		net_ip = string_to_ip("10.0.0.2");	 
		net_server_ip = string_to_ip("10.0.0.1");
		net_tx_packet = &net_pkt_buf[0] + (PKTALIGN - 1);
		net_tx_packet -= (unsigned long)net_tx_packet % PKTALIGN;
		for (i = 0; i < PKTBUFSRX; i++) {
			net_rx_packets[i] = net_tx_packet +
				(i + 1) * PKTSIZE_ALIGN;
		}
		print_format("initializing ARP ...\n\r");
		arp_init();
		print_format("Done initializing ARP ...\n\r");
		//	net_clear_handlers();
		/* Only need to setup buffer pointers once. */
		first_call = 0;
	}

	/*initialize ethernet device*/
	if(dm9000_initialize() == -1){
		print_format("net_init failed at ethernet device initialization, aborting.\n\r");
		return; 
	}else{
		/* device is initialized! register generic listener for requests...*/

	}

}



/*
 * We want this to handle ARP,TFTP and PING, nothing more!
 * */
int net_loop(enum proto_t protocol)
{
	int ret = -EINVAL;

	net_restarted = 0;
	net_dev_exists = 0;
	net_try_count = 1;
	print_format("--- net_loop Entry\n\r");

	print_format("eth_start, calling net_init\n\r");
	net_init();
	//	if (eth_is_on_demand_init() || protocol != NETCONS) {
	//		eth_halt();
	//		eth_set_current();
	//		ret = eth_init();
	//		if (ret < 0) {
	//			eth_halt();
	//			return ret;
	//		}
	//	} else {
	//		eth_init_state_only();
	//	}
restart:
	net_set_state(NETLOOP_CONTINUE);

	/*
	 *	Start the ball rolling with the given start function.  From
	 *	here on, this code is a state machine driven by received
	 *	packets and timer events.
	 */
	print_format("--- net_loop Init\n\r");

	switch (net_check_prereq(protocol)) {
		case 1:
			/* network not configured */
			print_format("-ERR network not configured, halting device ...\n\r");
			eth_halt();
			return -ENODEV;

		case 2:
			/* network device not configured */
			print_format("-ERR network device not configured ...\n\r");
			break;

		case 0:
			net_dev_exists = 1;
			net_boot_file_size = 0;
			print_format("acting on chosen protocol ...\n\r");
			switch (protocol) {
				case TFTPGET:
					/* always use ARP to get server ethernet address */
		//			tftp_start(protocol);
					break;
				case BOOTP:
		//			bootp_reset();
		//			net_ip.s_addr = 0;
		//			bootp_request();
					break;
				case PING:
		//			ping_start();
					break;
#if defined(CONFIG_CMD_SNTP)
				case SNTP:
		//			sntp_start();
					break;
#endif
				case LINKLOCAL:
		//			link_local_start();
					break;
				default:
					break;
			}

			break;
	}

	/*
	 *	Main packet reception loop.  Loop receiving packets until
	 *	someone sets `net_state' to a state that terminates.
	 */
	print_format("looping and polling ethernet recv ...\n\r");
	for (;;) {

		/*
		 *	Check the ethernet for a new packet.  The ethernet
		 *	receive routine will process it.
		 *	Most drivers return the most recent packet size, but not
		 *	errors that may have happened.
		 */
		eth_rx();

		//		if (arp_timeout_check() > 0) {
		//		    time_start = get_timer(0);
		//		}

		/*
		 *	Check for a timeout, and run the timeout handler
		 *	if we have one.
		 */
		//		if (time_handler &&
		//		    ((get_timer(0) - time_start) > time_delta)) {
		//			thand_f *x;
		//			print_format("--- net_loop timeout\n");
		//			x = time_handler;
		//			time_handler = (thand_f *)0;
		//			(*x)();
		//		}

		if (net_state == NETLOOP_FAIL)
			print_format("some failure was encounterd, good luck debugging !!\n\r");
		switch (net_state) {
			case NETLOOP_RESTART: /*I should never happen ..*/
				net_restarted = 1;
				goto restart;

			case NETLOOP_SUCCESS:
//				net_cleanup_loop(); // clears handlers, not using it  ...
//				if (net_boot_file_size > 0) {
//					print_format("Bytes transferred = %d (%x hex)\n\r",
//							net_boot_file_size, net_boot_file_size);
//				}
//				if (protocol != NETCONS)
//					eth_halt();
//				eth_set_last_protocol(protocol);
//
//				ret = net_boot_file_size;
				print_format("--- net_loop Success!\n\r");
				goto done;

			case NETLOOP_FAIL:
	//			net_cleanup_loop();
				/* Invalidate the last protocol */
				//eth_set_last_protocol(BOOTP);
				print_format("--- net_loop Fail!\n");
				goto done;

			case NETLOOP_CONTINUE:
				continue;
		}
	}

done:
	return ret;
}



void net_process_received_packet(unsigned char *in_packet, int len)
{
	struct ethernet_hdr *et;
	struct ip_udp_hdr *ip;
	struct in_addr dst_ip;
	struct in_addr src_ip;
	int eth_proto;
	unsigned short cti = 0, 
		       vlanid = VLAN_NONE, 
		       myvlanid, 
		       mynvlanid; 
	print_format("packet received\n\r"); 
	net_rx_packet = in_packet; 
	net_rx_packet_len = len; 
	et = (struct ethernet_hdr *)in_packet;

	/* too small packet? */
	if (len < ETHER_HDR_SIZE)
		return;


	myvlanid = ntohs(net_our_vlan);
	if (myvlanid == (unsigned short)-1)
		myvlanid = VLAN_NONE;

	mynvlanid = ntohs(net_native_vlan);
	if (mynvlanid == (unsigned short)-1)
		mynvlanid = VLAN_NONE;

	eth_proto = ntohs(et->et_protlen);


	if (eth_proto < 1514) {
		struct e802_hdr *et802 = (struct e802_hdr *)et;
		/*
		 *	Got a 802.2 packet.  Check the other protocol field.
		 *	XXX VLAN over 802.2+SNAP not implemented!
		 */
		eth_proto = ntohs(et802->et_prot);
		ip = (struct ip_udp_hdr *)(in_packet + E802_HDR_SIZE);
		len -= E802_HDR_SIZE;

	} else if (eth_proto != PROT_VLAN) {	/* normal packet */
		ip = (struct ip_udp_hdr *)(in_packet + ETHER_HDR_SIZE);
		len -= ETHER_HDR_SIZE;
	} else {			/* VLAN packet */
		struct vlan_ethernet_hdr *vet =
			(struct vlan_ethernet_hdr *)et;

		print_format("VLAN packet received\n\r");
		/* too small packet? */
		if (len < VLAN_ETHER_HDR_SIZE)
			return;

		if ((ntohs(net_our_vlan) & VLAN_IDMASK) == VLAN_NONE)
			return;

		cti = ntohs(vet->vet_tag);
		vlanid = cti & VLAN_IDMASK;
		eth_proto = ntohs(vet->vet_type);

		ip = (struct ip_udp_hdr *)(in_packet + VLAN_ETHER_HDR_SIZE);
		len -= VLAN_ETHER_HDR_SIZE;

	}	

	print_format("Receive from protocol 0x%x\n\r", eth_proto);

	if ((myvlanid & VLAN_IDMASK) != VLAN_NONE) {
		if (vlanid == VLAN_NONE)
			vlanid = (mynvlanid & VLAN_IDMASK);
		/* not matched? */
		if (vlanid != (myvlanid & VLAN_IDMASK))
			return;
	}

	switch (eth_proto) {
		case PROT_ARP:
			arp_receive(et, ip, len);
			break;
		default:
			print_format("I promise to never, ever happen :-)\n\r");
	}

}





/*   header fillers   */

int net_set_ether(unsigned char *xet, const unsigned char *dest_ethaddr, uint32_t prot)
{
	struct ethernet_hdr *et = (struct ethernet_hdr *)xet;
	unsigned short myvlanid;

	myvlanid = ntohs(net_our_vlan);
	if (myvlanid == (unsigned short)-1)
		myvlanid = VLAN_NONE;

	memcpy(et->et_dest, dest_ethaddr, 6);
	memcpy(et->et_src, net_ethaddr, 6);
	if ((myvlanid & VLAN_IDMASK) == VLAN_NONE) {
		et->et_protlen = htons(prot);
		return ETHER_HDR_SIZE;
	} else {
		struct vlan_ethernet_hdr *vet =
			(struct vlan_ethernet_hdr *)xet;

		vet->vet_vlan_type = htons(PROT_VLAN);
		vet->vet_tag = htons((0 << 5) | (myvlanid & VLAN_IDMASK));
		vet->vet_type = htons(prot);
		return VLAN_ETHER_HDR_SIZE;
	}
}

int net_update_ether(struct ethernet_hdr *et, unsigned char *addr, unsigned int prot)
{
	unsigned short protlen;

	memcpy(et->et_dest, addr, 6);
	memcpy(et->et_src, net_ethaddr, 6);
	protlen = ntohs(et->et_protlen);
	if (protlen == PROT_VLAN) {
		struct vlan_ethernet_hdr *vet =
			(struct vlan_ethernet_hdr *)et;
		vet->vet_type = htons(prot);
		return VLAN_ETHER_HDR_SIZE;
	} else if (protlen > 1514) {
		et->et_protlen = htons(prot);
		return ETHER_HDR_SIZE;
	} else {
		/* 802.2 + SNAP */
		struct e802_hdr *et802 = (struct e802_hdr *)et;
		et802->et_prot = htons(prot);
		return E802_HDR_SIZE;
	}
}
