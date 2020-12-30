/*
 * @Author: sky
 * @Date: 2020-12-26 22:21:01
 * @LastEditors: sky
 * @LastEditTime: 2020-12-30 08:31:59
 * @Description: 
 */
#include "../include/epoll.h"

Epoll::Epoll(int LISTENQ,int listenfd,int MAXEVENTS){
    m_listenfd = listenfd;
    m_epollfd = epoll_create(LISTENQ+1);
    check_exit(m_epollfd != -1,"func:epoll_create error");
    m_events = new epoll_event[MAXEVENTS];
    m_active_events = new epoll_event[MAXEVENTS];
    m_http = new Http_connect[MAXEVENTS];
}

int Epoll::wait(int max_events,int timeout){
    int number;
    while(true){

        debug("Epoll start!");
        int sockfd;
        vector<Http_connect*> m_IO_events;
        number = epoll_wait(m_epollfd,m_active_events,max_events,timeout);
        check_exit(number >= 0,"func: Epollwait error. errorno:%d\n",errno);

        if((number < 0) && (errno != EINTR)){
            log_err("Epoll failure\n") ;
            break;
        }


        for(int i = 0;i<number;i++){
            sockfd = m_active_events[i].data.fd;

            if(sockfd == m_listenfd){
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(m_listenfd,(struct sockaddr*)&client_address,&client_addrlength);

                if(connfd < 0){
                    log_err("Errno is %d",errno);
                    /* exit(0); */
                    continue;
                }
                __uint32_t  events_type = EPOLLIN | EPOLLET;
                debug("Accept new connect");
                add(connfd,events_type);
            }else{
                //Judge the type of I/O,push into vector.
                m_http[sockfd].setFd(sockfd);
                m_IO_events.push_back(&m_http[sockfd]);

                /* m_http[m_active_events->data.fd].init(sockfd); */
                /* m_http[m_active_events->data.fd].handleRequest(); */
            }

        }

        //Traverse the vector,add them to the threadpool.
        if(!m_IO_events.empty()) {
            for(auto &cur_http:m_IO_events){
                debug("Socket:%d is going to be added to threadpool\n",cur_http->getFd());

                Threadpool::addTask(std::bind(&Http_connect::init,cur_http,std::placeholders::_1),cur_http->getFd());
            }
        }
        m_IO_events.clear();
    }

    return number;
}

void Epoll::add(int fd,__uint32_t events){
    /* Make socket nonBlocking */
    setnonBlocking(fd);
    m_events[fd].data.fd = fd;
    m_events[fd].events = events;
    int ret = epoll_ctl(m_epollfd,EPOLL_CTL_ADD,fd,&m_events[fd]);
    check_exit(ret >= 0,"func:epoll_add error");
    debug("%d was added to epoll\n",fd);
}

Epoll::~Epoll(){
    close(m_epollfd);
}
