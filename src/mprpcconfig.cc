#include "mprpcconfig.h"
#include <iostream>

// 负责解析加载配置文件
void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *fp = fopen(config_file, "r");
    if (nullptr == fp)
    {
        std::cout << config_file << " is not exists!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(fp))
    {
        char buf[512] = {0};
        fgets(buf, 512, fp);

        // 去掉字符串前面多余的空格
        std::string read_buf(buf);
        Trim(read_buf);

        // 判断#注释
        if (read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }

        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            // 配置项不合法
            continue;
        }

        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        Trim(key);
        // 127.0.0.1\n
        int endidx = read_buf.find('\n', idx);
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value);
        m_configMap.insert({key, value});
    }
}
// 查询配置项目信息
std::string MprpcConfig::Load(const std::string &key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}

void MprpcConfig::Trim(std::string &read_buf)
{
    int idx = read_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        read_buf = read_buf.substr(idx, read_buf.size() - idx);
    }
    // 去掉字符串后面的空格
    idx = read_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串后面有空格
        read_buf = read_buf.substr(idx, idx + 1);
    }
}