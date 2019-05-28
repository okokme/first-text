#pragma once

class EventLoop;

class Poller {
public:
 //   Poller() { }
    virtual ~Poller() { }
    virtual void wait(EventLoop *loop, int timeout) = 0;
    virtual void add( int fd, int events ) = 0; 
    virtual void change( int fd, int events ) = 0; 
    virtual void del( int fd ) = 0; 
};