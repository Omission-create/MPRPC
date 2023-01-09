#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace google;
using namespace protobuf;

class MprpcChannel : public RpcChannel
{
public:
    // 所有通过stub代理对象调用的rpc方法，统一进行序列化和发送
    void CallMethod(const MethodDescriptor *method,
                    RpcController *controller, const Message *request,
                    Message *response, Closure *done);

private:
};
