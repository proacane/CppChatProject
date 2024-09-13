#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include <memory>

#include "singleton.h"
#include "userdata.h"

class UserMgr : public QObject, public SingleTon<UserMgr>, public std::enable_shared_from_this<UserMgr> {
    Q_OBJECT
    friend class SingleTon<UserMgr>;

  public:
    ~UserMgr() = default;
    UserMgr(const UserMgr&) = delete;
    UserMgr& operator=(const UserMgr&) = delete;
    bool checkFriendById(int uid);
    void setUserName(QString name);
    void setUid(int uid);
    void setToken(QString token);

    void addFriend(std::shared_ptr<AuthInfo> auth_info);
    void addFriend(std::shared_ptr<AuthRsp> auth_rsp);
    QString getName() {
        return _user_info->_name    ;
    }
    std::vector<std::shared_ptr<ApplyInfo>> getApplyList();
    int getUid()const{
        return _user_info->_uid;
    }
    QString getAvatar()const{
        return _user_info->_avatar;
    }
    ;
    bool alreadyApply(int uid);
    void setUserInfo(std::shared_ptr<UserInfo> userinfo);
    void addApplyList(std::shared_ptr<ApplyInfo> app);
    void appendApplyList(QJsonArray array);
    void appendFriendList(QJsonArray array);
    std::vector<std::shared_ptr<FriendInfo>> getFriendList();
  private:
    std::shared_ptr<UserInfo> _user_info;
    UserMgr() = default;
    // QString _user_name;
    // int _uid;
    QString _token;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;

    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
  signals:
};

#endif  // USERMGR_H
