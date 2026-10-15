// fork.c — one process becomes two. Memory is COPIED, not shared.
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int counter = 100;
    printf("[before fork] I am PID %d\n", getpid());

    pid_t pid = fork();          // <-- returns TWICE

    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {              // child sees 0
        counter += 1;
        printf("[child ] pid=%d ppid=%d fork()=%d counter=%d &counter=%p\n",
               getpid(), getppid(), pid, counter, (void*)&counter);
        return 7;                // exit status the parent will collect
    }

    // parent sees the child's PID
    counter -= 1;
    printf("[parent] pid=%d             fork()=%d counter=%d &counter=%p\n",
           getpid(), pid, counter, (void*)&counter);

    int status;
    waitpid(pid, &status, 0);    // reap the child
    printf("[parent] child %d exited with status %d\n", pid, WEXITSTATUS(status));
    return 0;
}
