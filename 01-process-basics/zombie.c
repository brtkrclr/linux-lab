// zombie.c — child exits, parent never calls wait() => zombie (state Z)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    pid_t pid = fork();
    if (pid == 0) {
        printf("[child %d] exiting now\n", getpid());
        exit(0);
    }
    printf("[parent %d] NOT waiting for child %d. Sleeping 60s...\n", getpid(), pid);
    fflush(stdout);
    sleep(60);
    return 0;
}
