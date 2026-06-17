#include "traceroute.h"

#include <stdio.h> // printf()

void print_help(const char *progname)
{
	printf("Usage:\n");
	printf("  %s [options] hostname\n", progname);
	printf("\n");
	printf("Options:\n");
	printf("  -?, --help      Give this help list\n");
	printf("  -c, --count     Stop after sending COUNT packets\n");
	printf("  -i, --interval  Wait INTERVAL seconds between sending each packet\n");
	printf("  -t, --ttl       Specify TTL as time-to-live\n");
	printf("  -v, --verbose   Verbose output\n");
	printf("  -f, --flood     Flood ping (root only)\n");
	printf("  -p, --pattern   Fill ICMP packet with given pattern (hex)\n");
	printf("  -q, --quiet     Quiet output\n");
	printf("\n");
}
