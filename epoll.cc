#include "epoll.h"

Epoll::Epoll(int LISTENQ ,int MAXEVENTS){

   m_epollfd = epoll_create(LISTENQ+1);
   check_exit(m_epollfd != -1,"func:epoll_create error");
   m_events = new epoll_event[MAXEVENTS];

}

int Epoll::wait(int max_events,int timeout,epoll_event* events){
    int ret = epoll_wait(m_epollfd,events,max_events,timeout);
    check_exit(ret >= 0,"func: Epollwait error. errorno:%d\n",errno);
    return ret;
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
