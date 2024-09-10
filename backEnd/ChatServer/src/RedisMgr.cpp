/**
 *  FileName: RedisMgr.cpp
 *  CreateTime: 2024/8/19 13:23
 *  Description: 
 *  Author: ACAね
*/
#include <memory>
#include <utility>
#include "../include/ConfigMgr.h"
#include "../../ChatServer2/include/RedisMgr.h"
#include <spdlog/spdlog.h>

RedisMgr::RedisMgr() {
    auto &gCfgMgr = ConfigMgr::getInstance();
    auto host = gCfgMgr["Redis"]["Host"];
    auto port = gCfgMgr["Redis"]["Port"];
    auto pwd = gCfgMgr["Redis"]["Password"];
    _con_pool = std::make_unique<RedisConPool>(5, host.c_str(), atoi(port.c_str()), pwd.c_str());
}

RedisMgr::~RedisMgr() {
    close();
}

bool RedisMgr::get(const std::string &key, std::string &value) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "GET %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        spdlog::warn("[GET {}] execution error", key);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
        return false;
    }

    // 赋值
    value = reply->str;
    freeReplyObject(reply);
    spdlog::info("[GET {}] execution succeed", key);
    _con_pool->returnConnection(connect);
    return true;
}

bool RedisMgr::set(const std::string &key, const std::string &value) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    // 执行命令
    auto reply = (redisReply *) redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());
    if (reply == nullptr || !(reply->type == REDIS_REPLY_STATUS &&
                              (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0))) {
        spdlog::warn("[Set {} {}] execution error", key, value);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);

        return false;
    }
    // 执行成功
    freeReplyObject(reply);
    spdlog::info("[Set {} {}] execution succeed", key, value);
    _con_pool->returnConnection(connect);
    return true;
}


bool RedisMgr::lPush(const std::string &key, const std::string &value) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        spdlog::warn("[LPUSH {} {}] execution error", key, value);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
        return false;
    }

    freeReplyObject(reply);
    spdlog::info("[LPUSH {} {}] execution succeed", key, value);
    _con_pool->returnConnection(connect);
    return true;
}

bool RedisMgr::lPop(const std::string &key, std::string &value) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "LPOP %s", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        spdlog::warn("[LPOP {}] execution error", key);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
//    std::cout << "[LPOP " << key << "] execution succeed" << std::endl;
    spdlog::info("[LPOP {}] execution succeed", key);
    _con_pool->returnConnection(connect);
    return true;
}

bool RedisMgr::rPush(const std::string &key, const std::string &value) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
//        std::cout << "[RPUSH " << key << " " << value << "] execution error" << std::endl;
        spdlog::warn("[RPUSH {} {}] execution error", key, value);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
        return false;
    }

    freeReplyObject(reply);
//    std::cout << "[RPUSH " << key << " " << value << "] execution succeed" << std::endl;
    spdlog::info("[RPUSH {} {}] execution succeed", key, value);
    _con_pool->returnConnection(connect);
    return true;
}

bool RedisMgr::rPop(const std::string &key, std::string &value) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "RPOP %s", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
//        std::cout << "[RPOP " << key << "] execution error" << std::endl;
        spdlog::warn("[RPOP {}] execution error", key);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);

        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
//    std::cout << "[RPOP " << key << "] execution succeed" << std::endl;
    spdlog::info("[RPOP {}] execution succeed", key);
    _con_pool->returnConnection(connect);

    return true;
}

bool RedisMgr::hSet(const std::string &key, const std::string &field, const std::string &value) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "HSET %s %s %s", key.c_str(), field.c_str(),
                                             value.c_str());

    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
//        std::cout << "[HSet " << key << "  " << field << "  " << value << "] execution error" << std::endl;
        spdlog::warn("[HSet {} {} {}] execution error", key, field, value);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
        return false;
    }
//    std::cout << "[HSet " << key << "  " << field << "  " << value << "] execution succeed" << std::endl;
    spdlog::info("[HSet {} {} {}] execution succeed", key, field, value);
    freeReplyObject(reply);
    _con_pool->returnConnection(connect);
    return true;
}

bool RedisMgr::hSet(const char *key, const char *field, const char *hvalue, size_t hvaluelen) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    const char *argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = field;
    argvlen[2] = strlen(field);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    auto reply = (redisReply *) redisCommandArgv(connect, 4, argv, argvlen);
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
//        std::cout << "[HSet " << key << "  " << field << "  " << hvalue << "] execution error" << std::endl;
        spdlog::warn("[HSet {} {} {}] execution error", key, field, hvalue);
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
        return false;
    }
