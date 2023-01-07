#pragma once

#include "mprpcconfig.h"
/**
 * @brief mprpc框架基础类，初始化
 *
 */
class MprpcApplication
{
public:
    static void Init(int argc, char **argv);

    static MprpcApplication &GetInstance();

    static MprpcConfig &GetConfig();

private:
    static MprpcConfig m_config;

    MprpcApplication() {}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete; // 删除拷贝
};