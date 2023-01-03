#pragma once
#include "google/protobuf/service.h"

/**
 * @brief 框架提供的专门发布rpc服务的网络对象类
 *
 */
class RpcProvider
{
public:
    /**
     * @brief 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
     *
     * @param service Service基类指针，用来接收任意的发布service
     */
    void NotifyService(::google::protobuf::Service *service);

    /**
     * @brief 启动rpc服务节点，开始提供网络服务
     *
     */
    void Run();
};