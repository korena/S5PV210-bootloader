#ifndef _IN_H_
#define _IN_H_



/*TODO:move me somewhere sensible*/
#define ARCH_DMA_MINALIGN	64



uint32_t htonl (uint32_t x);
uint32_t ntohl (uint32_t x);
uint16_t htons (uint16_t x);
uint16_t ntohs (uint16_t x);



unsigned long simple_strtoul(const char *cp, char **endp,
		unsigned int base);


#endif /*_IN_H_*/
