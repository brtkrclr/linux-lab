FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      gcc make gdb strace ltrace procps psmisc htop man-db less vim-tiny \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /lab
