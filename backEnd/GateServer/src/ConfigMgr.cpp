/**
 *  FileName: ConfigMgr.cpp
 *  CreateTime: 2024/8/15 16:42
 *  Description: 
 *  Author: ACAね
*/
#include "../include/ConfigMgr.h"

ConfigMgr::ConfigMgr() {
    // 读取当前文件的路径
    boost::filesystem::path current_path = boost::filesystem::current_path();
    // 拼接配置文件的路径
    boost::filesystem::path config_path = current_path / "config.ini";
    std::cout << "config.ini path is: " << config_path.string() << std::endl;

    // 读取文件
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(config_path.string(), pt);


    for (const auto &section_pair: pt) {
        const std::string &section_name = section_pair.first;
        const auto &section_tree = section_pair.second;

        // 对于每个section，遍历其所有的key-value对
        std::map<std::string, std::string> section_config;
        for (const auto &key_value_pair: section_tree) {
            const std::string &key = key_value_pair.first;
            const std::string &value = key_value_pair.second.get_value<std::string>();
            section_config[key] = value;
        }
        SectionInfo sectionInfo;
        sectionInfo._section_data = section_config;
        // 将section的key-value对保存到config_map中
        _config_data[section_name] = sectionInfo;
    }

    // 输出读取的所有配置
    for (const auto &section_entry: _config_data) {
        const std::string &section_name = section_entry.first;
        SectionInfo section_config = section_entry.second;
        std::cout << "[" << section_name << "]" << std::endl;
        for (const auto &key_value_pair: section_config._section_data) {
            std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
        }
    }
}