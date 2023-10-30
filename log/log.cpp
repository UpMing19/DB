/********************************************************************************
 * @author: MingYu Wang
 * @email: M.Wang56@newcastle.ac.uk
 * @date: 2023/10/28 10:55
 * @description:
 ********************************************************************************/

#include "log.h"

const char *LevelString[5] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

void LogBuffer::append(const char *logline, int len)
{
    // todo：check buffer overflow
    memcpy(logbuffer + usedlen, logline, len);
    usedlen += len;
}

void LogBuffer::flush_to_file(FILE *fp)
{
    uint32_t wt_len = fwrite(logbuffer, 1, usedlen, fp);
    if (wt_len != usedlen)
        std::cerr << "fwrite fail!" << std::endl;
    usedlen = 0;
    fflush(fp);
}

LogBuffer::LogBuffer(int size = BUFSIZ)
{
    bufsize = BUFSIZ;
    state = BufState::FREE;
    usedlen = 0;
    logbuffer = new char[bufsize];
    if (logbuffer == nullptr)
        std::cerr << "mem alloc fail : new char" << std::endl;
}

LogBuffer::~LogBuffer()
{
    if (logbuffer != nullptr)
        delete[] logbuffer;
}

Logger::Logger()
{
    level = LogLevel::INFO;
    fp = nullptr;
    buftotalnum = 0;
    start = false;
}

void Logger::init(const char *logdir, LogLevel lev)
{
}
void Logger::append(int level, const char *file, int line, const char *func, const char *fmt, ...)
{
}

void Logger::flush()
{
    start = true;

    while (1)
    {
        LogBuffer *p;
        {
            std::unique_lock<std::mutex> lock(flushmutx);
            while (start == true && flushbufqueue.empty())
                flushcond.wait(lock);

            if (start == false || flushbufqueue.empty())
                return;
            p = flushbufqueue.front();
            flushbufqueue.pop();
        }
        
        p->flush_to_file(fp);
        p->set_state(LogBuffer::BufState::FREE);
        {
            std::lock_guard<std::mutex> lock2(freemtx);
            freebufqueue.push(p);
        }
    }
}

Logger::~Logger()
{
}
