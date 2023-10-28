/********************************************************************************
 * @author: MingYu Wang
 * @email: M.Wang56@newcastle.ac.uk
 * @date: 2023/10/28 17:19
 * @description:
 ********************************************************************************/

#ifndef __SKIPLIST_LOG__
#define __SKIPLIST_LOG__

#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
#include <queue>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>

#define BUFSIZE 8 * 1024 * 1024    // 16MB
#define LOGLINESIZE 4096           // 4KB
#define MEM_LIMIT 512 * 512 * 1024 // 512MB

class Logger;

// tips 

#define LOG_INIT(logdir,level) \
    do{ \
        Logger::GetInstance->init(logdir,level); \
    }while(0)


#define LOG(level,fmt,...) \
    do{ \
        if(Logger::GetInstance->get_level() <= level){ \
            Logger::GetInstance->append(level,__FILE__,__LINE__,__FUNCTION__,fmt,__VA_ARGS__); \
        } \
    }while(0)


enum LogLevel
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

class LogBuffer
{
public:
    enum BufState
    {
        FREE = 0,
        FLUSH,
    };
    LogBuffer(int size = BUFSIZ);
    ~LogBuffer();
    int get_usedlen() { return usedlen; };
    int get_availlen() { return bufsize - usedlen; };
    int get_state() { return state; };
    void set_state(BufState s) { state = s; };
    void append(const char *logline, int len);
    void flush_to_file(FILE *fp);

private:
    char *logbuffer;
    uint32_t bufsize;
    uint32_t usedlen;
    int state;
};

class Logger
{
private:
    int level;
    FILE *fp;

    std::map<std::thread::id, LogBuffer *> threadbufmap;

    std::mutex mtx;

    int buftotalnum;

    std::mutex flushmutx;

    std::condition_variable flushcond;

    std::queue<LogBuffer *> flushbufqueue;

    std::mutex freemtx;

    std::queue<LogBuffer *> freebufqueue;

    std::thread flushthread;

    bool start; // flush state

    char save_ymdhms[64]; // time

public:
    Logger();
    ~Logger();
    static Logger *GetInstance()
    {
        static Logger logger;
        return &logger;
    }

    void init(const char *logdir, LogLevel lev);

    int get_level() const
    {
        return level;
    }

    void append(int level,const char *file,int line,const char *func,const char *fmt,...);

    void flush();

};

#endif