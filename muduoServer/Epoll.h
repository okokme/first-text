#pragma once

#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <assert.h>
#include "Poller.h"
#include "Nocopyable.h"
class EventLoop;

//class Epoll : public  Poller, Nocopyable {
class Epoll : public Nocopyable {    
public:
    Epoll() { //单线程内核监听数目为1 
        _epfd = epoll_create(1);  
        assert(_epfd >=0 );
    }
    ~Epoll() { close(_epfd); }
    void add(int fd, int events) {
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = events;
        epoll_ctl( _epfd, EPOLL_CTL_ADD, fd, &ev );
     }
    void wait() { }
    void del() { }
private:
    int _epfd;
    std::vector<struct epoll_event> events;
};