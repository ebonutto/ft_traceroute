#include "traceroute.h"

#include <ctype.h> // isxdigit()
#include <limits.h> // INT_MAX, UCHAR_MAX
#include <stddef.h> // NULL()
#include <stdio.h> // dprintf()
#include <stdlib.h> // strod(), strol()
#include <string.h> // strlen()

int parse_value_unsigned_char(const char *str, void *out)
{
	char *end;
	long val;

	val = strtol(str, &end, 10);
	if (*end != '\0' || val <= 0 || val > UCHAR_MAX)
		return (1);
	*(int *)out = (int)val;
	return (0);
}

int parse_value_int(const char *str, void *out)
{
	char *end;
	long val;

	val = strtol(str, &end, 10);
	if (*end != '\0' || val <= 0 || val > INT_MAX)
		return (1);
	*(int *)out = (int)val;
	return (0);
}

int parse_value_double(const char *str, void *out)
{
	char *end;
	double val;

	val = strtod(str, &end);
	if (*end != '\0' || val <= 0)
		return (1);
	*(double *)out = (double)val;
	return (0);
}

int parse_value_pattern(const char *str, void *out)
{
	if (strlen(str) != 2)
		return (1);
	if (!isxdigit((unsigned char)str[0])
	    || !isxdigit((unsigned char)str[1]))
		return (1);
	*(unsigned char *)out = (unsigned char)strtol(str, NULL, 16);
	return (0);
}

int parse_value(t_ping_ctx *ctx, int argc, char **argv,
                int *i, t_parser parser, void *out)
{
	if (*i + 1 == argc) {
		dprintf(2, "%s: Error: Option '%s' requires a value\n",
		        ctx->progname, argv[*i]);
		return (1);
	}
	if (parser(argv[*i + 1], out)) {
		dprintf(2, "%s: Error: Option %s: Value '%s' is invalid\n",
		        ctx->progname, argv[*i], argv[*i + 1]);
		return (1);
	}
	(*i)++;
	return (0);
}
