#include "debug.h"
#include "ulity.h"
#include "epoll.h"
#include "http_connect.h"

#define MAXEVENTS 5000
/* #include "http_conn.h" */
/* #include "locker.h" */
/* #include "threadpool.h" */

using namespace std;

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

epoll_event* events;

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

    /* Create listen socket */
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
    
    /* setNonBlocking(listenfd); */

    /* Epoll init */ 
    Epoll epoll(MAXEVENTS);
    __uint32_t events_type = EPOLLIN | EPOLLET;
    epoll.add(listenfd,events_type);

    Http_connect* http = new Http_connect[MAXEVENTS];
    events = new epoll_event[MAXEVENTS];
    while(true){
        debug("Epoll start!");
        int number = epoll.wait(MAXEVENTS,-1,events);

        if((number < 0) && (errno != EINTR)){
            log_err("Epoll failure\n") ;
            break;
        }


        for(int i = 0;i<number;i++){
            int sockfd = events[i].data.fd;

            if(sockfd == listenfd){
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);

                if(connfd < 0){
                    log_err("Errno is",errno);
                    continue;
                }

                debug("Accept new connect");
                epoll.add(connfd,events_type);
            }else{
                http[events->data.fd].init(events->data.fd);
                http[events->data.fd].handle_request();
            }
            
            /* else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){ */
            /*     users[sockfd].close_conn(); */
            /* }else if(events[i].events & EPOLLIN){ */
            /*     cout << "客户端发出请求" << endl; */
            /*     if(users[sockfd].read()){ */
            /*         pool->append(users + sockfd); */
            /*     }else{ */
            /*         users[sockfd].close_conn(); */
            /*     } */
            /* }else if(events[i].events & EPOLLOUT){ */
            /*     if(!users[sockfd].write()){ */
            /*         users[sockfd].close_conn(); */
            /*     } */
            /* }else{ */
            /*     ; */
            /* } */
        }
    }
    
    close(listenfd);

    /* close(epollfd); */
    /* delete []users; */
    /* delete pool; */

    return 0;
}


