#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <logger.h>
#include <semaphore.h>
#include <iostream>

void global_watcher(zhandle_t *zh, int type,
                    int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT) // 回调消息类型是和会话相关的消息类型
    {
        if (state == ZOO_CONNECTED_STATE) /// zkclient和zkserver连接成功
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr)
{
}
ZkClient::~ZkClient()
{
    if (m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle); // 关闭句柄 释放资源
    }
}
/**
 * @brief zkclient启动连接zkserver
 *
 */
void ZkClient::Start()
{
    string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    string connstr = host + ":" + port;

    /*
        zookeeper_init内部创建线程，poll监控事件，注册watcher回调(3线程)
    */
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (nullptr == m_zhandle)
    {
        LOG_INFO("zookeeper init error!");
        return;
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem); // 给句柄绑定信号量

    sem_wait(&sem); // 阻塞，直至client和server连接成功
    cout << "zookeeper_init success!" << endl;
}
/**
 * @brief 在zkserver上根据指定的path创建znode
 *
 * @param path
 * @param data 节点上存储的数据
 * @param datalen
 * @param state 节点状态，临时性节点还是永久性节点
 */
void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;
    // 先判断path表示的znode节点是否存在
    flag = zoo_exists(m_zhandle, path, 0, nullptr); // 同步api
    if (ZNONODE == flag)
    {
        // 创建指定path的znode节点
        flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if (flag == ZOK)
        {
            cout << "znode create success....path" << path << endl;
        }
        else
        {
            cout << "flag:" << flag << endl;
            cout << "znode create error... path" << path << endl;
            return;
        }
    }
}
// 根据参数指定的znode节点路径，或znode节点的值
string ZkClient::GetData(const char *path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK)
    {
        cout << "get znode error ... path:" << path << endl;
        return "";
    }
    else
    {
        return buffer;
    }
}