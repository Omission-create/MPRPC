#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>

// 定义
MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 开始配置文件的加载 rpcserver_ip= rpcserver_port= zookeeper_ip= zookeeper_port=
    m_config.LoadConfigFile(config_file.c_str());
}
MprpcApplication &MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig &MprpcApplication::GetConfig()
{
    return m_config;
}