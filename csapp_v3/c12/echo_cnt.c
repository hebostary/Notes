/* 
 * A thread-safe version of echo that counts the total number
 * of bytes received from clients.
 */
/* $begin echo_cnt */
#include "csapp.h"

static int byte_cnt;  /* Byte counter， 所有线程都需要更新， 为了避免竞争条件，必须加锁来做访问控制 */
static sem_t mutex;   /* and the mutex that protects it */

static void init_echo_cnt(void)
{
    Sem_init(&mutex, 0, 1);
    byte_cnt = 0;
}

void echo_cnt(int connfd) 
{
    int n; 
    char buf[MAXLINE]; 
    rio_t rio;
    static pthread_once_t once = PTHREAD_ONCE_INIT;

    /* 当第一次有某个线程调用echo_cnt的时候，使用下面的pthread_once函数去调用初始化函数，
    缺点：每次调用echo_cnt都会调用pthread_once，尽管它没做什么有用的事情
    除了这种方法，我们也可以在主线程中显示地调用初始化函数 */
    Pthread_once(&once, init_echo_cnt); //line:conc:pre:pthreadonce
    Rio_readinitb(&rio, connfd);        //line:conc:pre:rioinitb
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
	    P(&mutex);
	    byte_cnt += n; //line:conc:pre:cntaccess1
	    printf("thread %d received %d (%d total) bytes on fd %d\n", 
	       (int) pthread_self(), n, byte_cnt, connfd); //line:conc:pre:cntaccess2
	    V(&mutex);
	    Rio_writen(connfd, buf, n);
    }
}
/* $end echo_cnt */

