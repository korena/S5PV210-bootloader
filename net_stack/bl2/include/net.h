#ifndef _NET_H_
#define _NET_H_

#include <stdint.h>
#include <string.h>
#include <terminal.h>
#include "eth.h"
#include "in.h"
#include "io.h"

/*
 *  Note:
 *  All variables of type struct in_addr are stored in NETWORK byte order
 *  (big endian).
 **/
struct in_addr {
	uint32_t s_addr;
};

/* Our subnet mask (0=unknown) */
struct in_addr net_netmask;
/* Our gateways IP address */
struct in_addr net_gateway;
/* Our ethernet address */
const uint8_t net_ethaddr[6] ;
const uint8_t net_null_ethaddr[6];
/* Boot server enet address */
uint8_t net_server_ethaddr[6];
/* Our IP addr (0 = unknown) */
struct in_addr	net_ip;
/* Server IP addr (0 = unknown) */
struct in_addr	net_server_ip;


#define PROT_IP		0x0800		/* IP protocol			*/
#define PROT_ARP	0x0806		/* IP ARP protocol		*/
#define PROT_RARP	0x8035		/* IP ARP protocol		*/
#define PROT_VLAN	0x8100		/* IEEE 802.1q protocol		*/

#define IPPROTO_ICMP	 1	/* Internet Control Message Protocol	*/
#define IPPROTO_UDP	17	/* User Datagram Protocol		*/


/*
 *	Ethernet header
 */

struct ethernet_hdr {
	uint8_t		et_dest[6];	/* Destination node		*/
	uint8_t		et_src[6];	/* Source node			*/
	uint16_t	et_protlen;	/* Protocol or length		*/
};

/* Ethernet header size */
#define ETHER_HDR_SIZE	(sizeof(struct ethernet_hdr))


#define ETH_FCS_LEN	4		/* Octets in the FCS		*/

struct e802_hdr {
	uint8_t		et_dest[6];	/* Destination node		*/
	uint8_t		et_src[6];	/* Source node			*/
	uint16_t	et_protlen;	/* Protocol or length		*/
	uint8_t		et_dsap;	/* 802 DSAP			*/
	uint8_t		et_ssap;	/* 802 SSAP			*/
	uint8_t		et_ctl;		/* 802 control			*/
	uint8_t		et_snap1;	/* SNAP				*/
	uint8_t		et_snap2;
	uint8_t		et_snap3;
	uint16_t	et_prot;	/* 802 protocol			*/
};

/* 802 + SNAP + ethernet header size */
#define E802_HDR_SIZE	(sizeof(struct e802_hdr))




/*
 *	Virtual LAN Ethernet header
 */
struct vlan_ethernet_hdr {
	uint8_t		vet_dest[6];	/* Destination node		*/
	uint8_t		vet_src[6];	/* Source node			*/
	uint16_t	vet_vlan_type;	/* PROT_VLAN			*/
	uint16_t	vet_tag;	/* TAG of VLAN			*/
	uint16_t	vet_type;	/* protocol type		*/
};

/* VLAN Ethernet header size */
#define VLAN_ETHER_HDR_SIZE	(sizeof(struct vlan_ethernet_hdr))

struct ip_hdr {
	uint8_t		ip_hl_v;	/* header length and version	*/
	uint8_t		ip_tos;		/* type of service		*/
	uint16_t	ip_len;		/* total length			*/
	uint16_t	ip_id;		/* identification		*/
	uint16_t	ip_off;		/* fragment offset field	*/
	uint8_t		ip_ttl;		/* time to live			*/
	uint8_t		ip_p;		/* protocol			*/
	uint16_t	ip_sum;		/* checksum			*/
	struct in_addr	ip_src;		/* Source IP address		*/
	struct in_addr	ip_dst;		/* Destination IP address	*/

};

#define IP_OFFS		0x1fff /* ip offset *= 8 */
#define IP_FLAGS	0xe000 /* first 3 bits */
#define IP_FLAGS_RES	0x8000 /* reserved */
#define IP_FLAGS_DFRAG	0x4000 /* don't fragments */
#define IP_FLAGS_MFRAG	0x2000 /* more fragments */

#define IP_HDR_SIZE		(sizeof(struct ip_hdr))

/*
 *   ICMP stuff (just enough to handle (host) redirect messages)
 */
#define ICMP_ECHO_REPLY		0	/* Echo reply			*/
#define ICMP_NOT_REACH		3	/* Detination unreachable	*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO_REQUEST	8	/* Echo request			*/

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/

/* Codes for NOT_REACH */
#define ICMP_NOT_REACH_PORT	3	/* Port unreachable		*/

