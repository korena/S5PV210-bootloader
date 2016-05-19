#include <stdint.h>
#include <string.h>
#include "eth.h"
#include "net.h"
#include "terminal.h"



int eth_register(struct eth_device *dev)
{
	if(dev == NULL)
		return 1;

	if(strlen((const char*)dev->name) > sizeof(dev->name)){
		print_format("error registering ethernet device, name too long\n\r");
		print_format("length of name:%d\n\r",sizeof(dev->name));
	return 2;
	}
	
	dev->state = ETH_STATE_INIT;
	eth_current = dev;
	if(eth_current == NULL)
		return 3;

	return 0;
}

int eth_init(void)
{
	if (!eth_current || (eth_current->state != ETH_STATE_ACTIVE))
		return;
	if(eth_current->init(eth_current) == 0){
		eth_current->state = ETH_STATE_ACTIVE;
		return 0;
	}
	return -1;
}

void eth_halt(void)
{
	if (!eth_current || (eth_current->state != ETH_STATE_ACTIVE))
		return;
	eth_current->halt(eth_current);
	eth_current->state = ETH_STATE_PASSIVE;
}

int eth_is_active(struct eth_device *dev)
{
	if (!dev || (dev->state != ETH_STATE_ACTIVE))
		return 0;
	else
		return 1;
}

int eth_get_dev_index(void)
{
	if (!eth_current)
		return -1;
	return eth_current->index;
}

int eth_send(void *packet, int length){
		int ret;
	     if (!eth_current)
	     		return -ENODEV;
	
	     	if (!(eth_current->state == ETH_STATE_ACTIVE))
	     		return -EINVAL;
	     	ret = eth_current->send(eth_current, packet, length);
	     if (ret < 0) {
	     		/* We cannot completely return the error at present */
	     	print_format(": send() returned error %d\n\r",ret);
	     	}
		return ret;
}

int eth_rx(void)
{
	int ret;
	int i;

	if (!eth_current){
		print_format("eth_current is NULL .. aborting eth_rx\n\r");
     		return -ENODEV;
	}

	if (!(eth_current->state == ETH_STATE_ACTIVE)){
		print_format("Device status is not ACTIVE .. aborting eth_rx\n\r");
     		return -EINVAL;
	}

	/* Process up to 32 packets at one time */
	for (i = 0; i < 32; i++) {
		/*net_rx_packets is the universal packets holster,
		 *the ethernet device will place received packets 
		 *there.
		 * */
//		print_format("calling recv of ethernet device ...\n\r");
		ret = eth_current->recv(eth_current);
		if (ret > 0)
			net_process_received_packet(net_rx_packets[0], ret);
		if (ret >= 0 && 1)
		//	print_format("should clear rx space here?\n\r");
		if (ret <= 0)
			break;
	}
	if (ret == -EAGAIN)
		ret = 0;
	if (ret < 0) {
		/* We cannot completely return the error at present */
		print_format("eth device's recv() returned error %d\n", ret);
	}
	return ret;
}


const char *eth_get_name(void)
{
	return eth_current ? eth_current->name : "unknown";
}
