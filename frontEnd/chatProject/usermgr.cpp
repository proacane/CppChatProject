#include "usermgr.h"

bool UserMgr::checkFriendById(int uid) {
    auto iter = _friend_map.find(uid);
    if (iter == _friend_map.end()) {
        return false;
    }

    return true;
}

void UserMgr::setToken(QString token) {
    _token = std::move(token);
}

void UserMgr::addFriend(std::shared_ptr<AuthRsp> auth_rsp) {
    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    _friend_map[friend_info->_uid] = friend_info;
}

void UserMgr::addFriend(std::shared_ptr<AuthInfo> auth_info) {
    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    _friend_map[friend_info->_uid] = friend_info;
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::getApplyList() {
    return _apply_list;
}

bool UserMgr::alreadyApply(int uid) {
    for (auto& apply : _apply_list) {
        if (apply->_uid == uid) {
            return true;
        }
    }

    return false;
}

void UserMgr::setUserInfo(std::shared_ptr<UserInfo> userinfo) {
    _user_info = userinfo;
}

void UserMgr::addApplyList(std::shared_ptr<ApplyInfo> app) {
    _apply_list.push_back(app);
}

void UserMgr::appendApplyList(QJsonArray array) {
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue& value : array) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["avatar"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["gender"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name, desc, icon, nick, sex, status);
        _apply_list.push_back(info);
    }
}
