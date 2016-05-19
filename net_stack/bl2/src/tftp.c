/*
 * Copyright 1994, 1995, 2000 Neil Russell.
 * (See License)
 * Copyright 2000, 2001 DENX Software Engineering, Wolfgang Denk, wd@denx.de
 * Copyright 2011 Comelit Group SpA,
 *                Luca Ceresoli <luca.ceresoli@comelit.it>
 */

//#include <common.h>
//#include <command.h>
#include "mapmem.h"
#include "net.h"
#include "tftp.h"
#include "terminal.h"
#include "in.h"
#include "configs.h"
#include "timer.h"
#include <string.h>
//#include "bootp.h"
//#ifdef CONFIG_SYS_DIRECT_FLASH_TFTP
//#include <flash.h>
//#endif

/* Well known TFTP port # */
#define WELL_KNOWN_PORT	69
/* Millisecs to timeout for lost pkt */
#define TIMEOUT		5000UL
#ifndef	CONFIG_NET_RETRY_COUNT
/* # of timeouts before giving up */
# define TIMEOUT_COUNT	10
#else
# define TIMEOUT_COUNT  (CONFIG_NET_RETRY_COUNT * 2)
#endif
/* Number of "loading" hashes per line (for checking the image size) */
#define HASHES_PER_LINE	65

/*
 *	TFTP operations.
 */
#define TFTP_RRQ	1
#define TFTP_WRQ	2
#define TFTP_DATA	3
#define TFTP_ACK	4
#define TFTP_ERROR	5
#define TFTP_OACK	6

static uint32_t timeout_ms = TIMEOUT;
static int timeout_count_max = TIMEOUT_COUNT;
static uint32_t time_start;   /* Record time we started tftp */

/*
 * These globals govern the timeout behavior when attempting a connection to a
 * TFTP server. tftp_timeout_ms specifies the number of milliseconds to
 * wait for the server to respond to initial connection. Second global,
 * tftp_timeout_count_max, gives the number of such connection retries.
 * tftp_timeout_count_max must be non-negative and tftp_timeout_ms must be
 * positive. The globals are meant to be set (and restored) by code needing
 * non-standard timeout behavior when initiating a TFTP transfer.
 */
uint32_t tftp_timeout_ms = TIMEOUT;
int tftp_timeout_count_max = TIMEOUT_COUNT;

enum {
	TFTP_ERR_UNDEFINED           = 0,
	TFTP_ERR_FILE_NOT_FOUND      = 1,
	TFTP_ERR_ACCESS_DENIED       = 2,
	TFTP_ERR_DISK_FULL           = 3,
	TFTP_ERR_UNEXPECTED_OPCODE   = 4,
	TFTP_ERR_UNKNOWN_TRANSFER_ID  = 5,
	TFTP_ERR_FILE_ALREADY_EXISTS = 6,
};

static struct in_addr tftp_remote_ip;
/* The UDP port at their end */
static int	tftp_remote_port;
/* The UDP port at our end */
static int	tftp_our_port;
static int	timeout_count;
/* packet sequence number */
static uint32_t	tftp_cur_block;
/* last packet sequence number received */
static uint32_t	tftp_prev_block;
/* count of sequence number wraparounds */
static uint32_t	tftp_block_wrap;
/* memory offset due to wrapping */
static uint32_t	tftp_block_wrap_offset;
static int	tftp_state;
#ifdef CONFIG_TFTP_TSIZE
/* The file size reported by the server */
static int	tftp_tsize;
/* The number of hashes we printed */
static short	tftp_tsize_num_hash;
#endif
#define tftp_put_active	0
#define STATE_SEND_RRQ	1
#define STATE_DATA	2
#define STATE_TOO_LARGE	3
#define STATE_BAD_MAGIC	4
#define STATE_OACK	5
#define STATE_RECV_WRQ	6
#define STATE_SEND_WRQ	7

/* default TFTP block size */
#define TFTP_BLOCK_SIZE		512
/* sequence number is 16 bit */
#define TFTP_SEQUENCE_SIZE	((uint16_t)(1<<16))

