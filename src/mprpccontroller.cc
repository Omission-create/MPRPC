#include "mprpccontroller.h"

MprpcController::MprpcController()
{
    m_failed = false;
    m_errText = "";
}
// 重载RpcController方法
void MprpcController::Reset()
{
    m_failed = false;
    m_errText = "";
}
bool MprpcController::Failed() const
{
    return m_failed;
}
string MprpcController::ErrorText() const
{
    return m_errText;
}
void MprpcController::SetFailed(const string &reason)
{
    m_failed = true;
    m_errText = reason;
}

// 目前未实现的具体功能
void MprpcController::StartCancel() {}
bool MprpcController::IsCanceled() const { return false; }
void MprpcController::NotifyOnCancel(Closure *callback) {}