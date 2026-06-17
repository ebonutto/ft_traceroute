FROM debian:bullseye-slim AS dev

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gdb valgrind strace \
    inetutils-traceroute \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

ENTRYPOINT ["bash"]
