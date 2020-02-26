#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <signal.h>
#include "debug.h"

/* #include "http_conn.h" */
/* #include "locker.h" */
/* #include "threadpool.h" */

using namespace std;

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

extern int addfd(int epollfd,int fd,bool one_shot);
extern int removefd(int epollfd,int fd);

void addsig(int sig,void (handler)(int),bool restart = true){
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = handler;
    if(restart){
        sa.sa_flags |= SA_RESTART;
    }

    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL) != -1);
}

int main(int argc,char *argv[])
{
    
    /* Parse argument,read config. */
    const char* ip = "127.0.0.1";
    int port = 8080;
    log_info("Server starts.");

    /* ignore SIGPIPE */
    addsig(SIGPIPE,SIG_IGN);

    int listenfd = socket(PF_INET,SOCK_STREAM,0);
    check_exit(listenfd >= 0,"func:socket error.");

    #ifndef NDEBUG
    struct linger tmp = {1,0};
    setsockopt(listenfd,SOL_SOCKET,SO_LINGER,&tmp,sizeof(tmp));
    #endif


    /* Init server address. */
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port = htons(port);

    ret = bind(listenfd,(struct sockaddr*)&address,sizeof(address));
    check_exit(ret >= 0,"func:bind error.");
    
    ret = listen(listenfd,5);
    check_exit(ret >= 0,"func:listen error.");

    struct sockaddr_in client_address;
    socklen_t client_address_length= sizeof(client_address);

    debug("%d",listenfd);
    int conn = accept(listenfd,(struct sockaddr*)&client_address,&client_address_length);
    debug("Accept new conect!");

    /* epoll_event events[MAX_EVENT_NUMBER]; */
    /* int epollfd = epoll_create(5); */
    /* assert(epollfd != -1); */
    /* addfd(epollfd,listenfd,false); */
    /* http_conn::m_epollfd = epollfd; */

    /* /1* while(true){ *1/ */
    /*     /1* std::cout << "Epoll start" << std::endl; *1/ */
    /*     int number = epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1); */
    /*     std::cout <<  number << std::endl; */
    /*     if((number < 0) && (errno != EINTR)){ */
    /*         printf("epoll failure\n"); */
    /*         break; */
    /*     } */

/*         for(int i = 0;i<number;i++){ */
/*             int sockfd = events[i].data.fd; */
/*             //std::cout << "sockfd = " << sockfd << std::endl; */

            /* if(sockfd == listenfd){ */
            /*     cout << "新的连接请求" << endl; */
            /*     struct sockaddr_in client_address; */
            /*     socklen_t client_addrlength = sizeof(client_address); */
            /*     int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength); */
            /*     if(connfd < 0){ */
            /*         printf("errno is:%d\n",errno); */
            /*         continue; */
            /*     } */
            /*     if(http_conn::m_user_count >= MAX_FD){ */
            /*         show_error(connfd,"Internal server busy"); */
            /*         continue; */
            /*     } */

                /* /1* 初始化客户连接 *1/ */
                /* users[connfd].init(connfd,client_address); */
            /* }else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){ */
                /* users[sockfd].close_conn(); */
            /* }else if(events[i].events & EPOLLIN){ */
                /* cout << "客户端发出请求" << endl; */
                /* if(users[sockfd].read()){ */
                /*     pool->append(users + sockfd); */
                /* }else{ */
                /*     users[sockfd].close_conn(); */
                /* } */
            /* }else if(events[i].events & EPOLLOUT){ */
                /* if(!users[sockfd].write()){ */
                /*     users[sockfd].close_conn(); */
                /* } */
            /* }else{ */
                /* ; */
            /* } */
        /* } */
    /* } */
    
    close(listenfd);

    /* close(epollfd); */
    /* delete []users; */
    /* delete pool; */

    return 0;
}


