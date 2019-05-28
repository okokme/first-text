#include <iostream>
#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "Poller.h"
#include "Nocopyable.h"

class Poller;

int main(int argc, char const *argv[])
{
    Epoll poller;
    EventLoop loop(&poller);
    Channel *ch1 = new Channel(&loop);

    return 0;
}
