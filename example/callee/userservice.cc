#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

/**
 * Userservice原来是一个本地服务，提供两个进程内的本地方法，Login和GetFrindList
 */
class UserService : public ::example::UserServiceRpc // 使用在rpc服务发布端(rpc服务提供者)
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << "pwd: " << pwd << std::endl;
        return true;
    }

    /**
     * @brief 重写基类UserServiceRpc虚函数 下面这些方法都是框架直接调用的
     * 1. caller ==> Login(LoginRequest)  ==> muduo ==> callee
     * 2. callee ==> Login(LoginRequest)  ==> 交给下面重写的Login方法上
     *
     * @param controller
     * @param request 请求
     * @param response 响应
     * @param done
     */
    void Login(::google::protobuf::RpcController *controller,
               const ::example::LoginRequest *request,
               ::example::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 框架给业务上报请求参数LoginRequest 应用获取响应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 本地业务
        bool login_result = Login(name, pwd);

        // 把响应写入
        response->set_success(login_result);
        example::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");

        // 执行回调操作 执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作 provider -i config.conf
    MprpcApplication::Init(argc, argv);

    // 将UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点 run以后，进程进入阻塞状态，等待远程rpc调用
    provider.Run();

    return 0;
}