#pragma once 
#include <cassert>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include "Channel.h"
//#include "Poller.h"
#include "Nocopyable.h"

class EventLoop : Nocopyable { //EventLoop只有一个设为不可拷贝
public:
    EventLoop(Epoll *poller ):_poller(poller), _loop(0) {  }
    ~EventLoop() {  }
     //向loop中添加一个新Channel 
    void addChannel(Channel *ch1);
    void fillActiveChannel(   );
    void run(); 
  
    

private:
    Epoll *_poller;
    std::map<int ,std::vector<Channel>> _channelMap;
    std::vector<Channel> _activeChannels;

    int _loop;
};


