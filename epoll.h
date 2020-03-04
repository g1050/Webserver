#ifndef EPOLL_H
#define EPOLL_H
#include "ulity.h"
#include "http_connect.h"
#include "threadpool.h"

#define EPOLL_H


class Epoll{
public:
    Epoll(int MAXEVENTS,int listenfd,int LISTENQ = 1024);
    ~Epoll();
public:
    void add(int fd,__uint32_t event);
    int wait(int max_events,int timeout);
private:
    epoll_event* m_events ;
    epoll_event* m_active_events;
    int m_epollfd;
    int m_listenfd;
    Http_connect* m_http;

};
#endif
