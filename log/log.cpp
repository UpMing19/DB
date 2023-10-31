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

LogBuffer::LogBuffer(int size)
{
    bufsize = size;
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
    time_t t = time(nullptr);
    struct tm *ptm = localtime(&t);

    char logfilepath[256] = {0};
    snprintf(logfilepath, 255, "%s/log_%d_%d_%d", logdir, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
    level = lev;

    fp = fopen(logfilepath, "w+");
    if (fp == nullptr)
    {
        printf("logfile opne fail!\n");
    }

    flushthread = std::thread(&Logger::flush, this);
    return;
}
void Logger::append(int level, const char *file, int line, const char *func, const char *fmt, ...)
{
    char logline[LOGLINESIZE];

    struct timeval tv;
    gettimeofday(&tv, NULL);
    static time_t lastsec = 0;

    if (lastsec != tv.tv_sec)
    {
        struct tm *ptm = localtime(&tv.tv_sec);
        lastsec = tv.tv_sec;
        int k = snprintf(save_ymdhms, 64, "%04d-%02d-%02d %02d:%02d:%02d", ptm->tm_year + 1900,
                         ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        save_ymdhms[k] = '\0';
    }
  
    std::thread::id tid = std::this_thread::get_id();

    uint32_t n = snprintf(logline, LOGLINESIZE, "[%s][%s.%03ld][%s:%d %s][pid:%zu]", LevelString[level], save_ymdhms, tv.tv_usec / 1000, file, line, func, std::hash<std::thread::id>()(tid));

    va_list args;
    va_start(args, fmt);

    int m = vsnprintf(logline + n, LOGLINESIZE - n, fmt, args);
    va_end(args);

    int len = n + m;
    LogBuffer *currentlogbuffer = nullptr;
  
    std::map<std::thread::id, LogBuffer *>::iterator it;
    {
        std::lock_guard<std::mutex> lock(mtx);
        it = threadbufmap.find(tid);
        if (it != threadbufmap.end())
        {
            currentlogbuffer = it->second;
        }
        else
        {
            threadbufmap[tid] = currentlogbuffer = new LogBuffer(BUFSIZ);
            buftotalnum++;
            std::cout << "--create new LogBuffer : " << buftotalnum << std::endl;
        }
    }

    if (currentlogbuffer->get_availlen() >= len && currentlogbuffer->get_state() == LogBuffer::BufState::FREE)
    {
        currentlogbuffer->append(logline, len);
    }
    else
    {
        if (currentlogbuffer->get_state() == LogBuffer::BufState::FREE)
        {
            currentlogbuffer->set_state(LogBuffer::BufState::FLUSH);
            {
                std::lock_guard<std::mutex> lock(flushmutx);
                flushbufqueue.push(currentlogbuffer);
            }
            flushcond.notify_one();

            std::lock_guard<std::mutex> lock(freemtx);
            if (!freebufqueue.empty())
            {
                currentlogbuffer = freebufqueue.front();
                freebufqueue.pop();
            }
            else
            {
                if (buftotalnum * BUFSIZ < MEM_LIMIT)
                {
                    currentlogbuffer = new LogBuffer(BUFSIZ);
                    buftotalnum++;
                    std::cout << "create new LogBuffer : " << buftotalnum << std::endl;
                }
                else
                {
                    std::cout << "drop log!" << std::endl;
                    return;
                }
            }
            currentlogbuffer->append(logline, len);
            {
                std::lock_guard<std::mutex> lock2(mtx);
                it->second = currentlogbuffer;
            }
        }
        else
        {
            std::lock_guard<std::mutex> lock(freemtx);
            if (!freebufqueue.empty())
            {
                currentlogbuffer = freebufqueue.front();
                freebufqueue.pop();
                {
                    std::lock_guard<std::mutex> lock2(mtx);
                    it->second = currentlogbuffer;
                }
            }
        }
    }
}

void Logger::flush()
{
    start = true;

    while (true)
    {
        LogBuffer *p;
        {
            std::unique_lock<std::mutex> lock(flushmutx);
            while (start == true && flushbufqueue.empty())
                flushcond.wait(lock);

            if (start == false && flushbufqueue.empty())
                return;
            p = flushbufqueue.front();
            flushbufqueue.pop();
        }

        p->flush_to_file(fp);
        p->set_state(LogBuffer::BufState::FREE);
        {
            std::lock_guard<std::mutex> lock(freemtx);
            freebufqueue.push(p);
        }
    }
}

Logger::~Logger()
{
    std::cout << "---start ~Logger ---" << std::endl;
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::map<std::thread::id, LogBuffer *>::iterator it;
        for (it = threadbufmap.begin(); it != threadbufmap.end(); it++)
        {
            it->second->set_state(LogBuffer::BufState::FLUSH);
            {
                std::lock_guard<std::mutex> lock2(flushmutx);
                flushbufqueue.push(it->second);
            }
        }
    }

    flushcond.notify_one();
    start = false;
    flushcond.notify_one();

    if (flushthread.joinable())
        flushthread.join();

    if (fp != nullptr)
        fclose(fp);
    while (!freebufqueue.empty())
    {
        LogBuffer *p = freebufqueue.front();
        freebufqueue.pop();
        delete p;
    }
    while (!flushbufqueue.empty())
    {
        LogBuffer *p = flushbufqueue.front();
        flushbufqueue.pop();
        delete p;
    }
}
