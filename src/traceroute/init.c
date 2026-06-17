#include "traceroute.h"

#include <stddef.h> // NULL
#include <string.h> // memset()

void ping_init(t_ping_ctx *ctx)
{
	/* Args */
	ctx->progname = NULL;
	ctx->hostname = NULL;
	ctx->flags = 0;
	ctx->count = 0;
	ctx->interval = 1.0;
	ctx->ttl = 64;
	ctx->pattern = 0;

	/* Socket */
	ctx->sockfd = -1;
	memset(&ctx->addr, 0, sizeof(ctx->addr));

	/* Stats */
	ctx->send = 0;
	ctx->received = 0;
	ctx->rtt_min = -1.0;
	ctx->rtt_max = -1.0;
	ctx->rtt_sum = 0.0;
	ctx->rtt_sum_sq = 0.0;
}
