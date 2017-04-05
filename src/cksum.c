#include "traceroute.h"

unsigned short	cksum(unsigned short *addr, int len)
{
	unsigned short	result;
	unsigned int		sum;

	sum = 0;
	while (len > 1)
	{
		sum += *addr++;
		len -= 2;
	}
	if (len == 1)
	sum += *(unsigned char*) addr;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return (result);
}