#ifndef CONFIG_TFTP_FILE_NAME_MAX_LEN
#define MAX_LEN 128
#else
#define MAX_LEN CONFIG_TFTP_FILE_NAME_MAX_LEN
#endif

static char tftp_filename[MAX_LEN];

/* 512 is poor choice for ethernet, MTU is typically 1500.
 * Minus eth.hdrs thats 1468.  Can get 2x better throughput with
 * almost-MTU block sizes.  At least try... fall back to 512 if need be.
 * (but those using CONFIG_IP_DEFRAG may want to set a larger block in cfg file)
 */
#ifdef CONFIG_TFTP_BLOCKSIZE
#define TFTP_MTU_BLOCKSIZE CONFIG_TFTP_BLOCKSIZE
#else
#define TFTP_MTU_BLOCKSIZE 1468
#endif

static unsigned short tftp_block_size = TFTP_BLOCK_SIZE;
static unsigned short tftp_block_size_option = TFTP_MTU_BLOCKSIZE;


static inline void store_block(int block, unsigned char *src, uint32_t len)
{
	uint32_t offset = block * tftp_block_size + tftp_block_wrap_offset;
	uint32_t newsize = offset + len;
		void *ptr = map_sysmem(load_addr + offset, len);

		ul_memcpy(ptr, src, len);
		unmap_sysmem(ptr);

	if (net_boot_file_size < newsize)
		net_boot_file_size = newsize;
}

/* Clear our state ready for a new transfer */
static void new_transfer(void)
{
	tftp_prev_block = 0;
	tftp_block_wrap = 0;
	tftp_block_wrap_offset = 0;
}

static void tftp_send(void);
static void tftp_timeout_handler(void);

/**********************************************************************/

static void show_block_marker(void)
{
#ifdef CONFIG_TFTP_TSIZE
	if (tftp_tsize) {
		uint32_t pos = tftp_cur_block * tftp_block_size +
			tftp_block_wrap_offset;
		if (pos > tftp_tsize)
			pos = tftp_tsize;

		while (tftp_tsize_num_hash < pos * 50 / tftp_tsize) {
		uart_print_string("#",1);
			tftp_tsize_num_hash++;
		}
	} else
#endif
	{
		if (((tftp_cur_block - 1) % 10) == 0)
			uart_print_string("#",1);
		else if ((tftp_cur_block % (10 * HASHES_PER_LINE)) == 0)
			uart_print("\n\r\t "); // not really gonna work :-)
	}
}

/**
 * restart the current transfer due to an error
 *
 * @param msg	Message to print for user
 */
static void restart(const char *msg)
{
	uart_print_string(msg,strlen(msg));
	uart_print(";\n\rstarting again\n\r");
	net_start_again();
}

/*
 * Check if the block number has wrapped, and update progress
 *
 * TODO: The egregious use of global variables in this file should be tidied.
 */
static void update_block_number(void)
{
	/*
	 * RFC1350 specifies that the first data packet will
	 * have sequence number 1. If we receive a sequence
	 * number of 0 this means that there was a wrap
	 * around of the (16 bit) counter.
	 */
	if (tftp_cur_block == 0 && tftp_prev_block != 0) {
		tftp_block_wrap++;
		tftp_block_wrap_offset += tftp_block_size * TFTP_SEQUENCE_SIZE;
		timeout_count = 0; /* we've done well, reset the timeout */
	} else {
		show_block_marker();
	}
}

/* The TFTP get or put is complete */
static void tftp_complete(void)
{
#ifdef CONFIG_TFTP_TSIZE
	/* Print hash marks for the last packet received */
	while (tftp_tsize && tftp_tsize_num_hash < 49) {
		uart_print_string("#",1);
		tftp_tsize_num_hash++;
	}
	uart_print_string("  ",2);
	print_format("Size: %d",tftp_tsize);
#endif
	time_start = get_timer(time_start);
	if (time_start > 0) {
		uart_print_string("\n\r\t ",4);	/* Line up with "Loading: " */
		print_format("rate: ",net_boot_file_size /
			time_start * 1000);
	}
	uart_print("\n\rdone\n\r");
	net_set_state(NETLOOP_SUCCESS);
}

