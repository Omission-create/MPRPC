#pragma once
#define THREADED
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>
using namespace std;

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    // zkclient启动连接zkserver
    void Start();
    // 在zkserver上根据指定的path创建znode
    void Create(const char *path, const char *data, int datalen, int state = 0);
    // 根据参数指定的znode节点路径，或znode节点的值
    string GetData(const char *path);

private:
    // zk的客户端句柄
    zhandle_t *m_zhandle;
};
