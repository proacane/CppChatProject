/**
 *  FileName: LogicSystem.h
 *  CreateTime: 2024/8/13 15:28
 *  Description: 
 *  Author: ACAね
*/
#ifndef GATESERVER_LOGICSYSTEM_H
#define GATESERVER_LOGICSYSTEM_H

#include "SingleTon.h"
#include "const.h"

class HttpConnection;

typedef std::function<void(std::shared_ptr<HttpConnection>)> httpHandler;

class LogicSystem : public SingleTon<LogicSystem> {
    // 让 SingleTon 访问私有构造
    friend class SingleTon<LogicSystem>;

public:
    ~LogicSystem();

    /**
     * 处理 get 请求
     * @param url 路由
     * @param connection
     * @return
     */
    bool handleGet(std::string url,std::shared_ptr<HttpConnection> connection);
    /**
     * 绑定路由和其回调函数
     * @param url
     * @param handler
     */
    void registerGet(std::string url,httpHandler handler);

private:
    LogicSystem();
    // 处理 post 请求的函数集合
    std::map<std::string, httpHandler> _post_handlers;
    // 处理 get 请求的函数集合
    std::map<std::string, httpHandler> _get_handlers;

};

#endif //GATESERVER_LOGICSYSTEM_H
