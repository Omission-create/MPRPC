#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "friend.pb.h"
#include <iostream>
using namespace std;
using namespace example;

int main(int argc, char **argv)
{
    // 调用框架初始化函数
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    FriendServiceRpc_Stub stub(new MprpcChannel());

    // rpc请求参数
    GetFriendsListsRequest request;
    request.set_userid(2000);

    GetFriendsListsResponse response;
    // 发起rpc方法调用 MprpcChannel::callMethod
    MprpcController controller;
    stub.GetFriendsLists(&controller, &request, &response, nullptr); // RpcChannel::callMethod集中来做所有的rpc方法调用方的参数序列化和网络发送

    // 调用结果
    if (controller.Failed())
    {
        cout << controller.ErrorText() << endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "rpc GetFriendsLists response success : " << std::endl;
            int size = response.friends_size();
            for (int i = 0; i < size; ++i)
            {
                cout << "index: " << i + 1 << " name: " << response.friends(i) << endl;
            }
        }
        else
        {
            std::cout << "rpc GetFriendsLists response failed : " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}