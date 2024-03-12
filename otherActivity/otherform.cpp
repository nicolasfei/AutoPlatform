#include "otherform.h"
#include "ui_otherform.h"
#include "userManager/usermanager.h"
#include "common/commonclass.h"
#include "common/commondialog.h"
#include "communication/businessinvoker.h"
#include "common/cabinetmanagement.h"

OtherForm::OtherForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::otherForm)
{
    ui->setupUi(this);
    ui->frame->setStyleSheet("#frame{border-image: url(:/images/images/communica_g.png);}");
    ui->frame_2->setStyleSheet("#frame_2{border-image: url(:/images/images/communica_g.png);}");
    ui->frame->setVisible(false);
    ui->frame_2->setVisible(false);
}

OtherForm::~OtherForm()
{
    delete ui;
}

void OtherForm::initData()
{

}

void OtherForm::initUiView()
{

}

void OtherForm::startBusiness()
{
    UserManager *manager = UserManager::instance();
    if(manager->getCurrentUser()->getLevel() == User::Normal_User){
        //普通用户不能修改高级用户密码
        ui->lineEdit_4->setEnabled(false);
        ui->lineEdit_5->setEnabled(false);
        ui->lineEdit_6->setEnabled(false);
        ui->setMasterPW->setEnabled(false);
        ui->resetMaster->setStyleSheet(CommonClass::DISABLE_STYLE);
        //普通用户不能重启设备
        ui->resetMaster->setEnabled(false);
        ui->resetMaster->setStyleSheet(CommonClass::DISABLE_STYLE);

    }else{
        ui->setMasterPW->setStyleSheet(CommonClass::ENABLE_STYLE);
        //高级用户可以重启设备
        ui->resetMaster->setEnabled(true);
        ui->resetMaster->setStyleSheet(CommonClass::ENABLE_STYLE);
    }
    ui->setNormalPW->setStyleSheet(CommonClass::ENABLE_STYLE);

    MY_DEBUG << "startBusiness";
    BusinessInvoker *invoker = BusinessInvoker::instance();
    //connect(invoker, &BusinessInvoker::response, this, &ParameterForm::myFormQueryResponse);       //绑定数据查询响应
    //唤醒modbus线程
    invoker->wakeup(BusinessClass::MODBUS_HANDLER);
}

/**
 * @brief ParameterForm::endBusiness--结束业务逻辑
 */
void OtherForm::endBusiness()
{
    MY_DEBUG << "endBusiness";
    BusinessInvoker *invoker = BusinessInvoker::instance();
    //disconnect(invoker, &BusinessInvoker::response, this, &ParameterForm::myFormQueryResponse);       //绑定数据查询响应
    //挂起modbus线程
    invoker->suspend(BusinessClass::MODBUS_HANDLER);
}


/**
 * @brief otherForm::on_resetMaster_clicked--重启下位机
 */
void OtherForm::on_resetMaster_clicked()
{
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败"));
        return;
    }
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromLocal8Bit("重启主控设备"));
    msgBox.setText(QString::fromLocal8Bit("请保证设备工作已经完成，重启设备不会造成故障！"));
    msgBox.setInformativeText(QString::fromLocal8Bit("确定要重启主控设备吗?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:{
        // Save was clicked
        MY_DEBUG;
        BusinessClass bc;
        bc.setLevel(BusinessClass::MODBUS_HANDLER);
        ParameterClass* reset = CabinetManagement::instance()->getMasterContrlReset();
        ParameterClass::RegisterData reg = reset->getRegisterData();
        reg.rw = 1;
        reg.src.uintValue=1;
        bc.addData(reg);
        invoker->exec(bc);
        MY_DEBUG;
    }
        break;
    case QMessageBox::Cancel:
        // Cancel was clicked
        break;
    default:
        // should never be reached
        break;
    }
}

/**
 * @brief otherForm::on_setNormalPW_clicked--普通用户设置密码
 */
void OtherForm::on_setNormalPW_clicked()
{
    UserManager *manager = UserManager::instance();
    User *normalUser = manager->getNomalUser();

    QString oldPW = ui->lineEdit->text();
    if(normalUser->getPwd() != oldPW){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("密码错误"));
        return;
    }

    QString newPW = ui->lineEdit_2->text();
    QString newPWAgein = ui->lineEdit_3->text();
    if(newPW != newPWAgein){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("新密码输入不一致，请检查"));
        return;
    }

    normalUser->updatePW(newPW);
    manager->updatePW(User::Normal_User, newPW);
    CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("密码更新成功"));
}

/**
 * @brief otherForm::on_setMasterPW_clicked--高级用户修改密码
 */
void OtherForm::on_setMasterPW_clicked()
{
    UserManager *manager = UserManager::instance();
    User *adminUser = manager->getAdminUser();

    QString oldPW = ui->lineEdit_4->text();
    if(adminUser->getPwd() != oldPW){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("密码错误"));
        return;
    }

    QString newPW = ui->lineEdit_5->text();
    QString newPWAgein = ui->lineEdit_6->text();
    if(newPW != newPWAgein){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("新密码输入不一致，请检查"));
        return;
    }

    adminUser->updatePW(newPW);
    manager->updatePW(User::Admin_User, newPW);
    CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("密码更新成功"));
}

