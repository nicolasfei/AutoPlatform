#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QMutex>
#include <QScopedPointer>
#include <QStringList>

#include "user.h"

class UserManager
{

public:
    static UserManager* instance(){
        static QMutex mutex;
        static QScopedPointer<UserManager> inst;
        if (Q_UNLIKELY(!inst)) {
            mutex.lock();
            if (!inst) {
                inst.reset(new UserManager);
            }
            mutex.unlock();
        }
        return inst.data();
    }
    ~UserManager();

    User *getCurrentUser();
    void setCurrentUser(User* user);

    User * getNomalUser()
    {
        return this->normalUser;
    }
    User* getAdminUser()
    {
        return this->adminUser;
    }

    QStringList getUserName()
    {
        return this->userName;
    }
    void updatePW(User::UserLevel level, QString newPW);

private:
    UserManager();
    //当前用户
    User *user;
    //普通用户
    User *normalUser;
    //高级用户
    User *adminUser;

    QStringList userName;
};

#endif // USERMANAGER_H
