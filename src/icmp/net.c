#include "traceroute.h"

#include <arpa/inet.h> // ntohs()
#include <netinet/ip_icmp.h> // struct ip, ICMP_ECHOREPLY, IP_MAXPACKET
#include <sys/socket.h> // struct sockaddr, recvfrom(), sendto()

#include <sys/time.h> // gettimeofday()
#include <sys/types.h> // ssize_t
#include <unistd.h> // getpid()

#include <errno.h> // EAGAIN, EINTR, EWOULDBLOCK, errno
#include <stddef.h> // size_t, NULL
#include <stdint.h> // uintX_t
#include <stdio.h> // perror()
#include <string.h>

int icmp_send(const t_traceroute_ctx *ctx, const t_icmp_pkt *pkt)
{
	ssize_t ret;

	ret = sendto(ctx->sockfd, pkt, sizeof(*pkt), 0,
	             (struct sockaddr *)&ctx->addr, sizeof(ctx->addr));
	if (ret == -1 || (size_t)ret != sizeof(*pkt)) {
		perror("sendto()");
		return (1);
	}
	return (0);
}

static int icmp_parse_reply(t_icmp_reply *reply, const char *buf, size_t len)
{
	struct ip	*ip;
	t_icmp_hdr	*icmp;
	size_t		ip_hlen;
	const char	*inner;
	size_t		inner_icmp_off;
	const char	*inner_payload;

	if (len < sizeof(struct ip) + sizeof(t_icmp_hdr))
		return (1);
	ip = (struct ip *)buf;
	ip_hlen = ip->ip_hl * 4;
	if (ip_hlen + sizeof(t_icmp_hdr) > len)
		return (1);
	icmp = (t_icmp_hdr *)(buf + ip_hlen);
	reply->ip = *ip;
	reply->ttl = ip->ip_ttl;
	reply->len = len - ip_hlen;

	if (icmp->type == ICMP_ECHOREPLY) {
		if (ntohs(icmp->id) != (uint16_t)(getpid() & 0xFFFF))
			return (1);
		reply->pkt = *(t_icmp_pkt *)(buf + ip_hlen);
		reply->pkt.hdr.seq = ntohs(icmp->seq);
		reply->pkt.hdr.id = ntohs(icmp->id);
		return (0);
	}

	/* TIME_EXCEEDED ou autre : extraire le paquet inner */
	reply->pkt.hdr = *icmp;

	inner = buf + ip_hlen + sizeof(t_icmp_hdr);
	if ((size_t)(inner - buf) + sizeof(struct ip) > len)
		return (1);
	reply->inner_ip = *(struct ip *)inner;

	inner_icmp_off = (size_t)(inner - buf) + reply->inner_ip.ip_hl * 4;
	if (inner_icmp_off + sizeof(t_icmp_hdr) > len)
		return (1);
	reply->inner_icmp = *(t_icmp_hdr *)(buf + inner_icmp_off);

	if (ntohs(reply->inner_icmp.id) != (uint16_t)(getpid() & 0xFFFF))
		return (1);

	/* Copier le timestamp depuis le payload du paquet original */
	inner_payload = buf + inner_icmp_off + sizeof(t_icmp_hdr);
	if ((size_t)(inner_payload - buf) + sizeof(struct timeval) > len)
		return (1);
	memcpy(reply->pkt.data, inner_payload, sizeof(struct timeval));

	return (0);
}

int icmp_recv(t_traceroute_ctx *ctx, t_icmp_reply *reply)
{
	char		buf[IP_MAXPACKET];
	socklen_t	fromlen;
	ssize_t		r;

	while (1) {
		fromlen = sizeof(reply->from);
		r = recvfrom(ctx->sockfd, buf, sizeof(buf), 0,
		             (struct sockaddr *)&reply->from, &fromlen);
		gettimeofday(&reply->tv_recv, NULL);
		if (r == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
				return (1);
			perror("recvfrom()");
			return (1);
		}
		if (icmp_parse_reply(reply, buf, (size_t)r) == 0)
			break;
	}
	return (0);
}
