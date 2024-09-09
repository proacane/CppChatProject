/**
 *  FileName: LogicSystem.cpp
 *  CreateTime: 2024/8/13 15:28
 *  Description: 
 *  Author: ACAね
*/
#include "../include/LogicSystem.h"
#include "../include/HttpConnection.h"
#include "../include/VerifyGrpcClient.h"
#include <spdlog/spdlog.h>
#include "../include/RedisMgr.h"
#include "../include/MysqlMgr.h"
#include "../include/StatusGrpcClient.h"

void LogicSystem::registerGet(std::string url, httpHandler handler) {
    _get_handlers.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem() {
//    registerGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
//        beast::ostream(connection->_response.body()) << "receive get_test req\r\n";
//        int i = 0;
//        for (const auto &item: connection->_get_params) {
//            i++;
//            beast::ostream(connection->_response.body()) << "param " << i << " key is " << item.first;
//            beast::ostream(connection->_response.body()) << ", " << i << " value is " << item.second << std::endl;
//        }
//    });
    // 处理验证码
    registerPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
        // 请求转换为 string
        auto body_str = beast::buffers_to_string(connection->_request.body().data());
        spdlog::info("GetVerifyCode Receive body is {}", body_str);
        // 响应类型为 json
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        // 请求解析为json
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            // 解析失败
            spdlog::warn("Failed to parse JSON data!");
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
        }

        auto email = src_root["email"].asString();
        GetVerifyRsp rsp = VerifyGrpcClient::getInstance()->getVerifyCode(email);
        spdlog::info("email is {}", email);
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
    });
    // 处理注册
    registerPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->_request.body().data());
        spdlog::info("Register Receive body is {}", body_str);
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            // 解析失败
            spdlog::warn("Failed to parse JSON data!");
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }

        // 密码校验
        auto user_name = src_root["user"].asString();
        auto email = src_root["email"].asString();
        auto password = src_root["password"].asString();
        auto confirm = src_root["confirm"].asString();
        if (password != confirm) {
            spdlog::warn("The passwords entered twice are inconsistent");
            root["error"] = ErrorCodes::PasswordErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }

        // 没问题去 redis 里查询验证码
        std::string verify_code;
        bool b_get_verify_code = RedisMgr::getInstance()->get("code_" + src_root["email"].asString(), verify_code);
        if (!b_get_verify_code) {
            // 获取失败表示验证码过期或未申请
            spdlog::warn("Verify code expired");
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }
        // 验证码错误
        if (verify_code != src_root["verifycode"].asString()) {
            spdlog::warn("Verify code is not correct");
            root["error"] = ErrorCodes::VerifyCodeErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }

        // 在 MySQL 中查询用户是否存在
        int uid = MysqlMgr::getInstance()->registerUser(user_name, email, password);
        if (uid == 0 || uid == 1) {
            spdlog::info("email or user_name already exists");
            root["error"] = ErrorCodes::UserExist;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }
        // 注册成功删除验证码
        bool b_del_verify = RedisMgr::getInstance()->del("code_" + src_root["email"].asString());
        if (!b_del_verify) {
            spdlog::warn("Delete verifyCode in redis failed");
        }
        // 返回数据
        root["error"] = ErrorCodes::Success;
        root["uid"] = uid;
        root["user"] = user_name;
        root["email"] = email;
        root["password"] = password;
        root["confirm"] = confirm;
        root["verifycode"] = src_root["verifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    });

    registerPost("/reset_pwd", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->_request.body().data());
        spdlog::info("Register Receive body is {}", body_str);
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            // 解析失败
            spdlog::warn("Failed to parse JSON data!");
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }
        auto user_name = src_root["user"].asString();
        auto email = src_root["email"].asString();

        // 先在数据库中查询用户与邮箱是否匹配（是否存在）
        int uid = MysqlMgr::getInstance()->checkEmailUserName(user_name, email);
        if (uid == 0 || uid == -1) {
            root["error"] = ErrorCodes::UserEmailNotMatch;
            std::string jsonStr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonStr;
            return;
        }

        // 在查询验证码
        std::string verify_code;
        bool b_get_verify_code = RedisMgr::getInstance()->get("code_" + src_root["email"].asString(), verify_code);
        if (!b_get_verify_code) {
            // 获取失败表示验证码过期或未申请
            spdlog::warn("Verify code expired");
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }
        // 验证码错误
        if (verify_code != src_root["verifycode"].asString()) {
            spdlog::warn("Verify code is not correct");
            root["error"] = ErrorCodes::VerifyCodeErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }
        auto password = src_root["password"].asString();
        int b_update_password = MysqlMgr::getInstance()->updatePassword(user_name, password);

        if (b_update_password == -1) {
            spdlog::warn("Update password failed");
            root["error"] = ErrorCodes::PasswordUpdateFail;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        } else if (b_update_password == 0) {
            // 密码重复
            spdlog::info("New password matches the current password for user: {}", user_name);
            root["error"] = ErrorCodes::PasswordSame;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }

        // 重置成功删除验证码
        bool b_del_verify = RedisMgr::getInstance()->del("code_" + src_root["email"].asString());
        if (!b_del_verify) {
            spdlog::warn("Delete verifyCode in redis failed");
        }

        spdlog::info("Update password succeed");
        root["error"] = 0;
        root["email"] = email;
        root["user"] = user_name;
        root["password"] = password;
        root["verifycode"] = src_root["verifycode"].asString();
        root["uid"] = uid;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
    });

    registerPost("/user_login", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->_request.body().data());
        spdlog::info("Register Receive body is {}", body_str);
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            // 解析失败
            spdlog::warn("Failed to parse JSON data!");
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }

        auto user_name = src_root["user"].asString();
        auto password = src_root["password"].asString();
        UserInfo userInfo;
        // 在 mysql 中验证输入是否正确
        bool b_valid = MysqlMgr::getInstance()->checkPassword(user_name, password, userInfo);
        if (!b_valid) {
            spdlog::warn("Error password or user doesn't exist");
            root["error"] = ErrorCodes::UserPasswordError;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }

        // 查询StatusServer找到合适的连接
        auto reply = StatusGrpcClient::getInstance()->getChatServer(userInfo.uid);
        if (reply.error()) {
            spdlog::info(" grpc get chat server failed, error is {}",reply.error() );
            root["error"] = ErrorCodes::RPCFailed;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return ;
        }

        spdlog::info("Login succeed, user uid is {}", userInfo.uid);
        root["error"] = ErrorCodes::Success;
        root["user"] = user_name;
        root["uid"] = userInfo.uid;
        root["token"] = reply.token();
        root["host"] = reply.host();
        root["port"] = reply.port();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    });
}

bool LogicSystem::handleGet(std::string url, std::shared_ptr<HttpConnection> connection) {
    if (_get_handlers.find(url) == _get_handlers.end()) {
        // 该路由未注册
        return false;
    }
    _get_handlers[url](connection);
    return true;
}

LogicSystem::~LogicSystem() = default;

void LogicSystem::registerPost(std::string url, httpHandler handler) {
    _post_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::handlePost(std::string url, std::shared_ptr<HttpConnection> connection) {
    if (_post_handlers.find(url) == _post_handlers.end()) {
        return false;
    }
    _post_handlers[url](connection);
    return true;
}
