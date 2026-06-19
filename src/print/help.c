#include "traceroute.h"

#include <stdio.h> // printf()

void print_help(const char *progname)
{
	printf("Usage:\n");
	printf("  %s [options] hostname\n", progname);
	printf("\n");
	printf("Options:\n");
	printf("  -h, --help      Give this help list\n");
	printf("\n");
}
