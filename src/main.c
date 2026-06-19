#include "traceroute.h"

int main(int argc, char **argv)
{
	t_traceroute_ctx ctx;

	traceroute_init(&ctx);
	if (parse_args(&ctx, argc, argv) != 0)
		return (1);
	if (ctx.flags & FLAG_HELP) {
		print_help(ctx.progname);
		return (0);
	}
	if (traceroute(&ctx) != 0)
		return (1);
	return (0);
}
