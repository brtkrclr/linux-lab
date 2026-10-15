# Linux Processes & Threads — Hands-on Lab

Learn how Linux really runs programs by writing small C programs and looking at them with `/proc`, `ps`, `strace` and `gdb`.
Everything runs inside a disposable Ubuntu container, so it works the same on macOS, Windows and Linux and can't damage your machine's own setup.

```
linux-lab/
├── docker-compose.yml      # defines the lab container ("proclab")
├── Dockerfile              # Ubuntu 24.04 + gcc, make, gdb, strace, ltrace, procps, htop
├── inside-hello.html       # interactive visualizer: what the kernel does for ./hello and ./hello | wc
└── 01-process-basics/      # Module 1 programs and exercises
```

## 1. Install Docker

You need Docker with the Compose plugin (`docker compose`, with a space).

| Your OS | What to install |
|---|---|
| macOS | [Docker Desktop for Mac](https://docs.docker.com/desktop/setup/install/mac-install/) (choose Apple Silicon or Intel) |
| Windows | [Docker Desktop for Windows](https://docs.docker.com/desktop/setup/install/windows-install/) with the WSL 2 backend |
| Linux | [Docker Engine](https://docs.docker.com/engine/install/) and the [Compose plugin](https://docs.docker.com/compose/install/linux/) |

On macOS and Windows, start Docker Desktop and wait until it says it is running. Then check from a terminal:

```bash
docker --version
docker compose version
```

Both should print a version. If you see `Cannot connect to the Docker daemon`, Docker Desktop isn't running yet.

## 2. Start the lab

```bash
git clone <this-repo-url> linux-lab
cd linux-lab
docker compose up -d --build
```

The first run downloads Ubuntu and installs the tools, which takes a minute or two. Later starts take a second.

Open a shell inside the container:

```bash
docker compose exec proclab bash
```

Your prompt changes to `root@proclab:/lab#`. You are now in Linux. `/lab` inside the container **is** the `linux-lab` folder on your computer, so you can edit files with your normal editor and compile and run them in the container.

Build and run your first program:

```bash
cd 01-process-basics
make
./hello
```

Type `exit` (or press Ctrl-D) to leave the container shell. The container keeps running in the background.

### Everyday commands

Run these on your computer, from the `linux-lab` folder:

| Task | Command |
|---|---|
| Start the lab | `docker compose up -d` |
| Open a shell in it | `docker compose exec proclab bash` |
| Open a second shell (e.g. to watch `ps` while a program runs) | the same command in another terminal window |
| Stop it (keeps the container) | `docker compose stop` |
| Remove the container | `docker compose down` |
| Rebuild after changing the Dockerfile | `docker compose up -d --build` |

Your files live on your computer, not in the container, so `docker compose down` never deletes your work. Anything you install with `apt` inside the container is lost when the container is removed; add it to the `Dockerfile` instead.

### Why the container is set up this way

- **`privileged: true`** lets `strace` and `gdb` trace processes and lets later modules create namespaces and cgroups. It also gives the container broad access to the Docker VM, so only run this lab on your own machine.
- **`init: false`** with `sleep infinity` as PID 1 is deliberate. `sleep` never reaps zombie processes, which Module 1 uses to show what a zombie is. Exercise 5 changes this.
- **`./:/lab` volume** shares this folder with the container.

## 3. The visualizer

Open `inside-hello.html` in any web browser (double-click it). It doesn't need Docker or a server.

It shows a terminal on the left. Type `./hello`, `./hello | wc` or `./hello > out.txt`, then step through what happens: fork/clone, copy-on-write, execve, the dynamic loader, page faults, pipes, exit and wait. You see the process table, each process's memory map, the file descriptor tables and an `strace -f` style log.

The traces are representative examples for Linux x86-64 with glibc and bash, not a recording of your machine. PIDs, addresses and the exact syscalls vary by kernel, libc and CPU architecture. Compare with a real trace in the container (`%process` = fork/exec/wait/exit calls, `%desc` = calls on file descriptors):

```bash
cd /lab/01-process-basics
strace -f -e trace=%process,%desc bash -c './hello | wc'
```

## Roadmap

1. **Process basics** — anatomy, `/proc`, fork/exec/wait, zombies and orphans
2. **Process lifecycle & states** — R/S/D/T/Z, scheduling, nice, context switches
3. **Signals** — SIGTERM vs SIGKILL, handlers, PID 1 signal semantics (e.g. graceful shutdown in Kubernetes)
4. **File descriptors & IPC** — fd inheritance, pipes, dup2, shared memory, sockets
5. **Threads** — pthreads, shared memory, clone() flags, `/proc/<pid>/task`
6. **Synchronization** — race conditions, mutexes, condition variables, deadlocks, atomics
7. **Memory** — virtual memory, copy-on-write, page faults, RSS vs VSZ, the OOM killer
8. **Scheduler internals** — CFS/EEVDF, CPU affinity, priorities, real-time
9. **Namespaces & cgroups** — why a container is "just a process"
10. **Debugging toolkit** — strace, gdb, perf, `/proc` forensics, live troubleshooting

## Module 1: Process basics

All commands below run inside the container, in `/lab/01-process-basics`. Build everything with `make`; `make clean` removes the binaries.

| Program | What it shows | How to explore it |
|---|---|---|
| `hello.c` | The smallest possible process | `./hello`, then `strace ./hello` to see every syscall |
| `anatomy.c` | What a process owns: PID, parent, and where code, data, heap and stack live in memory | Run `./anatomy &`, then `cat /proc/<pid>/maps` and `ls -l /proc/<pid>/fd` using the PID it prints |
| `fork.c` | One process becomes two; each gets its own copy of memory | `./fork` and compare the two `counter` values and addresses |
| `exec.c` | fork + exec + wait, which is what your shell does for every command | `./exec` |
| `zombie.c` | A child that exits while its parent never calls `wait()` | Run `./zombie &`, then `ps -o pid,ppid,stat,cmd` and look for state `Z` |

### Exercises

1. In the output of `anatomy`, find which line of `/proc/<pid>/maps` contains each printed address.
2. `fork.c` prints the `[before fork]` line twice when you run `./fork | cat`. Fix that **without** removing the `printf`.
3. Run `./fork | cat` and `./fork` and explain why the output differs. (Hint: stdio buffering. The visualizer's `./hello | wc` run shows the same effect.)
4. Write `orphan.c`: the parent exits first, and the child prints `getppid()` before and after. Who adopts it?
5. Make the container reap zombies. In `docker-compose.yml` change `init: false` to `init: true`, then on your computer run:
   ```bash
   docker compose up -d --force-recreate
   docker compose exec proclab bash
   ```
   Repeat the zombie experiment. What is PID 1 now (`ps -p 1 -o pid,cmd`), and what happens to the zombie after `./zombie` exits? Set it back to `false` afterwards.

## Troubleshooting

| Problem | Fix |
|---|---|
| `Cannot connect to the Docker daemon` | Start Docker Desktop (macOS: `open -a Docker`) and wait for it to finish starting. |
| `Conflict. The container name "/proclab" is already in use` | An older container with that name exists. Remove it with `docker rm -f proclab` (your files are not affected), then `docker compose up -d`. |
| `service "proclab" is not running` | Start it first: `docker compose up -d`. |
| `exec format error` or `cannot execute binary file` | The binaries were built on a different CPU architecture. Run `make clean && make` inside the container. |
| `strace: ptrace(PTRACE_TRACEME, ...): Operation not permitted` | The container is not privileged. Check `privileged: true` in `docker-compose.yml` and recreate it with `docker compose up -d --force-recreate`. |
| On Apple Silicon, addresses and syscall names differ from the visualizer | The container runs arm64 Linux, which uses different syscall numbers and a different loader path. The concepts are the same. |