struct icmp_hdr {
	uint8_t		type;
	uint8_t		code;
	uint16_t	checksum;
	union {
		struct {
			uint16_t	id;
			uint16_t	sequence;
		} echo;
		uint32_t	gateway;
		struct {
			uint16_t	unused;
			uint16_t	mtu;
		} frag;
		uint8_t data[0];
	} un;
};

#define ICMP_HDR_SIZE		(sizeof(struct icmp_hdr))
#define IP_ICMP_HDR_SIZE	(IP_HDR_SIZE + ICMP_HDR_SIZE)


/*
 *	Address Resolution Protocol (ARP) header.
 **/
struct arp_hdr {
	uint16_t	ar_hrd;		/* Format of hardware address	*/
#define ARP_ETHER       1		/* Ethernet  hardware address	*/
	uint16_t	ar_pro;		/* Format of protocol address	*/
	uint8_t		ar_hln;		/* Length of hardware address	*/
#define ARP_HLEN	6
	uint8_t		ar_pln;		/* Length of protocol address	*/
#define ARP_PLEN	4
	uint16_t	ar_op;		/* Operation			*/
#define ARPOP_REQUEST   1		/* Request  to resolve  address	*/
#define ARPOP_REPLY	2		/* Response to previous request	*/

#define RARPOP_REQUEST  3		/* Request  to resolve  address	*/
#define RARPOP_REPLY	4		/* Response to previous request */

	/*
	 * The remaining fields are variable in size, according to
	 * the sizes above, and are defined as appropriate for
	 * specific hardware/protocol combinations.
	 */
	uint8_t		ar_data[0];
#define ar_sha		ar_data[0]
#define ar_spa		ar_data[ARP_HLEN]
#define ar_tha		ar_data[ARP_HLEN + ARP_PLEN]
#define ar_tpa		ar_data[ARP_HLEN + ARP_PLEN + ARP_HLEN]
#if 0
	uint8_t		ar_sha[];	/* Sender hardware address	*/
	uint8_t		ar_spa[];	/* Sender protocol address	*/
	uint8_t		ar_tha[];	/* Target hardware address	*/
	uint8_t		ar_tpa[];	/* Target protocol address	*/
#endif /* 0 */
};

#define ARP_HDR_SIZE	(8+20)		/* Size assuming ethernet	*/


/*
 * 	Internet Protocol (IP) + UDP header.
 **/
struct ip_udp_hdr {
	uint8_t		ip_hl_v;	/* header length and version	*/
	uint8_t		ip_tos;		/* type of service		*/
	uint16_t	ip_len;		/* total length			*/
	uint16_t	ip_id;		/* identification		*/
	uint16_t	ip_off;		/* fragment offset field	*/
	uint8_t		ip_ttl;		/* time to live			*/
	uint8_t		ip_p;		/* protocol			*/
	uint16_t	ip_sum;		/* checksum			*/
	struct in_addr	ip_src;		/* Source IP address		*/
	struct in_addr	ip_dst;		/* Destination IP address	*/
	uint16_t	udp_src;	/* UDP source port		*/
	uint16_t	udp_dst;	/* UDP destination port		*/
	uint16_t	udp_len;	/* Length of UDP packet		*/
	uint16_t	udp_xsum;	/* Checksum			*/
};
#define IP_UDP_HDR_SIZE		(sizeof(struct ip_udp_hdr))
#define UDP_HDR_SIZE		(IP_UDP_HDR_SIZE - IP_HDR_SIZE)

#define PKTBUFSRX	4 // This is the length of the packet buffer,
#define PKTSIZE_ALIGN	1536
#define PKTALIGN	ARCH_DMA_MINALIGN
//this value is just an assumption, we'll see if it fails

uint8_t *net_rx_packets[PKTBUFSRX];
extern unsigned char *net_rx_packet;
extern unsigned char *net_tx_packet;
#define CONFIG_SERVER_IP

#define VLAN_NONE	4095			/* untagged */
#define VLAN_IDMASK	0x0fff			/* mask of valid vlan id */
extern unsigned short		net_our_vlan;		/* Our VLAN */
extern unsigned short		net_native_vlan;	/* Our Native VLAN */
extern int dm9000_initialize(void);
enum proto_t {
	BOOTP, RARP, ARP, TFTPGET, DHCP, PING, DNS, NFS, CDP, NETCONS, SNTP,
	TFTPSRV, TFTPPUT, LINKLOCAL
};

