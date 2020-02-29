#ifndef EPOLL_H
#define EPOLL_H
#include "ulity.h"

#define EPOLL_H
class Epoll{
public:
    Epoll(int MAXEVENTS,int LISTENQ = 1024);
    ~Epoll();
public:
    void add(int fd,__uint32_t event);
    int wait(int max_events,int timeout,epoll_event* events);
private:
    epoll_event* m_events ;
    int m_epollfd;

};
#endif
