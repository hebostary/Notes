#include <iostream>
#include <unistd.h>

int main()
{
    int i;

    printf("begin run\n");
    for ( i = 0; i < 2; i++)
    {
        printf("begin to fork child[%d]\n", i);
        fork();
    }
    
    pid_t p = getpid();
    pid_t pp = getppid();
    printf("[%d] [%d] hello\n", int(p), int(pp));
    exit(0);
}

/*
Output:
begin run
begin to fork child[0]
begin to fork child[1]
begin to fork child[1]
[11891] [11887] hello
[11893] [1] hello
[11892] [1] hello
[11894] [11892] hello

总共存在4个进程，其父进程可能更早退出，所以ppid变为1号进程systemd
fork结束后，父进程和子进程都从for循环条件开始运行
*/
