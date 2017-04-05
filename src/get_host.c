#include "traceroute.h"

static void		set_hints(struct addrinfo *hints)
{
	ft_bzero(hints, sizeof(struct addrinfo));
	hints->ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints->ai_socktype = 0;
	hints->ai_protocol = 0;          /* Any protocol */
	hints->ai_canonname = NULL;
	hints->ai_addr = NULL;
	hints->ai_next = NULL;
	hints->ai_flags = AI_CANONNAME;
}

char		*get_host(char *target)
{
	struct addrinfo		hints;
	struct addrinfo		*ret;
	char			buf[BUFSIZE];
	int			s;

	set_hints(&hints);
	if ((s = getaddrinfo(target, NULL, &hints, &ret)))
		ft_error(ft_strjoin("ft_traceroute: unknown host: ", target));
	while (ret)
	{
		if (ret->ai_family == AF_INET)
		{
			return (ft_strdup(inet_ntop(ret->ai_family,
				&((struct sockaddr_in *)ret->ai_addr)->sin_addr, buf, BUFSIZE)));
		}
		ret = ret->ai_next;
	}
	return (NULL);
}
