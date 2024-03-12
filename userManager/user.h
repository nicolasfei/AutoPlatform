#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    typedef enum User_Level{
        Normal_User = 0,
        Admin_User
    }UserLevel;

public:
    User(QString name, QString pw, UserLevel level);

    QString getName()
    {
        return this->name;
    }

    QString getPwd()
    {
        return this->pw;
    }

    User_Level getLevel()
    {
        return this->level;
    }

    void updatePW(QString pw);
protected:
    QString name;
    QString pw;
    UserLevel level;
};

#endif // USER_H
