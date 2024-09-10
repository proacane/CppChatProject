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
#include <QJsonArray>
#include <QString>
#include<QJsonObject>
#include <memory>
class SearchInfo {
  public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int gender,QString avatar);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _gender;
    QString _avatar;
};

class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int gender) : _from_uid(from_uid), _name(name), _desc(desc), _avatar(icon), _nick(nick), _gender(gender){};
    int _from_uid;
    QString _name;
    QString _desc;
    QString _avatar;
    QString _nick;
    int     _gender;
};

struct ApplyInfo {
    ApplyInfo(int uid, QString name, QString desc,
              QString icon, QString nick, int gender, int status)
        :_uid(uid),_name(name),_desc(desc),
        _avatar(icon),_nick(nick),_gender(gender),_status(status){}

    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        :_uid(addinfo->_from_uid),_name(addinfo->_name),
        _desc(addinfo->_desc),_avatar(addinfo->_avatar),
        _nick(addinfo->_nick),_gender(addinfo->_gender),
        _status(0)
    {}

    void setIcon(QString head){
        _avatar = head;
    }
    int _uid;
    QString _name;
    QString _desc;
    QString _avatar;
    QString _nick;
    int _gender;
    int _status;
};

struct AuthInfo {
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int gender):
        _uid(uid), _name(name), _nick(nick), _avatar(icon),
        _gender(gender){}
    int _uid;
    QString _name;
    QString _nick;
    QString _avatar;
    int _gender;
};

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_gender)
        :_uid(peer_uid),_name(peer_name),_nick(peer_nick),
        _icon(peer_icon),_gender(peer_gender)
    {}

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _gender;
};

struct TextChatData;
struct FriendInfo {
    FriendInfo(int uid, QString name, QString nick, QString icon,
               int gender, QString desc, QString back, QString last_msg=""):_uid(uid),
        _name(name),_nick(nick),_avatar(icon),_gender(gender),_desc(desc),
        _back(back),_last_msg(last_msg){}

    FriendInfo(std::shared_ptr<AuthInfo> auth_info):_uid(auth_info->_uid),
        _nick(auth_info->_nick),_avatar(auth_info->_avatar),_name(auth_info->_name),
        _gender(auth_info->_gender){}

    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp):_uid(auth_rsp->_uid),
        _nick(auth_rsp->_nick),_avatar(auth_rsp->_icon),_name(auth_rsp->_name),
        _gender(auth_rsp->_gender){}

    void AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec);

    int _uid;
    QString _name;
    QString _nick;
    QString _avatar;
    int _gender;
    QString _desc;
    QString _back;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int gender, QString last_msg = ""):
        _uid(uid),_name(name),_nick(nick),_avatar(icon),_gender(gender),_last_msg(last_msg){}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _avatar(auth->_avatar),_gender(auth->_gender),_last_msg(""){}

    UserInfo(int uid, QString name, QString icon):
        _uid(uid), _name(name), _avatar(icon),_nick(_name),
        _gender(0),_last_msg(""){

    }

    UserInfo(std::shared_ptr<AuthRsp> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _avatar(auth->_icon),_gender(auth->_gender),_last_msg(""){}

    UserInfo(std::shared_ptr<SearchInfo> search_info):
        _uid(search_info->_uid),_name(search_info->_name),_nick(search_info->_nick),
        _avatar(search_info->_avatar),_gender(search_info->_gender),_last_msg(""){

    }

    UserInfo(std::shared_ptr<FriendInfo> friend_info):
        _uid(friend_info->_uid),_name(friend_info->_name),_nick(friend_info->_nick),
        _avatar(friend_info->_avatar),_gender(friend_info->_gender),_last_msg(""){
        _chat_msgs = friend_info->_chat_msgs;
    }

    int _uid;
    QString _name;
    QString _nick;
    QString _avatar;
    int _gender;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

struct TextChatData{
    TextChatData(QString msg_id, QString msg_content, int fromuid, int touid)
        :_msg_id(msg_id),_msg_content(msg_content),_from_uid(fromuid),_to_uid(touid){

    }
    QString _msg_id;
    QString _msg_content;
    int _from_uid;
    int _to_uid;
};

struct TextChatMsg{
    TextChatMsg(int fromuid, int touid, QJsonArray arrays):
        _from_uid(fromuid),_to_uid(touid){
        for(auto  msg_data : arrays){
            auto msg_obj = msg_data.toObject();
            auto content = msg_obj["content"].toString();
            auto msgid = msg_obj["msgid"].toString();
            auto msg_ptr = std::make_shared<TextChatData>(msgid, content,fromuid, touid);
            _chat_msgs.push_back(msg_ptr);
        }
    }
    int _to_uid;
    int _from_uid;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};
#endif  // USERDATA_H
