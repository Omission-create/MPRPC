#include "test.pb.h"
#include <iostream>
using namespace fixbug;
using namespace google;
using namespace protobuf;

int main()
{
    LoginResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = 
    user1->set_name("qishy");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;
    return 0;
}

// int main1()
// {
//     // 序列化
//     LoginRequest req;
//     req.set_name("qishy");
//     req.set_pwd("123456");

//     std::string send_str;
//     if (req.SerializeToString(&send_str))
//     {
//         std::cout << send_str.c_str() << std::endl;
//     }

//     // 反序列化
//     LoginRequest reqB;
//     if (reqB.ParseFromString(send_str))
//     {
//         std::cout << "name: " << reqB.name() << "  "
//                   << "pwd:" << reqB.pwd() << std::endl;
//     }

//     return 0;
// }