#include "rpcprovider.h"
#include <string>
#include "mprpcapplication.h"
#include <iostream>
using namespace std;

/**
 * @brief 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
 *
 * @param service Service基类指针，用来接收任意的发布service
 */
void RpcProvider::NotifyService(::google::protobuf::Service *service)
{
}

/**
 * @brief 启动rpc服务节点，开始提供网络服务
 *
 */
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventloop, address, "RpcProvider");

    // 绑定连接回调和消息读写回调方法(分离网络代码和业务代码)
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);
    std::cout << "RpcProvider start service @ip: " << ip << " @port: " << port << std::endl;

    // 启动网络服务
    server.start();
    m_eventloop.loop();
}

// 新的socket的连接回调
void RpcProvider::OnConnection(const TcpConnectionPtr &conn)
{
}

// 已建立连接用户的读写时间回调
void RpcProvider::OnMessage(const TcpConnectionPtr &conn,
                            Buffer *buf,
                            Timestamp timestamp)
{
}