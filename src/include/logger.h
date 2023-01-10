#pragma once
#include "lockqueue.h"
#include <time.h>

// 定义日志级别
enum Loglevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
};

// Mprpc框架提供日志系统
class Logger
{
public:
    // 获取日志的单例
    static Logger &GetInstance();

    // 设置日志级别
    void SetLogLevel(Loglevel level);
    // 写日志
    void Log(string msg);

private:
    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;

    int m_loglevel;              // 记录日志级别
    LockQueue<string> m_lockQue; // 日志缓冲队列
};

// 定义宏
#define LOG_INFO(logmsgformat, ...)                     \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        logger.SetLogLevel(INFO);                       \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c);                                  \
    } while (0);

#define LOG_ERR(logmsgformat, ...)                      \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        logger.SetLogLevel(INFO);                       \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c);                                  \
    } while (0);
