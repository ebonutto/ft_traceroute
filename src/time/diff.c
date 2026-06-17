#include "traceroute.h"

#include <sys/time.h> // struct timeval

double time_diff_ms(struct timeval *start, struct timeval *end)
{
	struct timeval tmp;

	tmp.tv_sec = end->tv_sec - start->tv_sec;
	tmp.tv_usec = end->tv_usec - start->tv_usec;
	if (tmp.tv_usec < 0) {
		tmp.tv_sec -= 1;
		tmp.tv_usec += 1000000;
	}
	return (tmp.tv_sec * 1000.0 + tmp.tv_usec / 1000.0);
}
