// anatomy.c — what does a process "own"? Run it, then inspect /proc/<pid>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_initialized = 42;        // lives in .data
int global_uninitialized;           // lives in .bss

int main(void) {
    int on_stack = 7;                       // lives on the stack
    int *on_heap = malloc(sizeof(int));     // lives on the heap
    *on_heap = 99;

    printf("PID  = %d  (my id)\n", getpid());
    printf("PPID = %d  (who started me)\n", getppid());
    printf("UID  = %d\n\n", getuid());
    printf("code  (main)        @ %p\n", (void*)main);
    printf(".data (initialized) @ %p\n", (void*)&global_initialized);
    printf(".bss  (zeroed)      @ %p\n", (void*)&global_uninitialized);
    printf("heap  (malloc)      @ %p\n", (void*)on_heap);
    printf("stack (local var)   @ %p\n", (void*)&on_stack);

    printf("\nSleeping 300s — go inspect /proc/%d\n", getpid());
    fflush(stdout);
    sleep(300);
    return 0;
}