extern char net_boot_file_name[1024];/* Boot File name */
/* The actual transferred size of the bootfile (in bytes) */
extern uint32_t	net_boot_file_size;
/* Boot file size in blocks as reported by the DHCP server */
extern uint32_t net_boot_file_expected_size_in_blocks;



/*
 *	A timeout handler.  Called after time interval has expired.
 */
typedef void thand_f(void);

/**
 * An incoming packet handler.
 * @param pkt    pointer to the application packet
 * @param dport  destination UDP port
 * @param sip    source IP address
 * @param sport  source UDP port
 * @param len    packet length
 */
typedef void rxhand_f(unsigned char *pkt, unsigned dport,
		      struct in_addr sip, unsigned sport,
		      unsigned len);
/* Callbacks */
rxhand_f *net_get_udp_handler(void);	/* Get UDP RX packet handler */
void net_set_udp_handler(rxhand_f *);	/* Set UDP RX packet handler */
//rxhand_f *net_get_arp_handler(void);	/* Get ARP RX packet handler */
//void net_set_arp_handler(rxhand_f *);	/* Set ARP RX packet handler */
//void net_set_icmp_handler(rxhand_icmp_f *f); /* Set ICMP RX handler */
void net_set_timeout_handler(unsigned long, thand_f *);/* Set timeout handler */

/* Network loop state */
enum net_loop_state {
	NETLOOP_CONTINUE,
	NETLOOP_RESTART,
	NETLOOP_SUCCESS,
	NETLOOP_FAIL
};
extern enum net_loop_state net_state;


/*PROTOTYPES*/ 
int net_loop(enum proto_t protocol);
void net_init(void);
/* Load failed.	 Start again. */
int net_start_again(void);
void ip_to_string(struct in_addr x, char *s);
struct in_addr string_to_ip(const char *s);
/* Get size of the ethernet header when we send */
int net_eth_hdr_size(void);
int net_set_ether(unsigned char *xet, const unsigned char *dest_ethaddr, uint32_t prot);
int net_update_ether(struct ethernet_hdr *et, unsigned char *addr, unsigned int prot);
void net_process_received_packet(unsigned char *in_packet, int len);

/**
 * Some functions claiming alignment issues on ARM, 
 * I will trust those, no time to investigate.
 * */
static inline struct in_addr net_read_ip(void *from)
{
	struct in_addr ip;
	ul_memcpy((void *)&ip, (void *)from, sizeof(ip));
	return ip;
}


/* return ulong *in network byteorder* */
static inline uint32_t net_read_u32(uint32_t *from)
{
	uint32_t l;

	ul_memcpy((void *)&l, (void *)from, sizeof(l));
	return l;
}

/* write IP *in network byteorder* */
static inline void net_write_ip(void *to, struct in_addr ip)
{
	ul_memcpy(to, (void *)&ip, sizeof(ip));
}

/* copy IP */
static inline void net_copy_ip(void *to, void *from)
{
	ul_memcpy((void *)to, from, sizeof(struct in_addr));
}

/* copy unsigned long */
static inline void net_copy_u32(uint32_t *to, uint32_t *from)
{
	ul_memcpy((void *)to, (void *)from, sizeof(uint32_t));
}


static inline void net_send_packet(unsigned char *pkt, int len)
{
	/* Currently no way to return errors from eth_send() */
	(void) eth_send(pkt, len);
}

/*
 * Transmit "net_tx_packet" as UDP packet, performing ARP request if needed
 *  (ether will be populated)
 *
 * @param ether Raw packet buffer
 * @param dest IP address to send the datagram to
 * @param dport Destination UDP port
 * @param sport Source UDP port
 * @param payload_len Length of data after the UDP header
 */
int net_send_udp_packet(unsigned char *ether, struct in_addr dest, int dport,
			int sport, int payload_len);

static inline void net_set_state(enum net_loop_state state)
{
	print_format("--- NetState set to %d\n\r", state);
	net_state = state;
}
/* Set ethernet header; returns the size of the header */
void net_set_ip_header(unsigned char *pkt, struct in_addr dest, struct in_addr source);
int net_set_ether(unsigned char *xet, const unsigned char *dest_ethaddr, uint32_t prot);
int net_update_ether(struct ethernet_hdr *et, unsigned char *addr, unsigned int prot);
void net_set_udp_header(unsigned char *pkt, struct in_addr dest, int dport,
				int sport, int len);

/**
 * compute_ip_checksum() - Compute IP checksum
 *
 * @addr:	Address to check (must be 16-bit aligned)
 * @nbytes:	Number of bytes to check (normally a multiple of 2)
 * @return 16-bit IP checksum
 */
 unsigned compute_ip_checksum(const void *addr, unsigned nbytes);


#endif
