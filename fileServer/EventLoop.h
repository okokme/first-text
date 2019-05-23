#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
class EventLoop : Nocopyable { //EventLoop只有一个设为不可拷贝
public:
    explicit EventLoop(Poller *poller ):_poller(poller), _quit(0), _loop(0) {  }
    ~EventLoop() {  }
    //向loop中添加一个新Channel
    

private:
    Poller *_poller;
    std::map<int, std::shared_ptr<Channel>> _channelMap;
    std::vector<std::shared_ptr<Channel>> _activeChannels;
    Timer _timer;
    int _quit;
    int _loop;
};
#endif //_EVENTLOOP_H

