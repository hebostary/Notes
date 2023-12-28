#include <iostream>
#include <unistd.h>

void doit() {
    if (fork() == 0) //child
    {
        fork();
        printf("hello\n");
        return;
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
[12699] [12695] hello
hello
[12700] [1] hello
hello
[12701] [1] hello
*/
