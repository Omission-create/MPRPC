#pragma once
#include <queue>
#include <thread>             //pthread_mutex_t
#include <condition_variable> //pthread_condition_t

using namespace std;

// 异步日志的日志队列
template <typename T>
class LockQueue
{
public:
    void Push(T &data)
    {
        lock_guard<mutex> lock(m_mutex); // 自动加锁释放锁
        m_queue.push(data);
        m_condvariable.notify_one();
    }
    T Pop()
    {
        unique_lock<mutex> lock(m_mutex); // 条件变量需要
        while (m_queue.empty())           // while防止线程的虚假唤醒
        {
            // 日志队列为空，线程进入wait状态
            m_condvariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    queue<T> m_queue;
    mutex m_mutex;
    condition_variable m_condvariable;
};
