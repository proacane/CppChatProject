/**
 *  FileName: MysqlDao.h
 *  CreateTime: 2024/8/21 13:48
 *  Description: MySql池、DAO操作层
 *  Author: ACAね
*/
#ifndef GATESERVER_MYSQLDAO_H
#define GATESERVER_MYSQLDAO_H

#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <queue>
#include <mutex>
class SqlConnection {
public:
    SqlConnection(sql::Connection *con, int64_t lasttime);

    std::unique_ptr<sql::Connection> _connection;
    // 上次操作数据的时间
    int16_t _last_oper_time;
};

class MysqlPool {
public:
    MysqlPool(std::string url, std::string user, std::string pass, std::string schema,
              int poolSize);

    /**
     * 获取连接
     * @return
     */
    std::unique_ptr<SqlConnection> getConnection();

    /**
     * 归还连接
     * @param connection
     */
    void returnConnection(std::unique_ptr<SqlConnection> connection);

    /**
     * 关闭池
     */
    void close();

    ~MysqlPool();

    /**
     * 检查连接
     */
    void checkConnection();

private:
    std::string _url;
    std::string _user;
    std::string _password;
    std::string _schema;
    int _pool_size;
    std::queue<std::unique_ptr<SqlConnection>> _pool;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::atomic<bool> _b_stop;
    // 检测连接上次操作时间间隔
    std::thread _check_thread;
};

struct UserInfo {
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};

class MysqlDao {
public:
    MysqlDao();

    ~MysqlDao();

    /**
     * 注册用户
     * @param user_name
     * @param email
     * @param password
     * @return -1有异常，0有重复，大于0正常，为uid
     */
    int registerUser(const std::string &user_name, const std::string &email, const std::string &password);

    /**
     * 查询用户名邮箱是否存在
     * @param user_name
     * @param email
     * @return 返回 uid，-1表示异常，0表示不匹配
     */
    int checkEmailUserName(const std::string &user_name, const std::string &email);

    /**
     * 更新密码
     * @param user_name
     * @param password
     * @return 更新结果：-1异常，0 密码重复，1 表示成功
     */
    int updatePassword(const std::string &user_name, const std::string &password);

    /**
     * 登录验证用户名和密码
     * @param user_name
     * @param password
     * @param userInfo
     * @return
     */
    bool checkPassword(const std::string &user_name, const std::string &password, UserInfo &userInfo);

    /**
     * 获取用户信息
     * @param uid
     * @return
     */
    std::shared_ptr<UserInfo> getUser(int uid );
    /**
     * 查询用户信息
     * @param name
     * @return
     */
    std::shared_ptr<UserInfo> getUser(const std::string&name );
private:
    std::unique_ptr<MysqlPool> _pool;
};

#endif //GATESERVER_MYSQLDAO_H
