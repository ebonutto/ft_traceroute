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

static int resolve_hostname(t_traceroute_ctx *ctx)
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

int traceroute_setup(t_traceroute_ctx *ctx)
{
	if (resolve_hostname(ctx) != 0)
		return (1);
	ctx->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (ctx->sockfd == -1) {
		perror("socket()");
		return (1);
	}
	return (0);
}
