#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include "EventLoop.h"
#include "Timer.h"
#include "Logger.h"
#include <stdarg.h>
#include <cstring>

Logger *_log = nullptr;

Logger::Logger()
{
    _quit = 0;
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init(&_cond, NULL);
    mkdir(".log", 0777);
    // 泄漏了this指针
    pthread_create(&_tid, NULL, flushToFile, this);
}

Logger::~Logger()
{
    _quit = 1;
    pthread_cond_signal(&_cond);
    pthread_join(_tid, NULL);
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_cond);
}

void Logger::writeToBuffer(const char *s, size_t len)
{
    pthread_mutex_lock(&_mutex);
    _writeBuf.append(s, len);
    pthread_mutex_unlock(&_mutex);
}

void Logger::wakeUp(void)
{
    pthread_mutex_lock(&_mutex);
    if (_writeBuf.readable() > 0) {
        _writeBuf.swap(_flushBuf);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond);
    } else
        pthread_mutex_unlock(&_mutex);
}

void *Logger::flushToFile(void *arg)
{
    Logger *_l = static_cast<Logger*>(arg);
    int fd = open("./.log/x.log", O_WRONLY | O_APPEND | O_CREAT, 0777);
    while (1) {
        pthread_mutex_lock(&_l->_mutex);
        while (!_l->_quit && _l->_flushBuf.readable() == 0)
            pthread_cond_wait(&_l->_cond, &_l->_mutex);
        if (_l->_quit) {
            std::cout << "exit" << std::endl;
            write(fd, _l->_flushBuf.peek(), _l->_flushBuf.readable());
            pthread_mutex_unlock(&_l->_mutex);
            pthread_exit(NULL);
        }
        write(fd, _l->_flushBuf.peek(), _l->_flushBuf.readable());
        _l->_flushBuf.retrieveAll();
        pthread_mutex_unlock(&_l->_mutex);
    }
}

void Logger::output(int level, const char *_FILE_, int _LINE_,
        const char *_func_, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    std::string str;
    char buf[1024];
    str += levelstr(level);
    if (level == DEBUG)
        str += ": ";
    else if (level == WARN)
        str += ":  ";
    else if (level == ERROR)
        str += ": ";
    str += Timer::timestr(Timer::now(), buf, sizeof(buf));
    str += " ";
    str += _FILE_;
    str += ": ";
    bzero(buf, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d", _LINE_);
    str += buf;
    str += ": ";
    str += _func_;
    str += ": ";
    bzero(buf, sizeof(buf));
    vsnprintf(buf, sizeof(buf), fmt, ap);
    str += buf;
    str += "\n";
    writeToBuffer(str.c_str(), str.size());
    va_end(ap);
    if (level == ERROR) {
        fprintf(stderr, "logError");
        exit(1);
    }
}
