#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "timer.h"
#include "terminal.h"
#include "net.h"
#include "arp.h"
#include "tftp.h"
#include "in.h"
#include "io.h"
#include "configs.h"



//#define NET_DEBUG


static unsigned char net_pkt_buf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];
/* Current receive packet */
unsigned char 	*net_rx_packet;
/* Current rx packet length */
int	net_rx_packet_len;
/* Current UDP RX packet handler */
static rxhand_f *udp_packet_handler;
/* Current ARP RX packet handler */
static rxhand_f *arp_packet_handler;
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
/* Tried all network devices */
int net_restart_wrap;
/* Current timeout handler */
static thand_f *time_handler;
/* Time base value */
static ulong	time_start;
/* Current timeout value */
static ulong	time_delta;



static unsigned long net_try_count = 0;
static unsigned long net_restarted = 0;
static unsigned long net_dev_exists = 0;
/* Boot File name */
char net_boot_file_name[1024];
/* The actual transferred size of the bootfile (in bytes) */
uint32_t net_boot_file_size;
/* Boot file size in blocks as reported by the DHCP server */
uint32_t net_boot_file_expected_size_in_blocks;

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
	char temp_dest[16];
	char temp_num[4];
	print_format("first\n\r");
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
	case TFTPGET:
		if (net_server_ip.s_addr == 0) {
			print_format("*** ERROR: `serverip' not set\n\r");
			return 1;
		}
		/* Fall through */
	case NETCONS:
	case TFTPSRV:
		if (net_ip.s_addr == 0) {
			print_format("*** ERROR: `ipaddr' not set\n\r");
			return 1;
		}
		/* Fall through */
	case BOOTP:
	case CDP:
	case DHCP:
	case LINKLOCAL:
		if (memcmp(net_ethaddr, "\0\0\0\0\0\0", 6) == 0) {
			int num = eth_get_dev_index();

			switch (num) {
			case -1:
				print_format("*** ERROR: No ethernet found.\n");
				return 1;
			case 0:
				print_format("*** ERROR: `ethaddr' not set\n");
				break;
			default:
				print_format("*** ERROR: `eth%daddr' not set\n",
				       num);
				break;
			}

			net_start_again();
			return 2;
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
		net_server_ip = string_to_ip("10.0.0.20");
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
	if(dm9000_initialize() == 0){
		/* device is initialized! register generic listener for requests...*/
		print_format("dm9000_initialization complete.\n\r");
		eth_current->state = ETH_STATE_ACTIVE;
		return;
	}else{
		print_format("net_init failed at ethernet device initialization, aborting.\n\r");
		return; 
	}

}

static void start_again_timeout_handler(void)
{
	net_set_state(NETLOOP_RESTART);
}

int net_start_again(void)
{
	int retry_forever = 0;
	unsigned long retrycnt = 0;
	int ret;

		retrycnt = NET_RETRY;
		retry_forever = NET_RETRY_FOREVER;

	if ((!retry_forever) && (net_try_count >= retrycnt)) {
		eth_halt();
		net_set_state(NETLOOP_FAIL);
		/*
		 * We don't provide a way for the protocol to return an error,
		 * but this is almost always the reason.
		 */
		return -ETIMEDOUT;
	}

	net_try_count++;

	eth_halt();

	ret = eth_init();
	if (net_restart_wrap) {
		net_restart_wrap = 0;
		if (net_dev_exists) {
			net_set_timeout_handler(10000UL,
						start_again_timeout_handler);
			net_set_udp_handler(NULL);
		} else {
			net_set_state(NETLOOP_FAIL);
		}
	} else {
		net_set_state(NETLOOP_RESTART);
	}
	return ret;
}

