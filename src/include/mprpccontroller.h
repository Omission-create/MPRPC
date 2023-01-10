#pragma once

#include <google/protobuf/service.h>
#include <string>

using namespace google;
using namespace protobuf;
using namespace std;

class MprpcController : public RpcController
{
public:
    MprpcController();
    // 重载RpcController方法
    void Reset();
    bool Failed() const;
    string ErrorText() const;
    void SetFailed(const string& reason);

    // 目前未实现的具体功能
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(Closure *callback);

private:
    bool m_failed;    // Rpc方法执行过程中的状态
    string m_errText; // Rpc方法执行过程中的错误消息
};