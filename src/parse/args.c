#include "traceroute.h"

#include <stdio.h> // dprintf()
#include <string.h> // strcmp()

static int parse_hostname(t_traceroute_ctx *ctx, char *arg)
{
	if (!ctx->hostname) {
		ctx->hostname = arg;
		return (0);
	}
	dprintf(2, "%s: Error: Too many hostnames\n", ctx->progname);
	return (1);
}

static int parse_flag(t_traceroute_ctx *ctx, int argc, char **argv, int *i)
{
	(void)argc;
	if (strcmp(argv[*i], "-h") == 0
	           || strcmp(argv[*i], "--help") == 0)
		ctx->flags |= FLAG_HELP;
	return (0);
}

int parse_args(t_traceroute_ctx *ctx, int argc, char **argv)
{
	int i;

	ctx->progname = argv[0];
	i = 1;
	while (i < argc) {
		if (argv[i][0] == '-') {
			if (parse_flag(ctx, argc, argv, &i) != 0)
				return (1);
		}
		else
			if (parse_hostname(ctx, argv[i]) != 0)
				return (1);
		i++;
	}
	if (!(ctx->flags & FLAG_HELP) && !ctx->hostname) {
		dprintf(2, "%s: Error: Missing hostname\n", ctx->progname);
		print_help(ctx->progname);
		return (1);
	}
	return (0);
}
