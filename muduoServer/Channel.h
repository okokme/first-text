
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Nocopyable.h"

class EventLoop;

class Channel : Nocopyable {
public: 
    Channel( EventLoop *loop ):_loop(loop) { }
    ~Channel() { }

    void listen() {
        struct sockaddr_in _sockaddr;
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(_sockfd >=0 );

        _sockaddr.sin_family = AF_INET;
        _sockaddr.sin_port = htons(8888);
        _sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

        int t = bind(_sockfd,(struct sockaddr*)&_sockaddr, sizeof(_sockaddr) );
        assert(t >= 0);
        t = ::listen(_sockfd, 1024);
        assert(t >= 0);
    }

    void handleAccept( void ) {
        
    }
    int fd() { return _sockfd; }
   
private:
    EventLoop *_loop;
    int _sockfd;
    
};