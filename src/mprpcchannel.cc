#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include "mprpcapplication.h"
#include "mprpccontroller.h"
#include "zookeeperutil.h"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
using namespace mprpc;
using namespace std;

void MprpcChannel::CallMethod(const MethodDescriptor *method,
                              RpcController *controller, const Message *request,
                              Message *response, Closure *done)
{
    const ServiceDescriptor *sd = method->service();
    string service_name = sd->name();
    string method_name = method->name();

    // 序列化字符串长度
    uint32_t args_size = 0;
    string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        // std::cout << "request->SerializeToString failed!" << endl;
        controller->SetFailed("request->SerializeToString failed!");
        return;
    }

    // rpc请求header
    RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        // std::cout << "rpcHeader.SerializeToString failed!" << endl;
        controller->SetFailed("rpcHeader.SerializeToString failed!");
        return;
    }

    //
    string send_rpc_str;
    send_rpc_str.insert(0, string((char *)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    // 调试信息
    cout << "service_name: " << service_name << endl;
    cout << "method_name: " << method_name << endl;
    cout << "args_str: " << args_str << endl;

    // 使用tcp,完成调用
    // 使用muduo库对socket的封装
    int clientfd = socket(AF_INET, SOCK_STREAM, 0); // 可以定义一个智能指针自动删除
    if (-1 == clientfd)
    {
        // std::cout << "create socket failed!" << endl;
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket errno: %d", errno);
        controller->SetFailed(errtxt);
        exit(EXIT_FAILURE);
    }
    // 读取配置文件rpcserver信息(可以存下来)
    // string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    ZkClient zkCli;
    zkCli.Start();
    string method_path = "/" + service_name + "/" + method_name;
    string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "")
    {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invaild!");
        return;
    }
    string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (-1 == connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        // cout << "connect errno : " << errno << endl;
        char errtxt[512] = {0};
        sprintf(errtxt, "connect errno: %d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }

    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        // cout << "send errno : " << errno << endl;
        char errtxt[512] = {0};
        sprintf(errtxt, "send errno: %d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }

    // 接收rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0)))
    {
        // cout << "recv errno : " << errno << endl;
        char errtxt[512] = {0};
        sprintf(errtxt, "recv errno: %d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }

    // string response_str(recv_buf, 0, recv_size);//string构造函数遇到\0结束
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        // cout << "response->ParseFromString failed!" << endl;
        controller->SetFailed("response->ParseFromString failed!");
        close(clientfd);
        return;
    }
    close(clientfd);
}