#include "traceroute.h"

#include <arpa/inet.h> // INET_ADDRSTRLEN, inet_ntoa(), inet_ntop()
#include <netinet/ip_icmp.h> // ICMP_X
#include <sys/socket.h> // AF_INET

#include <unistd.h> // getpid()

#include <stdio.h> // printf()
#include <math.h> // sqrt()

void print_header(const t_ping_ctx *ctx)
{
	char ip[INET_ADDRSTRLEN];
	pid_t pid;

	inet_ntop(AF_INET, &ctx->addr.sin_addr, ip, sizeof(ip));
	printf("ping %s (%s): %d data bytes",
	       ctx->hostname, ip, DATA_SIZE);
	if (ctx->flags & FLAG_VERBOSE) {
		pid = getpid();
		printf(", id 0x%04x = %d", (uint16_t)(pid & 0xFFFF),
		       (uint16_t)(pid & 0xFFFF));
	}
	printf("\n");
}

static void print_icmp_error(int type, int code)
{
	switch (type) {
	case ICMP_DEST_UNREACH:
		switch(code) {
		case ICMP_NET_UNREACH:
			printf("Destination Net Unreachable\n");
			break;
		case ICMP_HOST_UNREACH:
			printf("Destination Host Unreachable\n");
			break;
		case ICMP_PROT_UNREACH:
			printf("Destination Protocol Unreachable\n");
			break;
		case ICMP_PORT_UNREACH:
			printf("Destination Port Unreachable\n");
			break;
		case ICMP_FRAG_NEEDED:
			printf("Frag needed\n");
			break;
		case ICMP_SR_FAILED:
			printf("Source Route Failed\n");
			break;
		case ICMP_NET_UNKNOWN:
			printf("Destination Net Unknown\n");
			break;
		case ICMP_HOST_UNKNOWN:
			printf("Destination Host Unknown\n");
			break;
		case ICMP_HOST_ISOLATED:
			printf("Source Host Isolated\n");
			break;
		case ICMP_NET_ANO:
			printf("Destination Net Prohibited\n");
			break;
		case ICMP_HOST_ANO:
			printf("Destination Host Prohibited\n");
			break;
		case ICMP_NET_UNR_TOS:
			printf("Destination Net Unreachable for Type of Service\n");
			break;
		case ICMP_HOST_UNR_TOS:
			printf("Destination Host Unreachable for Type of Service\n");
			break;
		case ICMP_PKT_FILTERED:
			printf("Packet filtered\n");
			break;
		case ICMP_PREC_VIOLATION:
			printf("Precedence Violation\n");
			break;
		case ICMP_PREC_CUTOFF:
			printf("Precedence Cutoff\n");
			break;
		default:
			printf("Dest Unreachable, Bad Code: %d\n", code);
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		printf("Source Quench\n");
		break;
	case ICMP_TIME_EXCEEDED:
		printf("Time to live exceeded\n");
		break;
	case ICMP_REDIRECT:
		switch(code) {
		case ICMP_REDIR_NET:
			printf("Redirect Network\n");
			break;
		case ICMP_REDIR_HOST:
			printf("Redirect Host\n");
			break;
		case ICMP_REDIR_NETTOS:
			printf("Redirect Type of Service and Network\n");
			break;
		case ICMP_REDIR_HOSTTOS:
			printf("Redirect Type of Service and Host\n");
			break;
		default:
			printf("Redirect, Bad Code: %d\n", code);
			break;
		}
		break;
	default:
		printf("type %d, code %d\n", type, code);
		break;
	}
}

void print_response(const t_ping_ctx *ctx, const t_icmp_reply *reply,
                    double rtt)
{
	if (ctx->flags & FLAG_QUIET)
		return ;
	if (ctx->flags & FLAG_FLOOD) {
		printf("\b \b");
		return ;
	}
	printf("%zu bytes from %s: ", reply->len,
	       inet_ntoa(reply->from.sin_addr));
	if (reply->pkt.hdr.type == ICMP_ECHOREPLY)
		printf("icmp_seq=%u ttl=%d time=%.3f ms\n",
		       reply->pkt.hdr.seq, reply->ttl, rtt);
	else {
		print_icmp_error(reply->pkt.hdr.type, reply->pkt.hdr.code);
		if (ctx->flags & FLAG_VERBOSE)
			print_verbose(reply);
	}
}

void print_stats(const t_ping_ctx *ctx)
{
	double loss;
	double avg;
	double stddev;

	loss = 0.0;
	if (ctx->send != 0)
		loss = ((ctx->send - ctx->received) * 100.0) / ctx->send;
	printf("\n--- %s ping statistics ---\n", ctx->hostname);
	printf("%u packets transmitted, ", ctx->send);
	printf("%u packets received, ", ctx->received);
	printf("%.1f%% packet loss\n", loss);
	if (ctx->received == 0)
		return ;
	avg = ctx->rtt_sum / ctx-> received;
	stddev = sqrt(ctx->rtt_sum_sq / ctx->received - avg * avg);
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
	       ctx->rtt_min, avg, ctx->rtt_max, stddev);
}
