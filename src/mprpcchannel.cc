#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
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
        std::cout << "request->SerializeToString failed!" << endl;
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
        std::cout << "rpcHeader.SerializeToString failed!" << endl;
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
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        std::cout << "create socket failed!" << endl;
        ecit(EXIT_FAILURE);
    }
}