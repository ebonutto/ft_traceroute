#include "traceroute.h"

#include <arpa/inet.h> // htons()
#include <netinet/ip_icmp.h> // ICMP_ECHO

#include <sys/time.h> // struct timeval, gettimeofday()
#include <unistd.h> // getpid()

#include <stddef.h> // NULL, size_t
#include <string.h> // memcpy(), memset()

static void icmp_init_hdr(t_icmp_hdr *hdr, unsigned short seq)
{
	hdr->type = ICMP_ECHO;
	hdr->code = 0;
	hdr->id = htons((unsigned short)getpid());
	hdr->seq = htons(seq);
}

static void icmp_fill_data(t_ping_ctx *ctx, char *data)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	memcpy(data, &tv, sizeof(tv));
	memset(data + sizeof(tv), ctx->pattern, DATA_SIZE - sizeof(tv));
}

static unsigned short icmp_checksum(void *data, size_t len)
{
	unsigned short *buf;
	unsigned int sum;

	buf = data;
	sum = 0;
	while (len > 1) {
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (~sum);
}

t_icmp_pkt icmp_build(t_ping_ctx *ctx, unsigned short seq)
{
	t_icmp_pkt pkt;

	memset(&pkt, 0, sizeof(pkt));
	icmp_init_hdr(&pkt.hdr, seq);
	icmp_fill_data(ctx, pkt.data);
	pkt.hdr.checksum = icmp_checksum(&pkt, sizeof(pkt));
	return (pkt);
}
