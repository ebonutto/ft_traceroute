#include "traceroute.h"

int main(int argc, char **argv)
{
	t_ping_ctx ctx;

	ping_init(&ctx);
	if (parse_args(&ctx, argc, argv) != 0)
		return (1);
	if (ctx.flags & FLAG_HELP) {
		print_help(ctx.progname);
		return (0);
	}
	if (ping_run(&ctx) != 0)
		return (2);
	return (0);
}
