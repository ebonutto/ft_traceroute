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

#define FLAG_HELP (1 << 0) // -h, --help

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

typedef struct s_traceroute_ctx {
	/* Args */
	const char *progname;
	const char *hostname;
	unsigned long flags;
	int max_hops;

	/* Socket */
	int sockfd;
	struct sockaddr_in addr;

	/* Stats */
	int nprobes;
	int seq;
} t_traceroute_ctx;

/* Global variables */
extern int g_pingloop;

/* Prototypes */
/* parse */
int parse_args(t_traceroute_ctx *ctx, int argc, char **argv);

/* time */
double time_diff_ms(struct timeval *start, struct timeval *end);
void wait_remaining_time(t_traceroute_ctx *ctx, struct timeval *before,
                         struct timeval *after);

/* icmp */
t_icmp_pkt icmp_build(t_traceroute_ctx *ctx, unsigned short seq);
int icmp_send(const t_traceroute_ctx *ctx, const t_icmp_pkt *pkt);
int icmp_recv(t_traceroute_ctx *ctx, t_icmp_reply *reply);

/* signal */
int signal_setup(void);

/* traceroute */

void traceroute_init(t_traceroute_ctx *ctx);
int traceroute_setup(t_traceroute_ctx *ctx);
int traceroute(t_traceroute_ctx *ctx);


/* print */
void print_help(const char *progname);
void print_header(const t_traceroute_ctx *ctx);
void print_response(const t_traceroute_ctx *ctx, const t_icmp_reply *reply,
                    double rtt);
void print_stats(const t_traceroute_ctx *ctx);
void print_verbose(const t_icmp_reply *reply);

#endif
