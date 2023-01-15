#include "rpcprovider.h"
#include <string>
#include "mprpcapplication.h"
#include <iostream>
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

/**
 * @brief 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
 * service对象 => service描述 => method描述
 *
 * @param service Service基类指针，用来接收任意的发布service
 */
void RpcProvider::NotifyService(::Service *service)
{
    ServiceInfo service_info;

    // 获取服务对象的描述信息
    const ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    string service_name = pserviceDesc->name();
    int methodCnt = pserviceDesc->method_count();

    LOG_INFO("service_name:%s", service_name);

    for (int i = 0; i < methodCnt; ++i)
    {
        // 获取服务对象指定下标服务方法的描述（抽象描述）
        const MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
        string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc}); // 存储service对应的method
    }

    service_info.m_service = service;
    m_serviceInfoMap.insert({service_name, service_info}); // 存储service
}

/**
 * @brief 启动rpc服务节点，开始提供网络服务
 *
 */
void RpcProvider::Run()
{
    string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    InetAddress address(ip, port);

    // 创建TcpServer对象
    TcpServer server(&m_eventloop, address, "RpcProvider");

    // 绑定连接回调和消息读写回调方法(分离网络代码和业务代码)
    server.setConnectionCallback(bind(&RpcProvider::OnConnection, this, placeholders::_1));
    server.setMessageCallback(bind(&RpcProvider::OnMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout 30s    zkclient 网络IO线程  1/3*timeout时间发送ping消息
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点 method_name为临时性节点
    for (auto &sp : m_serviceInfoMap)
    {
        // /service_name
        string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap)
        {
            // /service_name/method_name
            string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    cout
        << "RpcProvider start service @ip: " << ip << " @port: " << port << endl;

    // 启动网络服务
    server.start();
    m_eventloop.loop();
}

// 新的socket的连接回调
void RpcProvider::OnConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 和rpc client的连接断开
        conn->shutdown();
    }
}

// 已建立连接用户的读写事件回调
// RpcProvide和RpcConsumer需要协商好通信的protobuf数据类型
// header_size + header_str(service_name+method_name+args_size) + args_str
void RpcProvider::OnMessage(const TcpConnectionPtr &conn,
                            Buffer *buf,
                            Timestamp timestamp)
{
    // 网络上接受的远程rpc调用请求的字节流
    string recv_buf = buf->retrieveAllAsString();

    // 读取前4个字节
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);

    // 根据header_size读取数据头的原始字节流
    string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    string service_name;
    string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据序列头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        // 数据头反序列化失败
        cout << "rpcHeader.ParseFromString failed!" << endl;
        return;
    }

    // 获取rpc方法参数的字符流数据
    string args_str = recv_buf.substr(4 + header_size, args_size);

    // 调试信息
    cout << "service_name: " << service_name << endl;
    cout << "method_name: " << method_name << endl;
    cout << "args_str: " << args_str << endl;

    // 获取service对象和method对象
    auto it = m_serviceInfoMap.find(service_name);
    if (it == m_serviceInfoMap.end())
    {
        cout << "m_serviceInfoMap.find failed!" << endl;
        return;
    }
    Service *service = it->second.m_service;

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        cout << "it->second.m_methodMap.find failed!" << endl;
        return;
    }

    const MethodDescriptor *method = mit->second; // 获取method对象

    // 生成rpc方法调用的请求request和相应response参数
    Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        cout << "request->ParseFromString failed!" << endl;
        return;
    }
    Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method方法的调用，绑定一个closure的回调函数
    // 指定模板参数类型
    Closure *done = NewCallback<RpcProvider, const TcpConnectionPtr &, Message *>(this, &RpcProvider::SendRpcResponse, conn, response);

    // 在框架上根据远端的rpc请求，调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const TcpConnectionPtr &conn, Message *response)
{
    // 序列化
    string response_str;
    if (response->SerializeToString(&response_str))
    {
        // 序列化成功，通过网络把rpc方法执行结果返回rpc调用方
        conn->send(response_str);
    }
    else
    {
        cout << "response->SerializeToString failed!" << endl;
    }
    conn->shutdown(); // 模拟http的短链接服务，有rpcprovider主动断开连接
}