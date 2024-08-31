#ifndef USERDATA_H
#define USERDATA_H
/*****************************************************************************
 * @file         userdata.h
 * @brief
 * @description  搜索到的信息
 * @author       ACA
 * @date         2024/08/31
 * @history
 *****************************************************************************/
#include <QString>
class SearchInfo {
  public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int gender);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _gender;
};

#endif  // USERDATA_H
