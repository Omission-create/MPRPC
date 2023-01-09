#pragma once
#include "google/protobuf/service.h"
#include <string>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

using namespace muduo;
using namespace net;
using namespace google;
using namespace protobuf;
using namespace std;

/**
 * @brief 框架提供的专门发布rpc服务的网络对象类
 *
 */
class RpcProvider
{
public:
    /**
     * @brief 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
     *
     * @param service Service基类指针，用来接收任意的发布service
     */
    void NotifyService(Service *service);

    /**
     * @brief 启动rpc服务节点，开始提供网络服务
     *
     */
    void Run();

private:
    // 组合EventLoop
    EventLoop m_eventloop;

    // service服务类型信息
    struct ServiceInfo
    {
        Service *m_service;                                          // 服务对象
        unordered_map<string, const MethodDescriptor *> m_methodMap; // 保存服务方法描述
    };
    // 存储注册成功的服务对象和其方法的信息
    unordered_map<string, ServiceInfo> m_serviceInfoMap;

    // 新的socket的连接回调
    void OnConnection(const TcpConnectionPtr &);

    // 已建立连接用户的读写时间回调
    void OnMessage(const TcpConnectionPtr &,
                   Buffer *,
                   Timestamp);

    // Closure回调操作，用于序列化rpc的相应和网络发送
    void SendRpcResponse(const TcpConnectionPtr &, Message *);
};