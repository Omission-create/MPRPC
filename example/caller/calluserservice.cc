#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"
using namespace example;

int main(int argc, char **argv)
{
    // 调用框架初始化函数
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    UserServiceRpc_Stub stub(new MprpcChannel());

    // rpc请求参数
    LoginRequest request;
    request.set_name("qishy");
    request.set_pwd("qishy");

    LoginResponse response;
    // 发起rpc方法调用 MprpcChannel::callMethod
    stub.Login(nullptr, &request, &response, nullptr); // RpcChannel::callMethod集中来做所有的rpc方法调用方的参数序列化和网络发送

    // 调用结果
    if (0 == response.result().errcode())
    {
        std::cout << "rpc login response success : " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response failed : " << response.result().errmsg() << std::endl;
    }

    // 演示调用远程发布的rpc方法Register
    // rpc请求参数
    RegisterRequest request2;
    request2.set_id(2000);
    request2.set_name("qishy");
    request2.set_pwd("qishy");
    RegisterResponse response2;

    // 以同步的方式发起rpc请求
    stub.Register(nullptr, &request2, &response2, nullptr);
    // 调用结果
    if (0 == response.result().errcode())
    {
        std::cout << "rpc request response success : " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc request response failed : " << response.result().errmsg() << std::endl;
    }

    return 0;
}