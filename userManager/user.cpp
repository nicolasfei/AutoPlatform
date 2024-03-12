#include "user.h"
#include "common/commonclass.h"

User::User(QString name, QString pw, UserLevel level)
{
    MY_DEBUG << "name is " << name << "pw is " << pw << "levl is " << level;
    this->name = name;
    this->pw=pw;
    this->level=level;
}

void User::updatePW(QString pw)
{
    this->pw = pw;
}
