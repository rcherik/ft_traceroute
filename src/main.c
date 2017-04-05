#include "traceroute.h"

static void	usage(void)
{
	printf("Usage: ./traceroute [-h] [-m max_ttl] [-n] [-q nqueries] [-f first_ttl] [target]\n");
	exit(1);
}

void		ft_error(char *s)
{
	printf("%s\n", s);
	exit(1);
}

static void	init_tr(t_tr *tr)
{
	tr->pid = getpid();
	tr->n_probes = 3;
	tr->max_ttl = 30;
	tr->f = 1;
}

static void		get_opt(t_tr *tr, char **argv, int *i)
{
	if (!ft_strcmp(argv[*i], "-h"))
		tr->h = 1;
	else if (!ft_strcmp(argv[*i], "-n"))
		tr->n = 1;
	else if (!ft_strcmp(argv[*i], "-m") && argv[*i + 1] && !ft_str_is_number(argv[*i + 1]))
	{
		tr->max_ttl = ft_atoi(argv[*i + 1]);
		*i += 1;
	}
	else if (!ft_strcmp(argv[*i], "-f") && argv[*i + 1] && !ft_str_is_number(argv[*i + 1])
		&& ft_strcmp(argv[*i + 1], "0"))
	{
		tr->f = ft_atoi(argv[*i + 1]);
		*i += 1;
	}
	else if (!ft_strcmp(argv[*i], "-q") && argv[*i + 1] && !ft_str_is_number(argv[*i + 1]))
	{
		tr->n_probes = ft_atoi(argv[*i + 1]);
		*i += 1;
	}
	else
		usage();
}

static void		check_opt(t_tr *tr, int argc, char **argv)
{
	int		i;
	struct in_addr	tmp;

	i = 1;
	while (argv[i] && argv[i][0] == '-' && i < argc)
	{
		if (!ft_strcmp(argv[i], "-m") || !ft_strcmp(argv[i], "-h")
			|| !ft_strcmp(argv[i], "-f") || !ft_strcmp(argv[i], "-n")
			|| !ft_strcmp(argv[i], "-q"))
			get_opt(tr, argv, &i);
		else
			usage();
		i++;
	}
	if (i + 1 != argc)
		usage();
	tr->target = ft_strdup(argv[i]);
	if (!(inet_pton(AF_INET, tr->target, &tmp)))
		tr->host = get_host(tr->target);
	else
		tr->host = tr->target;
}

int		main(int argc, char **argv)
{
	t_tr			*tr;

	if (argc < 2)
		usage();
	tr = (t_tr *)malloc(sizeof(t_tr));
	ft_bzero(tr, sizeof(t_tr));
	if (getuid())
		ft_error("Error: Root privelidges needed");
	if ((tr->sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		ft_error("Error: recv_socket failed.");
	init_tr(tr);
	check_opt(tr, argc, argv);
	tr->send.sin_family = AF_INET;
	inet_pton(AF_INET, tr->host, &tr->send.sin_addr);
	setuid(getuid());
	if (tr->h)
		usage();
	printf("traceroute to %s (%s), %d hops max, 60 byte packets\n", tr->target, inet_ntoa(tr->send.sin_addr), tr->max_ttl);
	loop(tr);
	return (0);
}
