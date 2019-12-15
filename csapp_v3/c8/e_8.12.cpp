#include <iostream>
#include <unistd.h>

void doit() {
    fork();
    fork();
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
[7402] [7398] hello
[7404] [1] hello 
[7403] [1] hello
[7405] [7403] hello

总共存在4个进程，其父进程可能更早退出，所以ppid变为1号进程systemd
*/
