#include "traceroute.h"

#include <netdb.h> // struct addrinfo, gai_strerror(), getaddrinfo(), freeaddrinfo()
#include <netinet/in.h> // struct sockaddr_in
#include <netinet/ip.h> // IPPROTO_IP, IP_TTL
#include <sys/socket.h> // AF_INET, IPPROTO_ICMP, SOCK_RAW, SOL_SOCKET, SO_RCVTIMEO, setsockopt(), socket()

#include <sys/time.h> // struct timeval, suseconds_t
#include <unistd.h> // close()

#include <stddef.h> // NULL
#include <stdio.h> // dprintf(), perror()
#include <string.h> // memset()
#include <time.h> // time_t

static int resolve_hostname(t_ping_ctx *ctx)
{
	struct addrinfo hints;
	struct addrinfo *res;
	int ret;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	ret = getaddrinfo(ctx->hostname, NULL, &hints, &res);
	if (ret != 0) {
		dprintf(2, "%s: %s: %s\n", ctx->progname, ctx->hostname,
		        gai_strerror(ret));
		return (1);
	}
	ctx->addr = *(struct sockaddr_in *)res->ai_addr;
	freeaddrinfo(res);
	return (0);
}

static int configure_socket(t_ping_ctx *ctx)
{
	struct timeval timeout;

	if (setsockopt(ctx->sockfd, IPPROTO_IP, IP_TTL,
	               &ctx->ttl, sizeof(ctx->ttl)) == -1) {
		perror("setsockopt() TTL");
		return (1);
	}
	timeout.tv_sec  = (time_t)ctx->interval;
	timeout.tv_usec = (suseconds_t)((ctx->interval - timeout.tv_sec)
	                                * 1000000.0);
	if (setsockopt(ctx->sockfd, SOL_SOCKET, SO_RCVTIMEO,
	               &timeout, sizeof(timeout)) == -1) {
		perror("setsockopt() RCVTIMEO");
		return (1);
	}
	return (0);
}

int ping_setup(t_ping_ctx *ctx)
{
	if (resolve_hostname(ctx) != 0)
		return (1);
	ctx->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (ctx->sockfd == -1) {
		perror("socket()");
		return (1);
	}
	if (configure_socket(ctx) != 0) {
		close(ctx->sockfd);
		return (1);
	}
	return (0);
}
