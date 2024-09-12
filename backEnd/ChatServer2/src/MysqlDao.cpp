/**
 *  FileName: MysqlDao.cpp
 *  CreateTime: 2024/8/21 13:48
 *  Description: 
 *  Author: ACAね
*/
#include <memory>
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
        std::unique_ptr<sql::PreparedStatement> stmt(
                con->_connection->prepareStatement("SELECT COUNT(*) FROM `user` WHERE `name` = ?"));
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
            stmt.reset(con->_connection->prepareStatement(
                    "INSERT INTO `user` (`uid`, `name`, `email`, `pwd`) VALUES (?, ?, ?, ?)"));
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

int MysqlDao::checkEmailUserName(const std::string &user_name, const std::string &email) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return 0;
        }

        // 查询语句
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement("Select email,uid from user where name = ?"));
        // 绑定参数
        preparedStatement->setString(1, user_name);
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(preparedStatement->executeQuery());
        // 遍历结果
        while (res->next()) {
            std::string queryEmail = res->getString("email");
            int queryUid = res->getInt("uid");

            spdlog::info("Query email is {}, Parameter email is {}", queryEmail.c_str(), email);
            spdlog::info("Query UID is {}", queryUid);

            if (email != queryEmail) {
                _pool->returnConnection(std::move(con));
                return 0;
            }

            _pool->returnConnection(std::move(con));
            return queryUid;
        }
    } catch (sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return -1;
    }
    return -1;
}

int MysqlDao::updatePassword(const std::string &user_name, const std::string &password) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return -1;
        }
        // 首先查询当前用户的密码
        std::unique_ptr<sql::PreparedStatement> checkStatement(
                con->_connection->prepareStatement("select pwd from user where name = ?"));
        checkStatement->setString(1, user_name);
        std::unique_ptr<sql::ResultSet> res(checkStatement->executeQuery());

        // 如果找到了该用户的记录
        if (res->next()) {
            std::string currentPwd = res->getString("pwd");
            // 如果当前密码与传入的密码相同
            if (currentPwd == password) {
                _pool->returnConnection(std::move(con));
                return 0;
            }
        }
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement("update user set pwd = ? where name = ?"));
        preparedStatement->setString(1, password);
        preparedStatement->setString(2, user_name);
        int updateCount = preparedStatement->executeUpdate();
        spdlog::info("Updated rows: {}", updateCount);
        _pool->returnConnection(std::move(con));
        return 1;
    } catch (sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return -1;
    }
}

bool MysqlDao::checkPassword(const std::string &user_name, const std::string &password, UserInfo &userInfo) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return false;
        }
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement("SELECT * FROM user WHERE name = ?"));
        preparedStatement->setString(1, user_name);
        std::unique_ptr<sql::ResultSet> res(preparedStatement->executeQuery());
        std::string origin_pwd;
        while (res->next()) {
            origin_pwd = res->getString("pwd");
            spdlog::info("Query pwd is {}", origin_pwd);
            break;
        }
        if (origin_pwd != password) {
            return false;
        }
        userInfo.name = user_name;
        userInfo.email = res->getString("email");
        userInfo.pwd = origin_pwd;
        userInfo.uid = res->getInt("uid");
        return true;
    } catch (const sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return false;
    }
}

std::shared_ptr<UserInfo> MysqlDao::getUser(int uid) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return nullptr;
        }
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement("Select * from user where uid = ?"));
        preparedStatement->setInt(1, uid);
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(preparedStatement->executeQuery());
        std::shared_ptr<UserInfo> user_info = nullptr;
        while (res->next()) {
            user_info = std::make_shared<UserInfo>();
            user_info->name = res->getString("name");
            user_info->pwd = res->getString("pwd");
            user_info->uid = res->getInt("uid");
            user_info->email = res->getString("email");
            user_info->gender = res->getInt("gender");
            user_info->desc = res->getString("desc");
            user_info->avatar = res->getString("avatar");
            break;
        }
        _pool->returnConnection(std::move(con));
        return user_info;
    } catch (const sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return nullptr;
    }
}

