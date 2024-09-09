#include "userdata.h"

SearchInfo::SearchInfo(int uid, QString name, QString nick, QString desc, int gender, QString avatar) :
    _uid(uid), _name(name), _nick(nick), _desc(desc), _gender(gender) ,_avatar(avatar){
}
