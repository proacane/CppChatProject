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

    void setUserName(QString name);
    void setUid(int uid);
    void setToken(QString token);
    QString getName() {
        return _user_name;
    }
    std::vector<std::shared_ptr<ApplyInfo>> getApplyList();
    int getUid()const{
        return _uid;
    }
    bool alreadyApply(int uid);
    void addApplyList(std::shared_ptr<ApplyInfo> app);
  private:
    UserMgr() = default;
    QString _user_name;
    int _uid;
    QString _token;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
  signals:
};

#endif  // USERMGR_H
