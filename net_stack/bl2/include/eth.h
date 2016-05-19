#ifndef _ETH_H_
#define _ETH_H_

#include <stdint.h>
#include "errno.h"

enum eth_state_t {
	ETH_STATE_INIT,
	ETH_STATE_PASSIVE,
	ETH_STATE_ACTIVE
	};

struct eth_device {
	unsigned char name[16];
	unsigned char enetaddr[6];
	int iobase;
	int state;
	struct eth_ops *ops;

	int  (*init) (struct eth_device*);
	int  (*send) (struct eth_device*, volatile void* packet, int length);
	int  (*recv) (struct eth_device*);
	void (*halt) (struct eth_device*);
	int  (*write_hwaddr) (struct eth_device*);
	int index;
	void *priv;
};


/** * struct eth_ops - functions of Ethernet MAC controllers
 *
 * start: Prepare the hardware to send and receive packets
 * send: Send the bytes passed in "packet" as a packet on the wire
 * recv: Check if the hardware received a packet. If so, set the pointer to the
 *	 packet buffer in the packetp parameter. If not, return an error or 0 to
 *	 indicate that the hardware receive FIFO is empty. If 0 is returned, the
 *	 network stack will not process the empty packet, but free_pkt() will be
 *	 called if supplied
 * free_pkt: Give the driver an opportunity to manage its packet buffer memory
 *	     when the network stack is finished processing it. This will only be
 *	     called when no error was returned from recv - optional
 * stop: Stop the hardware from looking for packets - may be called even if
 *	 state == PASSIVE
 * mcast: Join or leave a multicast group (for TFTP) - optional
 * write_hwaddr: Write a MAC address to the hardware (used to pass it to Linux
     	121 *		 on some platforms like ARM). This function expects the
 *		 eth_pdata::enetaddr field to be populated. The method can
 *		 return -ENOSYS to indicate that this is not implemented for
		 this hardware - optional.
 * read_rom_hwaddr: Some devices have a backup of the MAC address stored in a
 *		    ROM on the board. This is how the driver should expose it
 *		    to the network stack. This function should fill in the
 *		    eth_pdata::enetaddr field - optional
 */
struct eth_ops {
	int (*start)(struct eth_device *dev);
	int (*send)(struct eth_device *dev,volatile void *packet, int length);
//	int (*recv)(struct eth_device *dev, int flags, unsigned char **packetp);
	int (*recv)(struct eth_device *dev);
	int (*free_pkt)(struct eth_device *dev, unsigned char *packet, int length);
	void (*stop)(struct eth_device *dev);
#ifdef CONFIG_MCAST_TFTP
	int (*mcast)(struct eth_device *dev, const uint8_t *enetaddr, int join);
#endif
	int (*write_hwaddr)(struct eth_device *dev);
	int (*read_rom_hwaddr)(struct eth_device *dev);
};

#define eth_get_ops(dev) ((struct eth_ops *)(dev)->ops)


/*prototypes*/

int eth_init(void);
int eth_send(void *packet,int length);
int eth_rx(void);
void eth_halt(void);
int eth_register(struct eth_device *dev);
const char *eth_get_name(void);		/* get name of current device */
int eth_get_dev_index(void);
/*ethernet device through which operations are performed*/
struct eth_device  *eth_current;

#endif /* _ETH_H_*/
