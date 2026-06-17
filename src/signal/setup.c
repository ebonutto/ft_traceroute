#include "traceroute.h"

#include <signal.h> // struct sigaction, SIGINT, sigaction(), sigemptyset()

#include <stddef.h> // NULL
#include <stdio.h> // perror()

int g_pingloop = 1;

static void signal_handler(int dummy)
{
	(void)dummy;
	g_pingloop = 0;
}

int signal_setup(void)
{
	struct sigaction sa;

	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction()");
		return (1);
	}
	return (0);
}
