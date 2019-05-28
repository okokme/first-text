#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
//
//+------------------+-------------------+
//|  readable bytes  |   writable bytes  |
//|                  |                   |
//+------------------+-------------------+
//|                  |                   |
//0 <=  readIndex  <= writeindex  <=   size
////
class Buffer
{
public:
    Buffer():_buf(INIT_SIZE) {}
    ~Buffer() {}
    static const size_t INIT_SIZE = 1024;
    static const char _crlf[]; //??

    size_t readableBytes() {
        return _writeindex - _readindex; 
    }

    const char*peek() {//返回指针 
        return (begin() + _readindex); //通过begin转换类型
    }

    void retrieve(size_t len ) {
       // readindex
       assert(len <= readableBytes());//这里意思是只有两种可能，比缓冲区小或等于 小的话减，等的化，恢复最开始长度
       if(len < readableBytes())
       {
           _readindex +=len;
       }
       else 
       {
    //       readabeAll();
             _readindex = _writeindex = 0;
       }
    }

 //   string retrieveAsString();
    void append(const char* data, size_t len) { //将data追加到buffer中
        if( writeable() < len )
        {
            makeSpace(len);
        }
        assert(writeable() >= len);
        _buf.insert(_buf.begin() + _writeindex, data, data+len);
        _writeindex += len;
    }

 //   prepend(const void*, int);
    size_t prependable() { return _readindex; }

    size_t readable() { return _writeindex - _readindex; }

    size_t writeable() { return _buf.capacity - _writeindex; }

    void swap(Buffer &_buffer) {  //不知道有什么用
        _buf.swap( _buffer._buf );//??
        std::swap( _readindex, _buffer._readindex );
        std::swap( _writeindex, _buffer._writeindex );
    }
    
    size_t readFd(int fd);
   //
    private:
    char* begin() { return &*_buf.begin(); }
    const char* begin() const { return &*_buf.begin(); }
    void makeSpace(size_t len) { //内存腾挪
        //有足够的腾挪空间
        if(len < writeable() && writeable() + prependable() > len ) {
            size_t readn = readable();
            std::copy(peek(), peek()+readn, begin());
            _readindex = 0;
            _writeindex = _readindex + readn;
        }    

    }
    std::vector<char> _buf;
    size_t _readindex = 0;
    size_t _writeindex = 0;
};
