#include "usermanager.h"
#include "common/commonclass.h"
#include "common/tool.h"
#include "common/cabinetmanagement.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QTextCodec>

#define CONFIG_FILE_NAME ("user")
#define CONFIG_DIR ("/user")
#define CONFIG_BAK_DIR  ("/userbak")
#define CONFIG_NORMAL_NAME  ("normal")
#define CONFIG_ADMIN_NAME  ("admin")

UserManager::UserManager()
    :user(Q_NULLPTR)
{
//    bool copyOk=false;
    this->userName << QString::fromLocal8Bit("普通用户") << QString::fromLocal8Bit("高级用户");
    this->normalUser = new User(this->userName.at(0), "123456", User::Normal_User);
    this->adminUser = new User(this->userName.at(1), "123456", User::Admin_User);
//    /**
//     * 需要判断是否已经copy一份原始配置文件
//     **/
//    QDir original(QDir::currentPath()+ CONFIG_DIR);
//    if(!original.exists()){
//        if(!original.mkdir(QDir::currentPath()+ CONFIG_DIR)){
//            MY_DEBUG << "create user dir failed";
//            return;
//        }else{
//            QFile normal(QDir::currentPath()+ CONFIG_DIR + "/" + CONFIG_NORMAL_NAME);
//            normal.open(QIODevice::Text |  QIODevice::WriteOnly );
//            QTextStream out(&normal);   //文本流
//            out.setCodec(QTextCodec::codecForName("UTF-8"));
//            out << userName.at(0) << endl << "123456";
//            normal.close();

//            QFile admin(QDir::currentPath()+ CONFIG_DIR + "/" + CONFIG_ADMIN_NAME);
//            admin.open(QIODevice::Text | QIODevice::WriteOnly );
//            QTextStream out1(&admin);   //文本流
//            out1.setCodec(QTextCodec::codecForName("UTF-8"));
//            out1 << userName.at(1) << endl << "123456";
//            admin.close();
//        }
//    }
//    MY_DEBUG;
//    for(int i=0; i<2; i++){
//        QDir copyDir((QDir::currentPath()+CONFIG_BAK_DIR + QString::number(i)));
//        if(!copyDir.exists()){
//            copyOk = Tool::copyDirectoryFiles((QDir::currentPath()+ CONFIG_DIR),
//                                              (QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(i)),true);
//            MY_DEBUG << copyOk;
//        }else{
//            copyOk = true;
//        }
//    }
//    if(copyOk){
//        MY_DEBUG << "---------------------------------------------";
//        CabinetManagement *manager = CabinetManagement::instance();
//        QFile file(QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(manager->getCurrentCabinetID())+ "/" + CONFIG_NORMAL_NAME);
//        file.open(QIODevice::Text | QIODevice::ReadOnly );
//        QTextStream out(&file);   //文本流
//        out.setCodec(QTextCodec::codecForName("UTF-8"));
//        QString name = out.readLine();
//        QString pw = out.readLine();
//        this->normalUser = new User(name, pw, User::Normal_User);
//        file.close();

//        QFile file1(QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(manager->getCurrentCabinetID())+ "/" + CONFIG_ADMIN_NAME);
//        file1.open(QIODevice::Text | QIODevice::ReadOnly );
//        QTextStream out1(&file1);   //文本流
//        out1.setCodec(QTextCodec::codecForName("UTF-8"));
//        QString name1 = out1.readLine();
//        QString pw1 = out1.readLine();
//        this->adminUser = new User(name1, pw1, User::Admin_User);
//        file1.close();
//    }
}

void UserManager::updatePW(User::UserLevel level, QString newPW)
{
    CabinetManagement *manager = CabinetManagement::instance();
    QString filePath;
    QString name;
    switch (level){
    case User::Normal_User:
        filePath = QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(manager->getCurrentCabinetID())+ "/" + CONFIG_NORMAL_NAME;
        name=userName.at(0);
        break;
    case User::Admin_User:
        filePath = QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(manager->getCurrentCabinetID())+ "/" + CONFIG_ADMIN_NAME;
        name=userName.at(1);
        break;
    default:
        break;
    }

    QFile file(filePath);
    if(file.exists()){
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);   //文本流
        out.setCodec(QTextCodec::codecForName("UTF-8"));
        out << name << newPW;
    }
}

UserManager::~UserManager()
{

}

User* UserManager::getCurrentUser()
{
    return this->user;
}

void UserManager::setCurrentUser(User* user)
{
    this->user = user;
}
