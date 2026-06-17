#include "traceroute.h"

#include <unistd.h> // close()

void ping_cleanup(t_ping_ctx *ctx)
{
	close(ctx->sockfd);
}
