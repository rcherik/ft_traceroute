#ifndef TRACEROUTE_H
# define TRACEROUTE_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <sys/time.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include "libft.h"

# define BUFSIZE 1024
# define TIME_EXPIRES -3
# define TIME_TRANSIT -2
# define PORT_UNREACH -1
# define ICMP_LEN 64

typedef struct			s_tr
{
	int			pid;
	int			sock;
	int			n_probes;
	int			max_ttl;
	struct sockaddr_in	send;
	struct sockaddr_in	recv;
	struct sockaddr_in	last;
	char			*target;
	char			*host;
	int			h;
	int			q;
	int			n;
	int			f;
}
				t_tr;
void				loop(t_tr *tr);
void				ft_error(char *s);
void				time_sub(struct timeval *out, struct timeval *in);
unsigned short			cksum(unsigned short *addr, int len);
char				*get_host(char *target);

#endif
