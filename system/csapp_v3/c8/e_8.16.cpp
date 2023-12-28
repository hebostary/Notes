#include <iostream>
#include <unistd.h>

int counter = 1;

int main()
{
    if (fork() == 0)
    {
        counter--;
        exit(0);
    } else
    {
        wait(NULL);
        printf("counter: %d\n", counter);    
    }
    exit(0);
}

/*
Output:
counter: 1

父进程和子进程的代码段和数据（包括全局变量）是独立的。互相不影响
*/
