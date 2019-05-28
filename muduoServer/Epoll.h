#pragma once

#include "Poller.h"
#include "Nocopyable.h"
class EventLoop;

class Epoll : public  Poller, Nocopyable {
public:
    Epoll() { }
    ~Epoll() { }
    void add() { }
    void wait() { }
    void del() { }
private:

};