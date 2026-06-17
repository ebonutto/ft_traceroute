#include "traceroute.h"

#include <netinet/ip_icmp.h> // ICMP_ECHOREPLY
#include <sys/time.h> // struct timeval, gettimeofday()

#include <stddef.h> // NULL
#include <stdio.h> // printf()
#include <string.h> // memcpy()

static void update_rtt(t_ping_ctx *ctx, double rtt)
{
	if (ctx->rtt_min < 0 || rtt < ctx->rtt_min)
		ctx->rtt_min = rtt;
	if (rtt > ctx->rtt_max)
		ctx->rtt_max = rtt;
	ctx->rtt_sum += rtt;
	ctx->rtt_sum_sq += rtt * rtt;
}

static int ping_once(t_ping_ctx *ctx, unsigned short seq)
{
	t_icmp_pkt pkt;
	t_icmp_reply reply;
	struct timeval tv_send;
	double rtt;

	pkt = icmp_build(ctx, seq);
	if (icmp_send(ctx, &pkt) != 0)
		return (1);
	ctx->send++;
	if (ctx->flags & FLAG_FLOOD && !(ctx->flags & FLAG_QUIET))
		printf(".");
	if (icmp_recv(ctx, &reply) != 0)
		return (0);
	rtt = 0;
	if (reply.pkt.hdr.type == ICMP_ECHOREPLY) {
		ctx->received++;
		memcpy(&tv_send, reply.pkt.data, sizeof(tv_send));
		rtt = time_diff_ms(&tv_send, &reply.tv_recv);
		update_rtt(ctx, rtt);
	}
	print_response(ctx, &reply, rtt);
	return (0);
}

static int ping_loop(t_ping_ctx *ctx)
{
	unsigned short seq;
	struct timeval tv_before;
	struct timeval tv_after;

	seq = 0;
	while (g_pingloop) {
		gettimeofday(&tv_before, NULL);
		if (ping_once(ctx, seq) != 0)
			return (1);
		seq++;
		if (ctx->count && seq >= (unsigned short)ctx->count)
			break ;
		gettimeofday(&tv_after, NULL);
		wait_remaining_time(ctx, &tv_before, &tv_after);
	}
	return (0);
}

int ping_run(t_ping_ctx *ctx)
{
	int ret;

	if (ping_setup(ctx) != 0)
		return (1);
	if (signal_setup() != 0) {
		ping_cleanup(ctx);
		return (1);
	}
	print_header(ctx);
	ret = ping_loop(ctx);
	print_stats(ctx);
	ping_cleanup(ctx);
	return (ret);
}
