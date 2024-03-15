#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include "io.hpp"

#define SERVER_PORT 12345
 
//最多处理的connect
#define MAX_EVENTS 500
 
//当前的连接数
int currentClient = 0; 
 
#define REVLEN 20
 
int epfd;

struct epoll_event eventList[MAX_EVENTS];
 
void AcceptConn(int srvfd);
void RecvData(int fd);
 
int epoll_server()
{
    printf("\n[INFO] epoll server is running...\n");
    int sockListen;
    int timeout;
    struct sockaddr_in server_addr;
    
    //socket
    if((sockListen=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error\n");
        return -1;
    }
    
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family  =  AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr  =  htonl(INADDR_ANY); 
    
    //bind
    if(bind(sockListen, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("bind error\n");
        return -1;
    }
    
    //listen
    if(listen(sockListen, 5) < 0)
    {
        printf("listen error\n");
        return -1;
    }
    
    // epoll 初始化
    epfd = epoll_create(MAX_EVENTS);
    struct epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = sockListen;
    
    //add Event
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, sockListen, &event) < 0)
    {
        printf("epoll add fail : fd = %d\n", sockListen);
        return -1;
    }
    
    //epoll
    while(1)
    {
        timeout=30000;                
        //epoll_wait
        int ret = epoll_wait(epfd, eventList, MAX_EVENTS, timeout);
        
        if(ret < 0)
        {
            printf("epoll error\n");
            break;
        }
        else if(ret == 0)
        {
            printf("not receive any event before %d milliseconds timeout ...\n", timeout);
            continue;
        }
        
        //直接获取了事件数量,给出了活动的流,这里是和poll区别的关键
        for(int i=0; i<ret; i++)
        {
            //错误退出
            if ((eventList[i].events & EPOLLERR) ||
                (eventList[i].events & EPOLLHUP) ||
                !(eventList[i].events & EPOLLIN))
            {
              printf ( "epoll error\n");
              close (eventList[i].data.fd);
              return -1;
            }
            
            if (eventList[i].data.fd == sockListen)
            {
                // 有新的连接请求进来
                AcceptConn(sockListen);
        
            }else{
                // 现有连接里有新的数据，可以创建新的线程去处理I/O
                RecvData(eventList[i].data.fd);
            }
        }
    }
    
    close(epfd);
    close(sockListen);
    
 
    return 0;
}


void AcceptConn(int srvfd)
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(struct sockaddr_in);
    bzero(&sin, len);
 
    int confd = accept(srvfd, (struct sockaddr*)&sin, &len);
 
    if (confd < 0)
    {
       printf("bad accept\n");
       return;
    }else
    {
        printf("Accept Connection: %d\n", confd);
    }
    //将新建立的连接添加到EPOLL的监听中
    struct epoll_event event;
    event.data.fd = confd;
    event.events =  EPOLLIN|EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, confd, &event);
}
 
//读取数据
void RecvData(int fd)
{
    printf("Enter RecvData function for client %d\n", fd);
    int ret;
    int recvLen = 0;
    char recvBuf[REVLEN];
    
    memset(recvBuf, 0, REVLEN);
    
    if(recvLen != REVLEN)
    {
        //循环读取数据直到buffer读满，或者读不到任何数据
        while(1)
        {
            // Read N bytes into BUF from socket FD.
            ret = recv(fd, (char *)recvBuf+recvLen, REVLEN-recvLen, MSG_DONTWAIT);
            if(ret == 0)
            {
                // receive no data
                recvLen = 0;
                break;
            }
            else if(ret < 0)
            {
                // receive data error
                recvLen = 0;
                break;
            }
            // 读取数据正常
            recvLen = recvLen+ret;
            if(recvLen<REVLEN)
            {
                continue;
            }
            else
            {
                //buffer读满了
                printf("Buffered data = %s\n",  recvBuf);
                recvLen = 0;
                break;
            }
        }
    }
 
    printf("Received data from client %d is: %s\n", fd, recvBuf);
}