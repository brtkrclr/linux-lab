// exec.c — fork + exec + wait: exactly what your shell does for every command
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid == 0) {
        printf("[child %d] about to become 'ls'...\n", getpid());
        fflush(stdout);
        execlp("ls", "ls", "-l", "/lab/01-process-basics", NULL);
        perror("exec failed");   // only reached if exec FAILS
        return 127;
    }
    int status;
    waitpid(pid, &status, 0);
    printf("[parent %d] child finished, status=%d\n", getpid(), WEXITSTATUS(status));
    return 0;
}
