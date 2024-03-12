#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include "common/commonclass.h"
#include "common/commondialog.h"
#include "userManager/usermanager.h"
#include "userManager/user.h"

#include <QPainter>


Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);    //运行时最大化窗口
    this->setWindowTitle(QString::fromLocal8Bit("登录"));
    ui->verticalFrame->setStyleSheet("#verticalFrame{border-image: url(:/images/images/login_bg.jpg);}");
    ui->verticalFrame_2->setStyleSheet("#verticalFrame_2{border-image: url(:/images/images/login_bgbk.png);}");

    //初始化用户管理
    UserManager *manager = UserManager::instance();

    QStringList userNames = manager->getUserName();
    ui->comboBox->clear();
    for(int i=0; i<userNames.size(); i++){
        ui->comboBox->addItem(userNames.at(i));
    }

    manager->setCurrentUser(manager->getNomalUser());

    //
    ui->lineEdit->setEchoMode(QLineEdit::Password);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_clicked()
{
    //密码验证
    QString pwd = ui->lineEdit->text();
    if(pwd.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入密码"));
        return;
    }

    UserManager *manager = UserManager::instance();
    MY_DEBUG << "pwd is " << pwd << "user pwd is " << manager->getCurrentUser()->getPwd() <<
                "user level is " << manager->getCurrentUser()->getLevel();
    if(pwd != manager->getCurrentUser()->getPwd()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("密码错误"));
        return;
    }

    //进入主页面
    emit showMain();
    this->hide();
}


void Login::receiverShow()
{
    this->show();
}

/**
 * @brief Login::on_comboBox_currentIndexChanged--用户选择
 * @param index
 */
void Login::on_comboBox_currentIndexChanged(int index)
{
    //用户选择
    UserManager *manager = UserManager::instance();
    switch (index) {
    case 0:
        manager->setCurrentUser(manager->getNomalUser());
        break;
    case 1:
        manager->setCurrentUser(manager->getAdminUser());
        break;
    default:
        break;
    }
}