//    std::cout << "[HSet " << key << "  " << field << "  " << hvalue << "] execution succeed" << std::endl;
    spdlog::info("[HSet {} {} {}] execution succeed", key, field, hvalue);
    freeReplyObject(reply);
    _con_pool->returnConnection(connect);

    return true;
}

std::string RedisMgr::hGet(const std::string &key, std::string &field) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return "";
    }
    const char *argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = field.c_str();
    argvlen[2] = field.length();

    auto reply = (redisReply *) redisCommandArgv(connect, 3, argv, argvlen);
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
//        std::cout << "[HGet " << key << " " << field << "] execution error" << std::endl;
        spdlog::warn("[HGet {} {} ] execution error", key, field);
        return "";
    }
    std::string value(reply->str);
    freeReplyObject(reply);
//    std::cout << "[HGet " << key << " " << field << "] execution succeed" << std::endl;
    spdlog::info("[HGet {} {} ] execution succeed", key, field);
    _con_pool->returnConnection(connect);
    return value;
}

bool RedisMgr::del(const std::string &key) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "DEL %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        spdlog::warn("[Del {}] execution error", key);
//        std::cout << "[Del " << key << "] execution error" << std::endl;
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);
        return false;
    }
//    std::cout << "[Del " << key << "] execution succeed" << std::endl;
    spdlog::info("[Del {}] execution succeed", key);
    freeReplyObject(reply);
    _con_pool->returnConnection(connect);

    return true;
}

bool RedisMgr::existsKey(const std::string &key) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "exists %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
        spdlog::info("[Key {}] does not exist ", key);
//        std::cout << "[Key " << key << "] does not exist " << std::endl;
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
        _con_pool->returnConnection(connect);

        return false;
    }
//    std::cout << "[Key " << key << "] exists" << std::endl;
    spdlog::info("[Key {}] exists", key);
    freeReplyObject(reply);
    _con_pool->returnConnection(connect);
    return true;

}

void RedisMgr::close() {
    // 回收池子
    _con_pool->close();
}

bool RedisMgr::hDel(const std::string &key, const std::string &field) {
    auto connect = _con_pool->getConnection();
    if (connect == nullptr) {
        return false;
    }

    Defer defer([&connect, this]() {
        _con_pool->returnConnection(connect);
    });

    redisReply* reply = (redisReply*)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
    if (reply == nullptr) {
        std::cerr << "HDEL command failed" << std::endl;
        return false;
    }

    bool success = false;
    if (reply->type == REDIS_REPLY_INTEGER) {
        success = reply->integer > 0;
    }

    freeReplyObject(reply);
    return success;
}

RedisConPool::RedisConPool(size_t poolSize, const char *host, int port,
                           const std::string &password) : _pool_size(poolSize), _host(host), _port(port),
                                                          _b_stop(false) {
    for (size_t i = 0; i < poolSize; i++) {
        auto *context = redisConnect(host, port);
        if (context == nullptr || context->err != 0) {
            if (context != nullptr) {
                redisFree(context);
            }
            continue;
        }
        auto reply = (redisReply *) redisCommand(context, "AUTH %s", password.c_str());
        if (reply->type == REDIS_REPLY_ERROR) {
//            std::cout << "Authentication failed" << std::endl;
            spdlog::error("Authentication failed");
            // 释放redisCommand执行后返回的redisReply所占用的内存
            freeReplyObject(reply);
            continue;
        }
        freeReplyObject(reply);
        _connections.push(context);
    }
//    std::cout << "Redis connect pool has total " << _connections.size() << " context\n";
    spdlog::info("Redis connect pool has total {} contexts", _connections.size());
}

RedisConPool::~RedisConPool() {
    std::lock_guard<std::mutex> lock(_mutex);
    while (!_connections.empty()) {
        auto *context = _connections.front();
        redisFree(context);
        _connections.pop();
    }
}

redisContext *RedisConPool::getConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] {
        if (_b_stop) {
            // 关闭线程池继续执行
            return true;
        }
        return !_connections.empty();
    });
    if (_b_stop) {
        // 池子关闭了
        return nullptr;
    }
    auto *context = _connections.front();
    _connections.pop();
    return context;
}

void RedisConPool::returnConnection(redisContext *context) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_b_stop) {
        // 池子关闭了释放就行
        redisFree(context);
        return;
    }
    _connections.push(context);
    _cond.notify_one();
}

void RedisConPool::close() {
    _b_stop = true;
    _cond.notify_all();
}