std::shared_ptr<UserInfo> MysqlDao::getUser(const std::string &name) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return nullptr;
        }
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement("Select * from user where name = ?"));
        preparedStatement->setString(1, name);
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(preparedStatement->executeQuery());
        std::shared_ptr<UserInfo> user_info = nullptr;
        while (res->next()) {
            user_info = std::make_shared<UserInfo>();
            user_info->name = res->getString("name");
            user_info->pwd = res->getString("pwd");
            user_info->uid = res->getInt("uid");
            user_info->email = res->getString("email");
            user_info->gender = res->getInt("gender");
            user_info->desc = res->getString("desc");
            user_info->avatar = res->getString("avatar");
            break;
        }
        _pool->returnConnection(std::move(con));
        return user_info;
    } catch (const sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return nullptr;
    }
}

bool MysqlDao::addFriendApply(int uid, int to_uid) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return false;
        }
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement(
                        "INSERT IGNORE INTO friend_apply (from_uid, to_uid) VALUES (?, ?);"));
        preparedStatement->setInt(1, uid);
        preparedStatement->setInt(2, to_uid);

        // 执行更新
        int row_affected = preparedStatement->executeUpdate();
        if (row_affected < 0) {
            return false;
        }
        _pool->returnConnection(std::move(con));
        return true;
    } catch (const sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return false;
    }
}

bool MysqlDao::getFriendApplyList(int uid, int limit, std::vector<std::shared_ptr<ApplyInfo>> &apply_list) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return false;
        }
        // 查询申请列表,查出的都是申请人的信息
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement(
                        "select apply.from_uid, apply.status, user.name, user.nick, user.gender, user.avatar from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? order by apply.id ASC LIMIT ? "));
        preparedStatement->setInt(1, uid);
        preparedStatement->setInt(2, limit);
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(preparedStatement->executeQuery());
        // 遍历结果集
        while (res->next()) {
            auto name = res->getString("name");
            auto uid_f = res->getInt("from_uid");
            auto status = res->getInt("status");
            auto nick = res->getString("nick");
            auto gender = res->getInt("gender");
            auto avatar = res->getString("avatar");
            auto apply_ptr = std::make_shared<ApplyInfo>(uid_f, name, "", avatar, nick, gender, status);
            apply_list.push_back(apply_ptr);
        }
        _pool->returnConnection(std::move(con));
        return true;
    } catch (const sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return false;
    }
}

bool MysqlDao::authFriendApply(int uid, int to_uid) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return false;
        }
        // 查询申请列表,查出的都是申请人的信息
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement(
                        "UPDATE friend_apply SET status = 1 WHERE from_uid = ? AND to_uid = ?"));
        // 认证的时候发起方和接收方是相反的
        preparedStatement->setInt(1, to_uid);
        preparedStatement->setInt(2, uid);
        // 执行更新
        int rowAffected = preparedStatement->executeUpdate();
        if (rowAffected < 0) {
            _pool->returnConnection(std::move(con));
            return false;
        }
        return true;
    } catch (const sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return false;
    }
}

bool MysqlDao::addFriend(int from_uid, int to_uid, const std::string &back_name) {
    auto con = _pool->getConnection();
    try {
        if (con == nullptr) {
            return false;
        }
        // 开启事务
        con->_connection->setAutoCommit(false);
        // 添加两条记录，表示互为好友
        std::unique_ptr<sql::PreparedStatement> preparedStatement(
                con->_connection->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) VALUES (?, ?, ?) "));
        preparedStatement->setInt(1, from_uid);
        preparedStatement->setInt(2, to_uid);
        preparedStatement->setString(3, back_name);
        int rowAffected = preparedStatement->executeUpdate();
        if (rowAffected < 0) {
            con->_connection->rollback();
            _pool->returnConnection(std::move(con));
            return false;
        }
        std::unique_ptr<sql::PreparedStatement> preparedStatement2(
                con->_connection->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) VALUES (?, ?, ?) "));
        preparedStatement2->setInt(1, to_uid);
        preparedStatement2->setInt(2, from_uid);
        // TODO 对方同意好友请求后，用户可以自定义备注
        preparedStatement2->setString(3, "");
        rowAffected = preparedStatement2->executeUpdate();
        if (rowAffected < 0) {
            con->_connection->rollback();
            _pool->returnConnection(std::move(con));
            return false;
        }

        con->_connection->commit();
        _pool->returnConnection(std::move(con));
        return true;
    } catch (const sql::SQLException &e) {
        _pool->returnConnection(std::move(con));
        spdlog::warn("SQLException: {} (MySQL error code: {}, SQLState: {})",
                     e.what(),
                     e.getErrorCode(),
                     e.getSQLState());
        return false;
    }
}
