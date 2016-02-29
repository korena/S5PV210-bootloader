#ifndef _NET_H_
#define _NET_H_

struct eth_device {
	char name[16];
	unsigned char enetaddr[6];
	int iobase;
	int state;

	//int  (*init) (struct eth_device*, bd_t*);
	int  (*init) (struct eth_device*);
	int  (*send) (struct eth_device*, volatile void* packet, int length);
	int  (*recv) (struct eth_device*);
	void (*halt) (struct eth_device*);
//#ifdef CONFIG_MCAST_TFTP
//	int (*mcast) (struct eth_device*, u32 ip, u8 set);
//#endif
	int  (*write_hwaddr) (struct eth_device*);
//	struct eth_device *next;
	int index;
	void *priv;
};

# define PKTBUFSRX	16 // This is the length of the packet buffer,
			   //this value is just an assumption, we'll see if it fails
			   
char *net_rx_packets[PKTBUFSRX];

#endif
