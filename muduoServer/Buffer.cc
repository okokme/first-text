#include <assert.h>
#include <sys/uio.h>
#include "Buffer.h"

size_t Buffer::readFd(int fd) {
    char extrabuf[65535];
    struct iovec vec[2];
    const size_t writen = writeable();
    
    vec[0].iov_base = begin() + writen;
    vec[0].iov_len = writen;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    
    int n = readv(fd, iov, 2);
    assert( n >= 0 );
    if( n<= writen )
        _writeindex += n;
    else {
        _writeindex += writen;
        append(extrabuf, n-writen);
    }
    return n;

}