/*
 * We want this to handle ARP and TFTP, nothing more!
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
					tftp_start(protocol);
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
	int ethStatus ;
	for (;;) {

		/*
		 *	Check the ethernet for a new packet.  The ethernet
		 *	receive routine will process it.
		 *	Most drivers return the most recent packet size, but not
		 *	errors that may have happened.
		 */
		ethStatus = eth_rx();

				if (arp_timeout_check() > 0) {
				    time_start = get_timer(0);
				}

		/*
		 *	Check for a timeout, and run the timeout handler
		 *	if we have one.
		 */
				if (time_handler &&
				    ((get_timer(0) - time_start) > time_delta)) {
					thand_f *x;
					print_format("--- net_loop timeout\n\r");
					x = time_handler;
					time_handler = (thand_f *)0;
					(*x)();
				}

		if (net_state == NETLOOP_FAIL)
			ret = net_start_again();

		switch (net_state) {
			case NETLOOP_RESTART: 
				net_restarted = 1;
				goto restart;

			case NETLOOP_SUCCESS:
				if (net_boot_file_size > 0) {
					print_format("Bytes transferred = %d (%x hex)\n\r",
							net_boot_file_size, net_boot_file_size);
				}
				if (protocol != NETCONS)
					eth_halt();
				//eth_set_last_protocol(protocol);

				ret = net_boot_file_size;
				print_format("--- net_loop Success!\n\r");
				goto done;

			case NETLOOP_FAIL:
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
#ifdef NET_DEBUG
	//TODO: This is where you should check if the ethernet header is actually populated with 
	//      proper data, do this by dumping 
	if(et){
		print_format("pointer et is not NULL\n\r");
		print_format("uint8_t et_dest[0] =0x%x \n\r",et->et_dest[0]);
		print_format("uint8_t et_dest[1] =0x%x \n\r",et->et_dest[1]);
		print_format("uint8_t et_dest[2] =0x%x \n\r",et->et_dest[2]);
		print_format("uint8_t et_dest[3] =0x%x \n\r",et->et_dest[3]);
		print_format("uint8_t et_dest[4] =0x%x \n\r",et->et_dest[4]);
		print_format("uint8_t et_dest[5] =0x%x \n\r",et->et_dest[5]);
		print_format("uint8_t et_src[0] = 0x%x \n\r",et->et_src[0]);
		print_format("uint8_t et_src[1] = 0x%x \n\r",et->et_src[1]);
		print_format("uint8_t et_src[2] = 0x%x \n\r",et->et_src[2]);
		print_format("uint8_t et_src[3] = 0x%x \n\r",et->et_src[3]);
		print_format("uint8_t et_src[4] = 0x%x \n\r",et->et_src[4]);
		print_format("uint8_t et_src[5] = 0x%x \n\r",et->et_src[5]);
		print_format("uint16_t et_protlen = 0x%x\n\r",et->et_protlen);
	}else{
		print_format("the pointer et is MULL\n\r");
		while(1); // something is wrong, and you need to fix the bug.
	}
#endif
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

	} else if (eth_proto != PROT_VLAN) {
		/* normal packet */
		ip = (struct ip_udp_hdr *)(in_packet + ETHER_HDR_SIZE); // padding into the rx_packet by ethernet hdr size ...
		len -= ETHER_HDR_SIZE;
#ifdef NET_DEBUG
		print_format("header length and version: 0x%x\n\r",ip->ip_hl_v);
		print_format("type of service: 0x%x\n\r",ip->ip_tos);
		print_format("total length: 0x%x\n\r",ip->ip_len);
		print_format("identification: 0x%x\n\r",ip->ip_id);
		print_format("fragment offset field: 0x%x\n\r",ip->ip_off);
		print_format("time to live: 0x%x\n\r",ip->ip_ttl);
		print_format("protocol: 0x%x\n\r",ip->ip_p);
		print_format("checksum: 0x%x\n\r",ip->ip_sum);

		print_format("UDP source port: 0x%x\n\r",ip->udp_src);
		print_format("UDP dest port: 0x%x\n\r",ip->udp_dst);
		print_format("length of UDP packet: 0x%x\n\r",ip->udp_len);
		print_format("UDP checksum: 0x%x\n\r",ip->udp_xsum);
		
		// dump the whole IP thing !
//		for(int zip=0;zip<len;zip++){
//			print_format("B_%d = 0x%x\n\r",zip,*((uint8_t*)ip+zip));
//		}

#endif
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
#ifdef NET_DEBUG
	print_format("Receive from protocol 0x%x\n\r", eth_proto);
#endif

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

int net_send_udp_packet(unsigned char *ether, struct in_addr dest, int dport, int sport,
		int payload_len)
{
	unsigned char *pkt;
	int eth_hdr_size;
	int pkt_hdr_size;

	/* make sure the net_tx_packet is initialized (net_init() was called) */
	if (net_tx_packet == NULL)
		return -1;

	/* convert to new style broadcast */
	if (dest.s_addr == 0)
		dest.s_addr = 0xFFFFFFFF;

	/* if broadcast, make the ether address a broadcast and don't do ARP */
	if (dest.s_addr == 0xFFFFFFFF)
		ether = (unsigned char *)net_bcast_ethaddr;

	pkt = (unsigned char *)net_tx_packet;

	eth_hdr_size = net_set_ether(pkt, ether, PROT_IP);
	pkt += eth_hdr_size;
	net_set_udp_header(pkt, dest, dport, sport, payload_len);
	pkt_hdr_size = eth_hdr_size + IP_UDP_HDR_SIZE;

	/* if MAC address was not discovered yet, do an ARP request */
	if (memcmp(ether, net_null_ethaddr, 6) == 0) {
		print_format("sending ARP for 0x%xI4\n\r",dest.s_addr);

		/* save the ip and eth addr for the packet to send after arp */
		net_arp_wait_packet_ip = dest;
		arp_wait_packet_ethaddr = ether;

		/* size of the waiting packet */
		arp_wait_tx_packet_size = pkt_hdr_size + payload_len;

		/* and do the ARP request */
		arp_wait_try = 1;
		arp_wait_timer_start = get_timer(0);
		arp_request();
		return 1;	/* waiting */
	} else {
		print_format("sending UDP to 0x%xI4\n\r",dest.s_addr);
		net_send_packet(net_tx_packet, pkt_hdr_size + payload_len);
		return 0;	/* transmitted */
	}
}



/*   header fillers   */


int net_eth_hdr_size(void)
{
	unsigned short myvlanid;

	myvlanid = ntohs(net_our_vlan);
	if (myvlanid == (unsigned short)-1)
		myvlanid = VLAN_NONE;

	return ((myvlanid & VLAN_IDMASK) == VLAN_NONE) ? ETHER_HDR_SIZE :
		VLAN_ETHER_HDR_SIZE;
}



int net_set_ether(unsigned char *xet, const unsigned char *dest_ethaddr, uint32_t prot)
{
	struct ethernet_hdr *et = (struct ethernet_hdr *)xet;
	unsigned short myvlanid;

	myvlanid = ntohs(net_our_vlan);
	if (myvlanid == (unsigned short)-1)
		myvlanid = VLAN_NONE;

	ul_memcpy(et->et_dest, dest_ethaddr, 6);
	ul_memcpy(et->et_src, net_ethaddr, 6);
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

	ul_memcpy(et->et_dest, addr, 6);
	ul_memcpy(et->et_src, net_ethaddr, 6);
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


void net_set_ip_header(unsigned char *pkt, struct in_addr dest, struct in_addr source)
{
	struct ip_udp_hdr *ip = (struct ip_udp_hdr *)pkt;

	/*
	 *	Construct an IP header.
	 */
	/* IP_HDR_SIZE / 4 (not including UDP) */
	ip->ip_hl_v  = 0x45;
	ip->ip_tos   = 0;
	ip->ip_len   = htons(IP_HDR_SIZE);
	ip->ip_id    = htons(net_ip_id++);
	ip->ip_off   = htons(IP_FLAGS_DFRAG);	/* Don't fragment */
	ip->ip_ttl   = 255;
	ip->ip_sum   = 0;
	/* already in network byte order */
	net_copy_ip((void *)&ip->ip_src, &source);
	/* already in network byte order */
	net_copy_ip((void *)&ip->ip_dst, &dest);
}


void net_set_udp_header(unsigned char *pkt, struct in_addr dest, int dport, int sport,
			int len)
{
	struct ip_udp_hdr *ip = (struct ip_udp_hdr *)pkt;

	/*
	 *	If the data is an odd number of bytes, zero the
	 *	byte after the last byte so that the checksum
	 *	will work.
	 */
	if (len & 1)
		pkt[IP_UDP_HDR_SIZE + len] = 0;

	net_set_ip_header(pkt, dest, net_ip);
	ip->ip_len   = htons(IP_UDP_HDR_SIZE + len);
	ip->ip_p     = IPPROTO_UDP;
	ip->ip_sum   = compute_ip_checksum(ip, IP_HDR_SIZE);

	ip->udp_src  = htons(sport);
	ip->udp_dst  = htons(dport);
	ip->udp_len  = htons(UDP_HDR_SIZE + len);
	ip->udp_xsum = 0;
}

unsigned compute_ip_checksum(const void *vptr, unsigned nbytes)
{
	int sum, oddbyte;
	const unsigned short *ptr = vptr;

	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}
	if (nbytes == 1) {
		oddbyte = 0;
		((uint8_t *)&oddbyte)[0] = *(uint8_t *)ptr;
		((uint8_t *)&oddbyte)[1] = 0;
		sum += oddbyte;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	sum = ~sum & 0xffff;

	return sum;
}


static void dummy_handler(unsigned char *pkt, unsigned dport,
			struct in_addr sip, unsigned sport,
			unsigned len)
{
}

rxhand_f *net_get_udp_handler(void)
{
	return udp_packet_handler;
}
void net_set_udp_handler(rxhand_f *f)
{
	print_format("--- net_loop UDP handler set \n\r", f);
	if (f == NULL)
		udp_packet_handler = dummy_handler;
	else
		udp_packet_handler = f;
}

void net_set_timeout_handler(unsigned long iv, thand_f *f)
{
	if (iv == 0) {
		print_format("--- net_loop timeout handler cancelled\n\r");
		time_handler = (thand_f *)0;
	} else {
		print_format("--- net_loop timeout handler set\n\r");
		time_handler = f;
		time_start = get_timer(0);
		time_delta = iv/1000; // timeout in milliseconds/1000
	}
}
