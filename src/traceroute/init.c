#include "traceroute.h"

#include <stddef.h> // NULL
#include <string.h> // memset()

void traceroute_init(t_traceroute_ctx *ctx)
{
	/* Args */
	ctx->progname = NULL;
	ctx->hostname = NULL;
	ctx->flags = 0;
	ctx->max_hops = 50; //TODO Change it

	/* Socket */
	ctx->sockfd = -1;
	memset(&ctx->addr, 0, sizeof(ctx->addr));

	/* Stats */
	ctx->nprobes = 50;
	ctx->seq = 0;
}
