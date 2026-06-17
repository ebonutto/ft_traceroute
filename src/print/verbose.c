#include "traceroute.h"

#include <arpa/inet.h> // inet_ntoa()

#include <stdio.h> // printf()

void print_verbose(const t_icmp_reply *reply)
{
	uint16_t *words;
	int i;

	words = (uint16_t *)&reply->inner_ip;
	printf("IP header dump:\n");
	i = 0;
	printf(" ");
	while (i < (int)(sizeof(struct ip) / 2)) {
		printf(" %04x", ntohs(words[i]));
		i++;
	}
	printf("\n");
	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
	printf(" %d  %d  %02x %04x %04x   %x %04x  %02x  %02x %04x %s  ",
	       reply->inner_ip.ip_v,
	       reply->inner_ip.ip_hl,
	       reply->inner_ip.ip_tos,
	       ntohs(reply->inner_ip.ip_len),
	       ntohs(reply->inner_ip.ip_id),
	       (ntohs(reply->inner_ip.ip_off) >> 13) & 0x7,
	       ntohs(reply->inner_ip.ip_off) & 0x1FFF,
	       reply->inner_ip.ip_ttl,
	       reply->inner_ip.ip_p,
	       ntohs(reply->inner_ip.ip_sum),
	       inet_ntoa(reply->inner_ip.ip_src));
	       printf("%s \n", inet_ntoa(reply->inner_ip.ip_dst));
	printf("ICMP: type %d, code %d, size %zu, id 0x%04x, seq 0x%04x\n",
	       reply->inner_icmp.type,
	       reply->inner_icmp.code,
	       reply->len,
	       ntohs(reply->inner_icmp.id),
	       ntohs(reply->inner_icmp.seq));
}