static void tftp_send(void)
{
	unsigned char *pkt;
	unsigned char *xp;
	uint32_t len = 0;
	uint16_t *s;

	/*
	 *	We will always be sending some sort of packet, so
	 *	cobble together the packet headers now.
	 */
	pkt = net_tx_packet + net_eth_hdr_size() + IP_UDP_HDR_SIZE;

	switch (tftp_state) {
	case STATE_SEND_RRQ:
	case STATE_SEND_WRQ:
		xp = pkt;
		s = (uint16_t *)pkt;
		*s++ = htons(TFTP_RRQ);
		pkt = (unsigned char *)s;
		strcpy((char *)pkt, tftp_filename);
		pkt += strlen(tftp_filename) + 1;
		strcpy((char *)pkt, "octet");
		pkt += 5 /*strlen("octet")*/ + 1;
		strcpy((char *)pkt, "timeout");
		pkt += 7 /*strlen("timeout")*/ + 1;
		sprintf((char *)pkt, "%lu", timeout_ms / 1000); // XXX please work
#ifdef TFTP_DEBUG
		uart_print("send option timeout ");
		uart_print_string((char *)pkt,strlen((char *)pkt));
		uart_print(" ms\n\r");
#endif
		pkt += strlen((char *)pkt) + 1;
#ifdef CONFIG_TFTP_TSIZE
		pkt += sprintf((char *)pkt, "tsize%c%u%c",
				0, net_boot_file_size, 0);
#endif
		/* try for more effic. blk size */
		pkt += sprintf((char *)pkt, "blksize%c%d%c",
				0, tftp_block_size_option, 0);
		len = pkt - xp;
		break;

	case STATE_OACK:
	case STATE_RECV_WRQ:
	case STATE_DATA:
		xp = pkt;
		s = (uint16_t *)pkt;
		s[0] = htons(TFTP_ACK);
		s[1] = htons(tftp_cur_block);
		pkt = (unsigned char *)(s + 2);
		len = pkt - xp;
		break;

	case STATE_TOO_LARGE:
		xp = pkt;
		s = (uint16_t *)pkt;
		*s++ = htons(TFTP_ERROR);
			*s++ = htons(3);

		pkt = (unsigned char *)s;
		strcpy((char *)pkt, "File too large");
		pkt += 14 /*strlen("File too large")*/ + 1;
		len = pkt - xp;
		break;

	case STATE_BAD_MAGIC:
		xp = pkt;
		s = (uint16_t *)pkt;
		*s++ = htons(TFTP_ERROR);
		*s++ = htons(2);
		pkt = (unsigned char *)s;
		strcpy((char *)pkt, "File has bad magic");
		pkt += 18 /*strlen("File has bad magic")*/ + 1;
		len = pkt - xp;
		break;
	}

	net_send_udp_packet(net_server_ethaddr, tftp_remote_ip,
			    tftp_remote_port, tftp_our_port, len);
}


