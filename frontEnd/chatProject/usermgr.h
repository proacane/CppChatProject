#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>

#include "singleton.h"

class UserMgr : public QObject, public SingleTon<UserMgr>, public std::enable_shared_from_this<UserMgr> {
    Q_OBJECT
    friend class SingleTon<UserMgr>;

  public:
    ~UserMgr()=default;
    UserMgr(const UserMgr&) = delete;
    UserMgr& operator=(const UserMgr&) = delete;

    void setUserName(QString name);
    void setUid(int uid);
    void setToken(QString token);

  private:
    UserMgr() = default;
    QString _user_name;
    int _uid;
    QString _token;
  signals:
};

#endif  // USERMGR_H
