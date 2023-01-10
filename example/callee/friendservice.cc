#include <iostream>
#include <string>
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "friend.pb.h"
#include <vector>
#include "logger.h"

using namespace std;
using namespace example;

class FriendService : public example::FriendServiceRpc
{
public:
    vector<string> GetFriendsLists(uint32_t userid)
    {
        cout << "do GetFriendService: "
             << "userid" << userid << endl;
        vector<string> vec;
        vec.push_back("friend1");
        vec.push_back("friend2");
        vec.push_back("friend3");
        return vec;
    }

    // 重写基类方法
    void GetFriendsLists(::google::protobuf::RpcController *controller,
                         const ::example::GetFriendsListsRequest *request,
                         ::example::GetFriendsListsResponse *response,
                         ::google::protobuf::Closure *done)
    {
        uint32_t userid = request->userid();
        vector<string> friendsLists = GetFriendsLists(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");

        for (string &name : friendsLists)
        {
            string *p = response->add_friends();
            *p = name;
        }
        done->Run();
    }

private:
};

int main(int argc, char **argv)
{
    LOG_INFO("first log message!");
    LOG_ERR("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);

    // 调用框架的初始化操作 provider -i config.conf
    MprpcApplication::Init(argc, argv);

    // 将UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个rpc服务发布节点 run以后，进程进入阻塞状态，等待远程rpc调用
    provider.Run();

    return 0;
}
