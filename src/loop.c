#include "traceroute.h"

static void	set_icmp(t_tr *tr, struct icmp *icmp, int seq)
{
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = tr->pid;
	icmp->icmp_seq = ++seq;
	icmp->icmp_cksum = 0;
}

static int	get_icmp(t_tr *tr, char *buf, int seq, struct timeval *time_recv)
{
	struct ip		*ip;
	struct ip		*hdr_ip;
	struct icmp		*icmp;
	struct icmp		*hdr_icmp;
	int			hdr_len1;
	int			hdr_len2;

	gettimeofday(time_recv, NULL);
	ip = (struct ip *)buf;
	hdr_len1 = ip->ip_hl << 2;
	icmp = (struct icmp *)(buf + hdr_len1);
	if (icmp->icmp_type == ICMP_TIMXCEED && icmp->icmp_code == ICMP_TIMXCEED_INTRANS)
	{
		hdr_ip = (struct ip *)(buf + hdr_len1 + 8);
		hdr_len2 = hdr_ip->ip_hl << 2;
		hdr_icmp = (struct icmp *)(buf + hdr_len1 + 8 + hdr_len2);
		if (hdr_icmp->icmp_id == tr->pid && hdr_icmp->icmp_seq == seq)
			return (TIME_TRANSIT);
	}
	if (icmp->icmp_type == ICMP_ECHOREPLY
		&& icmp->icmp_id == tr->pid
		&& icmp->icmp_seq == seq)
		return (PORT_UNREACH);
	return (0);
}

static int	pars(t_tr *tr, int seq, struct timeval *time_recv)
{
	socklen_t		len;
	char			recv_buf[BUFSIZE];
	struct timeval		wait;
	fd_set			fds;

	wait.tv_sec = 0;
//	wait.tv_sec = (int)(tr->w / tr->n_probes);
	wait.tv_usec = 1700000;
	while (42)
	{
		len = sizeof(tr->recv);
		FD_ZERO(&fds);
		FD_SET(tr->sock, &fds);
		if (select(tr->sock + 1, &fds, NULL, NULL, &wait) > 0)
			recvfrom(tr->sock, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&tr->recv, &len);
		else if (!FD_ISSET(tr->sock, &fds))
			return (TIME_EXPIRES);
		else
			ft_error("Error: recvfrom failed.");
		return (get_icmp(tr, recv_buf, seq, time_recv));
	}
}

static void	loop_probe(t_tr *tr, int seq, int *done)
{
	struct icmp	*icmp;
	char		send_buf[BUFSIZE];
	char		tmp[BUFSIZE];
	struct timeval	*time_send;
	struct timeval	time_recv;
	int		ret;
	double		rtt;

	icmp = (struct icmp *)send_buf;
	set_icmp(tr, icmp, seq);
	icmp->icmp_seq = ++seq;
	time_send = (struct timeval *)icmp->icmp_data;
	gettimeofday(time_send, NULL);
	icmp->icmp_cksum = cksum((unsigned short *)icmp, ICMP_LEN);
	if (sendto(tr->sock, send_buf, ICMP_LEN, 0, (struct sockaddr *)&tr->send, sizeof(tr->send)) < 0)
		ft_error("Error: sendto failed");
	if ((ret = pars(tr, seq, &time_recv)) == TIME_EXPIRES)
		printf(" *");
	else
	{
		if (ft_memcmp(&tr->recv.sin_addr, &tr->last.sin_addr, sizeof(tr->recv.sin_addr)))
		{
			if (!(getnameinfo((struct sockaddr *)&(tr->recv), sizeof(tr->recv), tmp, BUFSIZE, NULL, 0, 0)) && !tr->n)
				printf(" %s (%s)", tmp, inet_ntoa(tr->recv.sin_addr));
			else
				printf(" %s", inet_ntoa(tr->recv.sin_addr));
			ft_memcpy(&tr->last.sin_addr, &tr->recv.sin_addr, sizeof(tr->last.sin_addr));
		}
		time_sub(&time_recv, time_send);
		rtt = time_recv.tv_sec * 1000.0 + time_recv.tv_usec / 1000.0;
		printf("  %.3f ms", rtt);
		if (ret == -1)
			++*done;
	}
}

void		loop(t_tr *tr)
{
	int			seq;
	int			done;
	int			ttl;
	int			probe;

	seq = 0;
	done = 0;
	ttl = tr->f;
	while (ttl <= tr->max_ttl && !done)
	{
		probe = 0;
		if (setsockopt(tr->sock, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
			ft_error("Error: setsockopt failed\n");
		ft_bzero(&tr->last, sizeof(tr->last));
		printf("%2d ", ttl);
		while (probe < tr->n_probes)
		{
			loop_probe(tr, seq, &done);
			probe++;
		}
		printf("\n");
		ttl++;
	}
}

