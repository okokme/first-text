#ifndef _LOGGER_H
#define _LOGGER_H

#include <pthread.h>
#include "Buffer.h"

class EventLoop;

class Logger {
public:
    Logger();
    ~Logger();
    enum level {
        DEBUG   = 001,
        WARN    = 002,
        ERROR   = 004,
    };
    void wakeUp();
    void output(int level, const char *_FILE_, int _LINE_, 
            const char *_func_, const char *fmt, ...);
    const char *levelstr(int level)
    {
        if (level == DEBUG)
            return "DEBUG";
        else if (level == WARN)
            return "WARN";
        else if (level == ERROR)
            return "ERROR";
        else
            return "NONE";
    }
private:
    void writeToBuffer(const char *s, size_t len);
    static void *flushToFile(void *arg);
    Buffer _writeBuf;
    Buffer _flushBuf;
    pthread_t _tid;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    int _quit; 
};

extern Logger *_log;

#define logDebug(...) \
    _log->output(Logger::DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logWarn(...) \
    _log->output(Logger::WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logError(...) \
    _log->output(Logger::ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif // _LOGGER_H
