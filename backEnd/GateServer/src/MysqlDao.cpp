/**
 *  FileName: MysqlDao.cpp
 *  CreateTime: 2024/8/21 13:48
 *  Description: 
 *  Author: ACAね
*/
#include <utility>
#include <spdlog/spdlog.h>
#include "../include/MysqlDao.h"
#include "../include/const.h"
#include <jdbc/cppconn/statement.h>
#include "../include/ConfigMgr.h"
#include <jdbc/cppconn/prepared_statement.h>

SqlConnection::SqlConnection(sql::Connection *con, int64_t lasttime) : _connection(con),
                                                                       _last_oper_time(lasttime) {

}

MysqlPool::MysqlPool(std::string url, std::string user, std::string pass,
                     std::string schema, int poolSize) : _url(std::move(url)), _user(std::move(user)),
                                                         _password(std::move(pass)),
                                                         _schema(std::move(schema)), _pool_size(poolSize) {
    try {
        for (size_t i = 0; i < poolSize; i++) {
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            auto con(driver->connect(_url, _user, _password));
            con->setSchema(_schema);
            // 获取当前时间戳
            auto currentTime = std::chrono::system_clock::now().time_since_epoch();
            // 转换为秒
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
            _pool.push(std::make_unique<SqlConnection>(con, timestamp));

            // 检查连接
            _check_thread = std::thread([this] {
                while (!_b_stop) {
                    checkConnection();
                    std::this_thread::sleep_for(std::chrono::seconds(600));
                }
            });
            _check_thread.detach();
        }
    } catch (sql::SQLException &e) {
        spdlog::warn("Mysql Pool init failed: {}", e.what());
    }
}

void MysqlPool::checkConnection() {
    std::lock_guard<std::mutex> lock(_mutex);
    int pool_size = _pool.size();
    // 获取当前时间戳
    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    // 转换为秒
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

    // 开始检查
    for (int i = 0; i < pool_size; i++) {
        auto con = std::move(_pool.front());
        _pool.pop();
        // 执行完本次循环的时候就会放进去
        Defer defer([this, &con] {
            _pool.push(std::move(con));
        });

        if (timestamp - con->_last_oper_time <= 3600) {
            // 一个小时之内没问题
            continue;
        }
        try {
            std::unique_ptr<sql::Statement> statement(con->_connection->createStatement());
            statement->executeQuery("Select 1");
            con->_last_oper_time = timestamp;
//            spdlog::info("Execute timer alive query, current time is {}", timestamp);
        } catch (sql::SQLException &e) {
            spdlog::warn("Error keeping mysql connection alive: {}", e.what());
            // 重新创建连接并替换旧的连接
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            auto *newcon = driver->connect(_url, _user, _password);
            newcon->setSchema(_schema);
            con->_connection.reset(newcon);
            con->_last_oper_time = timestamp;
        }
    }
}

std::unique_ptr<SqlConnection> MysqlPool::getConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] {
        if (_b_stop) {
            return true;
        }
        return !_pool.empty();
    });
    if (_b_stop) {
        return nullptr;
    }
    std::unique_ptr<SqlConnection> con(std::move(_pool.front()));
    _pool.pop();
    return con;
}

void MysqlPool::returnConnection(std::unique_ptr<SqlConnection> connection) {
    std::unique_lock<std::mutex> lock(_mutex);
    if (_b_stop) {
        return;
    }
    _pool.push(std::move(connection));
    _cond.notify_one();
}

void MysqlPool::close() {
    _b_stop = true;
    _cond.notify_all();
}

MysqlPool::~MysqlPool() {
    std::lock_guard<std::mutex> lock(_mutex);
    while (!_pool.empty()) {
        _pool.pop();
    }
}


MysqlDao::MysqlDao() {
    // 读取配置，创建连接
    auto &config = ConfigMgr::getInstance();
    const auto &host = config["Mysql"]["Host"];
    const auto &port = config["Mysql"]["Port"];
    const auto &password = config["Mysql"]["Password"];
    const auto &schema = config["Mysql"]["Schema"];
    const auto &user = config["Mysql"]["User"];

    _pool = std::make_unique<MysqlPool>(host + ":" + port, user, password, schema, 5);
}

MysqlDao::~MysqlDao() {
    _pool->close();
}

int MysqlDao::registerUser(const std::string &user_name, const std::string &email, const std::string &pwd) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            // 池子关闭
            return false;
        }
        // 开始事务
        con->_connection->setAutoCommit(false);

        // 检查用户名是否已存在
        std::unique_ptr<sql::PreparedStatement> stmt(con->_connection->prepareStatement("SELECT COUNT(*) FROM `user` WHERE `name` = ?"));
        stmt->setString(1, user_name);
        // 执行结果
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        res->next();
        int exists_name = res->getInt(1);

        // 检查 email 是否已存在
        stmt.reset(con->_connection->prepareStatement("SELECT COUNT(*) FROM `user` WHERE `email` = ?"));
        stmt->setString(1, email);
        res.reset(stmt->executeQuery());
        res->next();
        int exists_email = res->getInt(1);
        // 返回结果
        int result = 0;

        if (exists_name > 0 || exists_email > 0) {
            // 用户名或 email 已存在，返回 0
            result = 0;
            con->_connection->commit();
        } else {
            // 更新 user_id 表
            stmt.reset(con->_connection->prepareStatement("UPDATE `user_id` SET `id` = `id` + 1"));
            stmt->executeUpdate();

            // 获取更新后的 id
            stmt.reset(con->_connection->prepareStatement("SELECT `id` FROM `user_id`"));
            res.reset(stmt->executeQuery());
            res->next();
            int new_id = res->getInt(1);

            // 在 user 表中插入新记录
            stmt.reset(con->_connection->prepareStatement("INSERT INTO `user` (`uid`, `name`, `email`, `pwd`) VALUES (?, ?, ?, ?)"));
            stmt->setInt(1, new_id);
            stmt->setString(2, user_name);
            stmt->setString(3, email);
            stmt->setString(4, pwd);
            stmt->executeUpdate();

            // 设置 result 为新插入的 uid
            result = new_id;
            con->_connection->commit();
        }
        _pool->returnConnection(std::move(con));
        // 返回结果
        return result;
    }
    catch (sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return -1;
    }
}
