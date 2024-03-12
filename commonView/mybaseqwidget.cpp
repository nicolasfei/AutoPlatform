#include "mybaseqwidget.h"
#include "common/cabinetmanagement.h"

MyBaseQWidget::MyBaseQWidget(QWidget *parent) : QWidget(parent)
{
    this->m_businessRunning=false;
    this->m_parameterList=NULL;
    CabinetManagement *manager = CabinetManagement::instance();
    connect(manager, &CabinetManagement::cabinetIdSwitch, this, &MyBaseQWidget::on_cabinetIdSwitch);

    this->waitDialog = new MyWaitDialog(this);
}

MyBaseQWidget::~MyBaseQWidget()
{
    this->m_businessRunning=false;
    CabinetManagement *manager = CabinetManagement::instance();
    disconnect(manager, &CabinetManagement::cabinetIdSwitch, this, &MyBaseQWidget::on_cabinetIdSwitch);
    delete this->waitDialog;
}

void MyBaseQWidget::initUiView()
{

}

void MyBaseQWidget::initData()
{

}

void MyBaseQWidget::startBusiness()
{

}

void MyBaseQWidget::endBusiness()
{

}

/**
 * @brief MyBaseQWidget::on_cabinetIdSwitch--机柜被切换了
 * @param cabinetID--要切换到那个机柜ID
 */
void MyBaseQWidget::on_cabinetIdSwitch()
{
    if(this->m_businessRunning){
        this->endBusiness();
        this->initData();
        this->initUiView();
        this->startBusiness();
    }else{
        this->initData();
        this->initUiView();
    }
    emit activitySwitchFinish(activityID);
}
