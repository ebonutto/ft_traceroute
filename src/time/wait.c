#include "traceroute.h"

#include <sys/time.h> // struct timeval
#include <sys/types.h> // useconds_t

#include <unistd.h> // usleep()

void wait_remaining_time(t_ping_ctx *ctx, struct timeval *before,
                         struct timeval *after)
{
	long elapsed_us;
	long wait_us;

	elapsed_us = (after->tv_sec  - before->tv_sec) * 1000000L
	             + (after->tv_usec - before->tv_usec);
	wait_us = (long)(ctx->interval * 1000000L) - elapsed_us;
	if (wait_us > 0)
		usleep((useconds_t)wait_us);
}
