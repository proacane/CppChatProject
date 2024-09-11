/**
 *  FileName: RedisMgr.h
 *  CreateTime: 2024/8/19 13:23
 *  Description: Redis管理
 *  Author: ACAね
*/
#ifndef GATESERVER_REDISMGR_H
#define GATESERVER_REDISMGR_H

#include <hiredis/hiredis.h>
#include "SingleTon.h"
#include <atomic>
#include <mutex>
#include <queue>

class RedisConPool {
public:
    RedisConPool(size_t poolSize, const char* host, int port, const std::string& password);
    ~RedisConPool();
    redisContext * getConnection();
    void returnConnection(redisContext *context);
    // 关闭池子
    void close();
private:
    // 是否关闭连接池
    std::atomic<bool> _b_stop;
    std::mutex _mutex;
    std::condition_variable _cond;
    // 连接池
    std::queue<redisContext *> _connections;
    std::string _host;
    int _port;
    size_t _pool_size;
};

class RedisMgr : public SingleTon<RedisMgr>, public std::enable_shared_from_this<RedisMgr> {
    friend class SingleTon<RedisMgr>;

public:
    ~RedisMgr() override;

    /**
     * 根据key获取 value
     * @param key
     * @param value 返回的值
     * @return
     */
    bool get(const std::string &key, std::string &value);

    /**
     * 设置 k-v
     * @param key
     * @param value
     * @return
     */
    bool set(const std::string &key, const std::string &value);


    /**
     * 左侧push
     * @param key
     * @param value
     * @return
     */
    bool lPush(const std::string &key, const std::string &value);

    /**
     * 左侧pop
     * @param key
     * @param value
     * @return
     */
    bool lPop(const std::string &key, std::string &value);

    /**
     * 右侧push
     * @param key
     * @param value
     * @return
     */
    bool rPush(const std::string &key, const std::string &value);

    /**
     * 右侧pop
     * @param key
     * @param value
     * @return
     */
    bool rPop(const std::string &key, std::string &value);

    /**
     * hset
     * @param key
     * @param field
     * @param value
     * @return
     */
    bool hSet(const std::string &key, const std::string &field, const std::string &value);

    /**
     * hset二进制数据
     * @param key
     * @param field
     * @param hvalue
     * @param hvaluelen
     * @return
     */
    bool hSet(const char *key, const char *field, const char *hvalue, size_t hvaluelen);

    /**
     * hget
     * @param key
     * @param field
     * @return
     */
    std::string hGet(const std::string &key, std::string &field);

    /**
     * 删除k-v
     * @param key
     * @return
     */
    bool del(const std::string &key);

    /**
     * key 是否存在
     * @param key
     * @return
     */
    bool existsKey(const std::string &key);

    /**
     * 关闭连接
     */
    void close();

    bool hDel(const std::string& key, const std::string& field);
private:
    RedisMgr();
    std::unique_ptr<RedisConPool> _con_pool;
};

#endif //GATESERVER_REDISMGR_H
