#include <stdint.h>
#include <ctype.h>
#include "in.h"




#undef  htonl
#undef  ntohl

uint32_t htonl (uint32_t x)
{
#if BYTE_ORDER == BIG_ENDIAN
	return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
	return __bswap_32 (x);
#else
# error "What kind of system is this?"
#endif
}

uint32_t ntohl (uint32_t x) __attribute__  ((weak,alias("htonl")));


uint16_t htons (uint16_t x)
{
#if BYTE_ORDER == BIG_ENDIAN
	return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
	return __bswap_16 (x);
#else
# error "What kind of system is this?"
#endif
}

uint16_t ntohs (uint16_t x) __attribute__  ((weak,alias("htons")));


unsigned long simple_strtoul(const char *cp, char **endp,
		unsigned int base)
{
	unsigned long result = 0;
	unsigned long value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}

		if (!base)
			base = 8;
	}

	if (!base)
		base = 10;

	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
					? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}

	if (endp)
		*endp = (char *)cp;

	return result;
}
