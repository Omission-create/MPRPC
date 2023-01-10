#include "logger.h"
#include <thread>
#include <iostream>

using namespace std;

// 获取日志的单例
Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
} // 栈上的静态对象也不会被销毁

// 设置日志级别
void Logger::SetLogLevel(Loglevel level)
{
    m_loglevel = level;
}
// 写日志
void Logger::Log(string msg)
{
    m_lockQue.Push(msg);
}

Logger::Logger(/* args */)
{
    // 启动专门写日志的线程
    thread writeLogTask([&]()
                        { 
                        for (;;)
                        {
                            //获取当前时间，然后取日志信息
                            time_t now = time(nullptr);
                            tm *nowtm = localtime(&now);

                            char file_name[128];
                            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

                            FILE *fp = fopen(file_name, "a+");
                            if(fp==nullptr)
                            {
                                cout << "logger file : " << file_name << " open failed!" << endl;
                                exit(EXIT_FAILURE);
                            }

                            string msg = m_lockQue.Pop();

                            char time_buf[128] = {0};
                            sprintf(time_buf, "%d:%d:%d => [%s]", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec,(m_loglevel==INFO?"info":"error"));
                            msg.insert(0, time_buf);
                            msg.append("\n");

                            fputs(msg.c_str(), fp);
                            fclose(fp);
                        } });
    // 分离线程 守护进程 ★
    writeLogTask.detach();
}
