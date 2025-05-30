/* 
 * echoservert_pre.c - A prethreaded concurrent echo server
 */
/* $begin echoservertpremain */
#include "csapp.h"
#include "sbuf.h"
#define NTHREADS  4
#define SBUFSIZE  16

void echo_cnt(int connfd);
void *thread(void *vargp);

sbuf_t sbuf; /* Shared buffer of connected descriptors */

int main(int argc, char **argv) 
{
    int i, listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid; 

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    listenfd = Open_listenfd(argv[1]);

    /* 初始化连接缓冲区， 通过信号实现互斥访问来保证线程安全 */
    sbuf_init(&sbuf, SBUFSIZE); //line:conc:pre:initsbuf
    for (i = 0; i < NTHREADS; i++)  /* Create worker threads */ //line:conc:pre:begincreate
	    Pthread_create(&tid, NULL, thread, NULL);               //line:conc:pre:endcreate

    while (1) { 
        clientlen = sizeof(struct sockaddr_storage);
	    connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
	    sbuf_insert(&sbuf, connfd); /* Insert connfd in buffer */
    }
}

void *thread(void *vargp) 
{  
    /* 分离线程，一个分离线程是不能被其它线程回收或者杀死的，它的内存资源在它终止时由系统自动释放 */
    Pthread_detach(pthread_self()); 
    while (1) { 
	int connfd = sbuf_remove(&sbuf); /* Remove connfd from buffer */ //line:conc:pre:removeconnfd
	echo_cnt(connfd);                /* Service client */
	Close(connfd);
    }
}
/* $end echoservertpremain */