static void tftp_handler(unsigned char *pkt, uint32_t dest, struct in_addr sip,
			 uint32_t src, uint32_t len)
{
	uint16_t proto;  //XXX changed type from __be16
	uint16_t *s;     //XXX changed type from __be16
	int i;

	if (dest != tftp_our_port) {
			return;
	}
	if (tftp_state != STATE_SEND_RRQ && src != tftp_remote_port &&
	    tftp_state != STATE_RECV_WRQ && tftp_state != STATE_SEND_WRQ)
		return;

	if (len < 2)
		return;
	len -= 2;
	/* warning: don't use increment (++) in ntohs() macros!! */
	s = (uint16_t *)pkt;  //XXX changed type from __be16
	proto = *s++;
	pkt = (unsigned char *)s;
	switch (ntohs(proto)) {
	case TFTP_RRQ:
		break;

	case TFTP_ACK:
		break;

	default:
		break;

	case TFTP_OACK:
#if TFTP_DUBUG
		uart_print("Got OACK: ");
		uart_print_string(pkt,strlen(pkt));
		uart_print_string(" \n\r",4);
#endif
		tftp_state = STATE_OACK;
		tftp_remote_port = src;
		/*
		 * Check for 'blksize' option.
		 * Careful: "i" is signed, "len" is unsigned, thus
		 * something like "len-8" may give a *huge* number
		 */
		for (i = 0; i+8 < len; i++) {
			if (strcmp((char *)pkt + i, "blksize") == 0) {
				tftp_block_size = (unsigned short)
					simple_strtoul((char *)pkt + i + 8,
						       NULL, 10);
#if TFTP_DEBUG
				uart_print("Blocksize ack: ");
				uart_print_string((char *)pkt + i + 8,strlen((char *)pkt + i + 8));
				print_format("0d%d\n\r",tftp_block_size);
#endif
			}
#ifdef CONFIG_TFTP_TSIZE
			if (strcmp((char *)pkt+i, "tsize") == 0) {
				tftp_tsize = simple_strtoul((char *)pkt + i + 6,
							   NULL, 10);
#if TFTP_DEBUG
				uart_print("size = ");
				uart_print_string((char *)pkt + i + 6,strlen((char *)pkt + i + 6));
				print_format("0d%d\n\r",tftp_tsize);
#endif
			}
#endif /* CONFIG_TFTP_TSIZE */
		}
		tftp_send(); /* Send ACK or first data block */
		break;
	case TFTP_DATA:
		if (len < 2)
			return;
		len -= 2;
		tftp_cur_block = ntohs(*(uint16_t *)pkt);  //XXX changed from __be16 type
		update_block_number();
		if (tftp_state == STATE_SEND_RRQ)
#if TFTP_DEBUG
			uart_print("Server did not acknowledge timeout option!\n\r");
#endif
		if (tftp_state == STATE_SEND_RRQ || tftp_state == STATE_OACK ||
		    tftp_state == STATE_RECV_WRQ) {
			/* first block received */
			tftp_state = STATE_DATA;
			tftp_remote_port = src;
			new_transfer();

			if (tftp_cur_block != 1) {	/* Assertion */
				uart_print("\n\rTFTP error: ");
				print_format("First block is not block 1 (%d)\n\r",
				       tftp_cur_block);
				uart_print("Starting again\n\n\r");
				net_start_again();
				break;
			}
		}

		if (tftp_cur_block == tftp_prev_block) {
			/* Same block again; ignore it. */
			break;
		}

		tftp_prev_block = tftp_cur_block;
		timeout_count_max = tftp_timeout_count_max;
		net_set_timeout_handler(timeout_ms, tftp_timeout_handler);

		store_block(tftp_cur_block - 1, pkt + 2, len);

		/*
		 *	Acknowledge the block just received, which will prompt
		 *	the remote for the next one.
		 */
		tftp_send();

		if (len < tftp_block_size)
			tftp_complete();
		break;

	case TFTP_ERROR:
		uart_print("\n\rTFTP error: ");
		uart_print_string(pkt + 2,strlen(pkt + 2));
		print_format("(%d)", ntohs(*(uint16_t *)pkt)); //XXX changed from type __be16

		switch (ntohs(*(uint16_t *)pkt)) {  //XXX changed from type __be16
		case TFTP_ERR_FILE_NOT_FOUND:
		case TFTP_ERR_ACCESS_DENIED:
			uart_print("\n\rNot retrying...\n\r");
			eth_halt();
			net_set_state(NETLOOP_FAIL);
			break;
		case TFTP_ERR_UNDEFINED:
		case TFTP_ERR_DISK_FULL:
		case TFTP_ERR_UNEXPECTED_OPCODE:
		case TFTP_ERR_UNKNOWN_TRANSFER_ID:
		case TFTP_ERR_FILE_ALREADY_EXISTS:
		default:
			uart_print("Starting again\n\n\r");
			net_start_again();
			break;
		}
		break;
	}
}


