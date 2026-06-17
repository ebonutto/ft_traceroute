#include "traceroute.h"

#include <stdio.h> // dprintf()
#include <string.h> // strcmp()

static int parse_hostname(t_ping_ctx *ctx, char *arg)
{
	if (!ctx->hostname) {
		ctx->hostname = arg;
		return (0);
	}
	dprintf(2, "%s: Error: Too many hostnames\n", ctx->progname);
	return (1);
}

static int parse_flag(t_ping_ctx *ctx, int argc, char **argv, int *i)
{
	if (strcmp(argv[*i], "-?") == 0
	           || strcmp(argv[*i], "--help") == 0)
		ctx->flags |= FLAG_HELP;
	else if (strcmp(argv[*i], "-c") == 0
	        || strcmp(argv[*i], "--count") == 0) {
		if (parse_value(ctx, argc, argv, i,
		                parse_value_int, &ctx->count) != 0)
			return (1);
	}
	else if (strcmp(argv[*i], "-i") == 0
	         || strcmp(argv[*i], "--interval") == 0) {
		if (parse_value(ctx, argc, argv, i,
		                parse_value_double, &ctx->interval) != 0)
			return (1);
	}
	else if (strcmp(argv[*i], "-t") == 0
	        || strcmp(argv[*i], "--ttl") == 0) {
		if (parse_value(ctx, argc, argv, i,
		                parse_value_unsigned_char, &ctx->ttl) != 0)
			return (1);
	}
	else if (strcmp(argv[*i], "-v") == 0
	         || strcmp(argv[*i], "--verbose") == 0)
		ctx->flags |= FLAG_VERBOSE;
	else if (strcmp(argv[*i], "-f") == 0
	         || strcmp(argv[*i], "--flood") == 0) {
		ctx->flags |= FLAG_FLOOD;
		ctx->interval = 0.01;
	}
	else if (strcmp(argv[*i], "-p") == 0
	        || strcmp(argv[*i], "--pattern") == 0) {
		if (parse_value(ctx, argc, argv, i,
		                parse_value_pattern, &ctx->pattern) != 0)
			return (1);
	}
	else if (strcmp(argv[*i], "-q") == 0
	         || strcmp(argv[*i], "--quiet") == 0)
		ctx->flags |= FLAG_QUIET;
	else {
		dprintf(2, "%s: Error: Option '%s' is invalid\n",
		        ctx->progname, argv[*i]);
		return (1);
	}
	return (0);
}

int parse_args(t_ping_ctx *ctx, int argc, char **argv)
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
