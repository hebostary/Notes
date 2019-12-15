#include <iostream>
#include <unistd.h>

void doit() {
    if (fork() == 0) //child
    {
        fork();
        printf("hello\n");
        exit(0);
    }
    return;
}

int main()
{
    doit();
    pid_t p = getpid();
    pid_t pp = getppid();
    printf("[%d] [%d] hello\n", int(p), int(pp));
    exit(0);
}

/*
Output:
[12496] [12491] hello
hello
hello
*/