static void tftp_timeout_handler(void)
{
	if (++timeout_count > timeout_count_max) {
		restart("Retry count exceeded");
	} else {
		uart_print_string("T ",2);
		net_set_timeout_handler(timeout_ms, tftp_timeout_handler);
		if (tftp_state != STATE_RECV_WRQ)
			tftp_send();
	}
}


void tftp_start(enum proto_t protocol)
{
#if TFTP_DEBUG
	print_format("TFTP blocksize = %d, timeout = %d ms\n",
	      tftp_block_size_option, timeout_ms);
#endif
	tftp_remote_ip = net_server_ip;
	if (net_boot_file_name[0] == '\0') {
		strncpy(tftp_filename, "zImage.img", MAX_LEN);
		tftp_filename[MAX_LEN - 1] = 0;

		uart_print("*** Warning: no boot file name; using ");
		uart_print_string(tftp_filename,strlen(tftp_filename));
		uart_print("***\n\r");
	} else {
		char *p = strchr(net_boot_file_name, ':');

		if (p == NULL) {
			strncpy(tftp_filename, net_boot_file_name, MAX_LEN);
			tftp_filename[MAX_LEN - 1] = 0;
		} else {
			tftp_remote_ip = string_to_ip(net_boot_file_name);
			strncpy(tftp_filename, p + 1, MAX_LEN);
			tftp_filename[MAX_LEN - 1] = 0;
		}
	}

	uart_print("Using ");
	uart_print_string(eth_get_name(),strlen(eth_get_name()));
	uart_print( "Device\n\r");
	print_format("TFTP from server 0x%xI4; our IP address is 0x%xI4",
	       tftp_remote_ip, net_ip);
	/* Check if we need to send across this subnet */
	if (net_gateway.s_addr && net_netmask.s_addr) {
		struct in_addr our_net;
		struct in_addr remote_net;

		our_net.s_addr = net_ip.s_addr & net_netmask.s_addr;
		remote_net.s_addr = tftp_remote_ip.s_addr & net_netmask.s_addr;
		if (our_net.s_addr != remote_net.s_addr)
			print_format("; sending through gateway 0x%xI4\n\r", net_gateway);
	}

	uart_print("Filename '");
	uart_print_string(tftp_filename,strlen(tftp_filename));
	uart_print("'.\n\r");
	if (net_boot_file_expected_size_in_blocks) {
		print_format(" Size is 0x%x ",
		       net_boot_file_expected_size_in_blocks << 9);
		print_format("Bytes = %d\n\r",net_boot_file_expected_size_in_blocks << 9);
	}

	{
		print_format("Load address: 0x%x\n\r", load_addr);
		uart_print("Loading: *\b");
		tftp_state = STATE_SEND_RRQ;
	}

	time_start = get_timer(0);
	timeout_count_max = tftp_timeout_count_max;

	net_set_timeout_handler(timeout_ms, tftp_timeout_handler);
	net_set_udp_handler(tftp_handler);
	tftp_remote_port = WELL_KNOWN_PORT;
	timeout_count = 0;
	/* Use a pseudo-random port unless a specific port is set */
	tftp_our_port = 1024 + (get_timer(0) % 3072);
/*
#ifdef CONFIG_TFTP_PORT
	ep = getenv("tftpdstp");
	if (ep != NULL)
		tftp_remote_port = simple_strtol(ep, NULL, 10);
	ep = getenv("tftpsrcp");
	if (ep != NULL)
		tftp_our_port = simple_strtol(ep, NULL, 10);
#endif
*/
	tftp_cur_block = 0;

	/* zero out server ether in case the server ip has changed */
	memset(net_server_ethaddr, 0, 6);
	/* Revert tftp_block_size to dflt */
	tftp_block_size = TFTP_BLOCK_SIZE;
#ifdef CONFIG_TFTP_TSIZE
	tftp_tsize = 0;
	tftp_tsize_num_hash = 0;
#endif

	tftp_send();
}


