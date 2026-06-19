#include "traceroute.h"

#include <netinet/ip_icmp.h> // ICMP_ECHOREPLY
#include <sys/time.h> // struct timeval, gettimeofday()

#include <stddef.h> // NULL
#include <stdio.h> // printf()
#include <string.h> // memcpy()

static int probe_once(t_traceroute_ctx *ctx, int ttl, unsigned short seq, double *rtt_out)
{
	t_icmp_pkt  pkt;
	t_icmp_reply reply;
	struct timeval tv_send;

	setsockopt(ctx->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	pkt = icmp_build(ctx, seq);
	if (icmp_send(ctx, &pkt) != 0)
		return (-1); // erreur
	if (icmp_recv(ctx, &reply) != 0)
		return (0); // timeout → afficher '*'
	memcpy(&tv_send, reply.pkt.data, sizeof(tv_send));
	*rtt_out = time_diff_ms(&tv_send, &reply.tv_recv);
	if (reply.pkt.hdr.type == ICMP_ECHOREPLY)
		return (2); // destination atteinte
	return (1); // TIME_EXCEEDED, continuer
}

int traceroute(t_traceroute_ctx *ctx)
{
	int ttl;
	int reached;
	int probe;
	double rtt;
	int r;

	if (traceroute_setup(ctx) != 0)
		return (1);

	for (ttl = 1; ttl <= ctx->max_hops; ttl++) {
		printf("%2d  ", ttl);
		reached = 0;
		for (probe = 0; probe < ctx->nprobes; probe++) {
			rtt = 0;
			printf("Before once\n");
			r = probe_once(ctx, ttl, ctx->seq++, &rtt);
			printf("After once\n");
			if (r == 0)
				printf(" *");
			else if (r >= 1)
				printf("  %.3f ms", rtt);
			if (r == 2)
				reached = 1;
			printf("\n");
		}
		if (reached)
			break;
	}
	return (0);
}
