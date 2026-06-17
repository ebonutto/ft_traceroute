# ping

## What is ping?

`ping` is a network diagnostic tool that measures the reachability of a host and the round-trip time (RTT) of packets sent to it.

It works by sending **ICMP Echo Request** packets to a target host and waiting for **ICMP Echo Reply** responses.

---

## How it works

### 1. Raw socket

`ping` opens a raw socket (`SOCK_RAW`) with the `IPPROTO_ICMP` protocol. This gives direct access to the IP layer — the entire IP header is included in every packet received.

```
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
```

Root privileges (or `CAP_NET_RAW`) are required to open a raw socket.

### 2. ICMP packet structure

Each packet sent is an **ICMP Echo Request** (type 8):

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|     Type      |     Code      |           Checksum            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           Identifier          |        Sequence Number        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            Payload                            |
|              (timestamp + padding up to data size)            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

- **Type** — 8 for Echo Request, 0 for Echo Reply
- **Identifier** — set to the process PID (to distinguish our packets from others)
- **Sequence** — incremented for each packet sent
- **Payload** — contains a `struct timeval` timestamp used to compute RTT

### 3. RTT measurement

The current time is embedded in the packet payload before sending. When the reply arrives, the timestamp is extracted and compared to the current time:

```
RTT = time_of_receipt - timestamp_in_payload
```

### 4. Receiving packets

Since the socket is raw, every ICMP packet destined for the machine is received — not just ours. Each incoming packet must be filtered by:

- **Type** — must be `ICMP_ECHOREPLY` (0) for normal replies
- **Identifier** — must match our PID

On loopback, both the Echo Request and Echo Reply are visible, so the type filter is critical.

### 5. Timing

`ping` sends one packet per second by default. The interval is managed precisely:

1. Record time before sending
2. Send packet and wait for reply
3. Record time after receiving
4. Sleep for `interval - elapsed` using `usleep`

This ensures packets are sent at a consistent rate regardless of RTT.

### 6. Error messages (ICMP errors)

When a packet cannot reach its destination, an intermediate router sends back an ICMP error. These contain the original packet embedded inside:

```
[ IP header | ICMP error header | inner IP header | inner ICMP header ]
```

Common ICMP error types:

| Type | Meaning |
|------|---------|
| 3    | Destination Unreachable |
| 11   | Time to Live Exceeded |
| 5    | Redirect |

The inner packet is used to verify that the error concerns our packet (via the identifier).

### 7. Statistics

On exit (SIGINT or after `-c` packets), ping displays:

```
--- google.com ping statistics ---
5 packets transmitted, 5 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 7.247/11.428/14.841/2.944 ms
```

- **stddev** is computed as: `sqrt(avg(rtt²) - avg(rtt)²)`

---

## Usage

```
Usage:
  ./ft_ping [options] hostname

Options:
  -?, --help      Give this help list
  -c, --count     Stop after sending COUNT packets
  -i, --interval  Wait INTERVAL seconds between sending each packet
  -t, --ttl       Specify TTL as time-to-live
  -v, --verbose   Verbose output
  -f, --flood     Flood ping (root only)
  -p, --pattern   Fill ICMP packet with given pattern (hex)
  -q, --quiet     Quiet output
```

---

## Building

```bash
make
```

Requires root or `CAP_NET_RAW`:

```bash
sudo ./ft_ping google.com
```
