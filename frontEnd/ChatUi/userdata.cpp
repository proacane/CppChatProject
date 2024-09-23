#include "userdata.h"
SearchInfo::SearchInfo(int uid, QString name, QString nick, QString desc, int gender, QString avatar) :
    _uid(uid), _name(name), _nick(nick), _desc(desc), _gender(gender), _avatar(avatar) {
}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc, QString avatar, QString nick, int gender) :
    _from_uid(from_uid), _name(name), _desc(desc), _avatar(avatar), _nick(nick), _gender(gender) {
}

void FriendInfo::AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData> > text_vec) {
    for (const auto& text : text_vec) {
        _chat_msgs.push_back(text);
    }
}
