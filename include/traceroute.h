#ifndef TRACEROUTE_H
#define TRACEROUTE_H

/* Includes */
#include <netinet/in.h> // struct sockaddr_in
#include <netinet/ip_icmp.h> // struct ip

#include <sys/time.h> // struct timeval

#include <stdint.h> // uintX_t

/* Types */
typedef int (*t_parser)(const char *str, void *out);

/* Macros */
#define DATA_SIZE 56

#define FLAG_HELP (1 << 0) // -?, --help
#define FLAG_VERBOSE (1 << 1) // -v, --verbose
#define FLAG_FLOOD (1 << 2) // -f, --flood
#define FLAG_QUIET (1 << 3) // -q, --quiet

/* Structures */
typedef struct s_icmp_hdr {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t seq;
} t_icmp_hdr;

typedef struct s_icmp_pkt {
	t_icmp_hdr hdr;
	char data[DATA_SIZE];
} t_icmp_pkt;

typedef struct s_icmp_reply {
	/* External packet */
	struct ip ip;
	t_icmp_pkt pkt;
	struct sockaddr_in from;
	int ttl;
	size_t len;
	struct timeval tv_recv;

	/* Inner packet */
	struct ip inner_ip;
	t_icmp_hdr inner_icmp;
} t_icmp_reply;

typedef struct s_ping_ctx {
	/* Args */
	const char *progname;
	const char *hostname;
	unsigned long flags;
	int count; // -c, --count
	double interval; // -i, --interval
	int ttl; // -t, --ttl
	unsigned char pattern; // -p, --pattern

	/* Socket */
	int sockfd;
	struct sockaddr_in addr;

	/* Stats */
	unsigned int send;
	unsigned int received;
	double rtt_min;
	double rtt_max;
	double rtt_sum;
	double rtt_sum_sq;
} t_ping_ctx;

/* Global variables */
extern int g_pingloop;

/* Prototypes */
/* parsing */
int parse_value_unsigned_char(const char *str, void *out);
int parse_value_int(const char *str, void *out);
int parse_value_double(const char *str, void *out);
int parse_value_pattern(const char *str, void *out);
int parse_value(t_ping_ctx *ctx, int argc, char **argv,
                int *i, t_parser parser, void *out);
int parse_args(t_ping_ctx *ctx, int argc, char **argv);

/* time */
double time_diff_ms(struct timeval *start, struct timeval *end);
void wait_remaining_time(t_ping_ctx *ctx, struct timeval *before,
                         struct timeval *after);

/* icmp */
t_icmp_pkt icmp_build(t_ping_ctx *ctx, unsigned short seq);
int icmp_send(const t_ping_ctx *ctx, const t_icmp_pkt *pkt);
int icmp_recv(t_ping_ctx *ctx, t_icmp_reply *reply);

/* signal */
int signal_setup(void);

/* ping */
void ping_init(t_ping_ctx *ctx);
int ping_setup(t_ping_ctx *ctx);
int ping_run(t_ping_ctx *ctx);
void ping_cleanup(t_ping_ctx *ctx);

/* print */
void print_help(const char *progname);
void print_header(const t_ping_ctx *ctx);
void print_response(const t_ping_ctx *ctx, const t_icmp_reply *reply,
                    double rtt);
void print_stats(const t_ping_ctx *ctx);
void print_verbose(const t_icmp_reply *reply);

#endif
