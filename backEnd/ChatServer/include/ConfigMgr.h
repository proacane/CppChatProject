/**
 *  FileName: ConfigMgr.h
 *  CreateTime: 2024/8/15 16:41
 *  Description: 管理配置信息
 *  Author: ACAね
*/
#ifndef GATESERVER_CONFIGMGR_H
#define GATESERVER_CONFIGMGR_H
#include "const.h"
#include <map>
#include <string>
struct SectionInfo {
    std::map<std::string, std::string> _section_data;

    SectionInfo() = default;

    ~SectionInfo() {
        _section_data.clear();
    };

    SectionInfo(const SectionInfo &src) {
        _section_data = src._section_data;
    };

    SectionInfo &operator=(const SectionInfo &src) {
        if (&src == this) {
            return *this;
        }
        this->_section_data = src._section_data;
        return *this;
    }

    std::string operator[](const std::string &key) {
        if (_section_data.find(key) == _section_data.end()) {
            return " ";
        } else {}
        return _section_data[key];
    }
};

class ConfigMgr {
public:
    static ConfigMgr& getInstance(){
        static ConfigMgr instance;
        return instance;
    }
    ~ConfigMgr() {
        _config_data.clear();
    }

    ConfigMgr(const ConfigMgr &src) {
        this->_config_data = src._config_data;
    }

    SectionInfo operator[](const std::string &section) {
        if (_config_data.find(section) == _config_data.end()) {
            return {};
        }
        return _config_data[section];
    }

    ConfigMgr &operator=(const ConfigMgr &src) {
        if (&src == this) {
            return *this;
        }
        this->_config_data = src._config_data;
        return *this;
    };
private:
    std::map<std::string, SectionInfo> _config_data;
    ConfigMgr();
};
#endif //GATESERVER_CONFIGMGR_H
