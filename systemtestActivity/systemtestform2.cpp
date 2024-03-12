#include "systemtestform2.h"
#include "ui_systemtestform2.h"
#include "common/cabinetmanagement.h"
#include "common/commondialog.h"
#include "common/commonclass.h"
#include "common/timertask.h"
#include "communication/businessinvoker.h"

#include <commonView/mylistchooicedialog.h>

#include <qinputdialog.h>

SystemTestForm2::SystemTestForm2(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::SystemTestForm2)
{
    ui->setupUi(this);

    //ui->verticalFrame_2->setStyleSheet("#verticalFrame_2{border-image: url(:/images/images/sys-3.png);}");
    ui->gridFrame_3->setStyleSheet("#gridFrame_3{border-image: url(:/images/images/sys-1.png);}");
    //ui->gridFrame_4->setStyleSheet("#gridFrame_4{border-image: url(:/images/images/sys-4.png);}");
    ui->gridFrame_2->setStyleSheet("#gridFrame_2{border-image: url(:/images/images/sys-5.png);}");
    //ui->gridFrame->setStyleSheet("#gridFrame{border-image: url(:/images/images/sys-2.png);}");

    this->m_businessRunning = false;

    this->initData();
    this->initUiView();
}

SystemTestForm2::~SystemTestForm2()
{
    this->m_businessRunning=false;
    delete ui;
}

void SystemTestForm2::initUiView()      //初始化ui view
{
    if(this->m_gridPowerList.size()>0){
        this->m_gridPowerList.clear();
    }
    this->m_gridPowerList.append(ui->pushButton_6);  //机侧给定1
    this->m_gridPowerList.append(ui->pushButton_7);  //机侧给定2

    this->m_gridPowerList.append(ui->pushButton_8);  //直流母线电压给定
    this->m_gridPowerList.append(ui->pushButton_9);  //网侧无功给定

    this->m_gridPowerList.append(ui->pushButton_10);  //d轴电流给定
    this->m_gridPowerList.append(ui->pushButton_11);  //q轴电流给定

    this->m_gridPowerList.append(ui->pushButton_12);  //PWM d轴给定
    this->m_gridPowerList.append(ui->pushButton_13);  //PWM q轴给定
}

void SystemTestForm2::initData()        //初始化数据
{
}

void SystemTestForm2::startBusiness()   //开始业务逻辑
{
    CabinetManagement *manager = CabinetManagement::instance();
    this->m_parameterList = manager->getSystemFormParameters();
    MY_DEBUG << "SystemTestForm2 startBusiness, parameter size is "<< this->m_parameterList->size();
    if(this->m_parameterList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数文件失败，请检查参数文件"));
        return;
    }

    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &SystemTestForm2::myQueryResponse); //绑定数据查询响应
    //唤醒数据接收线程
    invoker->wakeup(BusinessClass::MODBUS_HANDLER);

    //接收定时任务
    TimerTask *task = TimerTask::instance();
    connect(task, &TimerTask::taskIsTime, this, &SystemTestForm2::myQueryParameterTask);         //定时任务响应
    this->m_businessRunning=true;
}

void SystemTestForm2::endBusiness()     //结束业务逻辑
{
    MY_DEBUG << "SystemTestForm2 endBusiness";
    if(this->m_parameterList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数文件失败，请检查参数文件"));
    }
    this->m_businessRunning=false;
    //解除绑定数据查询响应
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &SystemTestForm2::myQueryResponse);
    //挂起线程
    invoker->suspend(BusinessClass::MODBUS_HANDLER);
    //解除定时任务绑定
    TimerTask *task = TimerTask::instance();
    disconnect(task, &TimerTask::taskIsTime, this, &SystemTestForm2::myQueryParameterTask);
}

/**
 * @brief queryMyParameterTask--数据查询任务
 */
void SystemTestForm2::myQueryParameterTask()
{
    if(!this->m_businessRunning){
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        return;
    }
    if(this->m_parameterList->size()==0){
        //解除定时任务绑定
        TimerTask *task = TimerTask::instance();
        disconnect(task, &TimerTask::taskIsTime, this, &SystemTestForm2::myQueryParameterTask);
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数配置文件失败！"));
        return;
    }
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass::RegisterData reg = this->m_parameterList->at(i)->getRegisterData();
        reg.rw = 0;
        bc.addData(reg);
    }
    invoker->exec(bc);
}
/**
 * @brief mainFormQueryResponse--数据查询响应
 * @param success
 * @param m_Data
 * @param errorID
 * @param errorMsg
 */
void SystemTestForm2::myQueryResponse(BusinessResponse data)
{
    if(data.type!=BusinessClass::REGISTER_RW){
        return;
    }
    if(!data.result){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("查询数据失败：") + data.errorMsg);
        return;
    }

    QList<ParameterClass::RegisterData> dataList = data.getRegisterData();
    for (int i=0; i<dataList.size(); i++)
    {
        ParameterClass::RegisterData rData = dataList.at(i);
        QString dataShow;
        switch(rData.type)
        {
        case ParameterClass::VOID_Type:
            dataShow = QString::number(rData.des.intValue);
            break;
        case ParameterClass::STRING_Type:
            dataShow = QString::number(rData.des.intValue);
            break;
        case ParameterClass::INT16_Type:
        {
            dataShow = QString::number(rData.des.shortValue);
        }
            break;
        case ParameterClass::UINT16_Type:
        {
            dataShow = QString::number(rData.des.ushortValue);
        }
            break;
        case ParameterClass::INT32_Type:
        {
            dataShow = QString::number(rData.des.intValue);
        }
            break;
        case ParameterClass::UINT32_Type:
        {
            dataShow = QString::number(rData.des.uintValue);
        }
            break;
        case ParameterClass::FLOAT32_Type:
        {
            dataShow = QString::number(rData.des.floatValue);
        }
            break;
        }

        //显示到界面
        //1）下拉菜单
        if(rData.addr == 524){       //工作模式
            switch (rData.des.uintValue) {
            case 0:
                ui->pushButton->setText(QStringLiteral("停止"));
                tb10Set=true;
                tb11Set=false;
                tb12Set=false;
                tb13Set=false;
                tb14Set=false;
                tb15Set=false;
                ui->toolButton_10->setIcon(QIcon(":/images/images/ON.png"));
                ui->toolButton_11->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_12->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_13->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_14->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_15->setIcon(QIcon(":/images/images/OFF.png"));
                break;
            case 1:
                ui->pushButton->setText(QStringLiteral("自动运行"));
                break;
            case 2:
                ui->pushButton->setText(QStringLiteral("直流母线充电"));
                tb10Set=false;
                tb11Set=true;
                tb12Set=false;
                tb13Set=false;
                tb14Set=false;
                tb15Set=false;
                ui->toolButton_10->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_11->setIcon(QIcon(":/images/images/ON.png"));
                ui->toolButton_12->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_13->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_14->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_15->setIcon(QIcon(":/images/images/OFF.png"));
                break;
            case 3:
                ui->pushButton->setText(QStringLiteral("主滤波电容充电"));
                tb10Set=false;
                tb11Set=false;
                tb12Set=true;
                tb13Set=false;
                tb14Set=false;
                tb15Set=false;
                ui->toolButton_10->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_11->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_12->setIcon(QIcon(":/images/images/ON.png"));
                ui->toolButton_13->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_14->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_15->setIcon(QIcon(":/images/images/OFF.png"));
                break;
            case 4:
                ui->pushButton->setText(QStringLiteral("并网"));
                tb10Set=false;
                tb11Set=false;
                tb12Set=false;
                tb13Set=true;
                tb14Set=false;
                tb15Set=false;
                ui->toolButton_10->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_11->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_12->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_13->setIcon(QIcon(":/images/images/ON.png"));
                ui->toolButton_14->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_15->setIcon(QIcon(":/images/images/OFF.png"));
                break;
            case 5:
                ui->pushButton->setText(QStringLiteral("网侧运行"));
                tb10Set=false;
                tb11Set=false;
                tb12Set=false;
                tb13Set=false;
                tb14Set=true;
                tb15Set=false;
                ui->toolButton_10->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_11->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_12->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_13->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_14->setIcon(QIcon(":/images/images/ON.png"));
                ui->toolButton_15->setIcon(QIcon(":/images/images/OFF.png"));
                break;
            case 6:
                ui->pushButton->setText(QStringLiteral("机侧运行"));
                tb10Set=false;
                tb11Set=false;
                tb12Set=false;
                tb13Set=false;
                tb14Set=false;
                tb15Set=true;
                ui->toolButton_10->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_11->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_12->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_13->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_14->setIcon(QIcon(":/images/images/OFF.png"));
                ui->toolButton_15->setIcon(QIcon(":/images/images/ON.png"));
                break;
            case 7:
                ui->pushButton->setText(QStringLiteral("测试模式"));
                break;
            default:
                break;
            }
//            isMyModeSelect=true;
//            ui->comboBox->setCurrentIndex(rData.des.uintValue);
            continue;
        }
        if(rData.addr == 526){       //系统模式
            switch (rData.des.uintValue) {
            case 0:
                ui->pushButton_2->setText(QStringLiteral("发电模式"));
                break;
            case 1:
                ui->pushButton_2->setText(QStringLiteral("电动模式"));
                break;
            case 2:
                ui->pushButton_2->setText(QStringLiteral("孤岛模式"));
                break;
            default:
                break;
            }
            continue;
        }
        if(rData.addr == 544){       //网侧模式
            switch (rData.des.uintValue) {
            case 0:
                ui->pushButton_3->setText(QStringLiteral("直流母线控制"));
                break;
            case 1:
                ui->pushButton_3->setText(QStringLiteral("功率控制"));
                break;
            case 2:
                ui->pushButton_3->setText(QStringLiteral("电压频率比控制"));
                break;
            case 3:
                ui->pushButton_3->setText(QStringLiteral("下垂控制"));
                break;
            case 4:
                ui->pushButton_3->setText(QStringLiteral("电流闭环控制"));
                break;
            case 5:
                ui->pushButton_3->setText(QStringLiteral("电压开环控制"));
                break;
            case 6:
                ui->pushButton_3->setText(QStringLiteral("孤岛控制"));
                break;
            case 7:
                ui->pushButton_3->setText(QStringLiteral("电压频率比开环控制"));
                break;
            default:
                break;
            }
            continue;
        }
        if(rData.addr == 546){       //机侧模式
            switch (rData.des.uintValue) {
            case 0:
                ui->pushButton_4->setText(QStringLiteral("功率(转矩)控制"));
                break;
            case 1:
                ui->pushButton_4->setText(QStringLiteral("电压频率比控制"));
                break;
            case 2:
                ui->pushButton_4->setText(QStringLiteral("直流母线控制"));
                break;
            case 3:
                ui->pushButton_4->setText(QStringLiteral("电流闭环控制"));
                break;
            case 4:
                ui->pushButton_4->setText(QStringLiteral("电压开环控制"));
                break;
            case 5:
                ui->pushButton_4->setText(QStringLiteral("孤岛控制"));
                break;
            case 6:
                ui->pushButton_4->setText(QStringLiteral("电压频率比开环控制"));
                break;
            case 7:
                ui->pushButton_4->setText(QStringLiteral("保留"));
                break;
            default:
                break;
            }
            continue;
        }
        if(rData.addr == 574){       //平台选择
            switch (rData.des.uintValue) {
            case 0:
                ui->pushButton_5->setText(QStringLiteral("软件仿真"));
                break;
            case 1:
                ui->pushButton_5->setText(QStringLiteral("半实物仿真"));
                break;
            case 2:
                ui->pushButton_5->setText(QStringLiteral("动模测试"));
                break;
            case 3:
                ui->pushButton_5->setText(QStringLiteral("目标柜体"));
                break;
            default:
                break;
            }
            continue;
        }

        //2）控制按钮
        if(rData.addr==556){
            if(rData.des.uintValue==0){
                ui->pushButton_90->setStyleSheet("background-color: rgb(255, 0, 0);");
                this->pb90Status=false;
            }else{
                ui->pushButton_90->setStyleSheet("background-color: rgb(0, 255, 0);");
                this->pb90Status=true;
            }
            continue;
        }
        if(rData.addr==558){
            if(rData.des.uintValue==0){
                ui->pushButton_91->setStyleSheet("background-color: rgb(255, 0, 0);");
                this->pb91Status=false;
            }else{
                ui->pushButton_91->setStyleSheet("background-color: rgb(0, 255, 0);");
                this->pb91Status=true;
            }
            continue;
        }
        if(rData.addr==562){
            if(rData.des.uintValue==0){
                ui->pushButton_92->setStyleSheet("background-color: rgb(255, 0, 0);");
                this->pb92Status=false;
            }else{
                ui->pushButton_92->setStyleSheet("background-color: rgb(0, 255, 0);");
                this->pb92Status=true;
            }
            continue;
        }
        if(rData.addr==564){
            if(rData.des.uintValue==0){
                ui->pushButton_93->setStyleSheet("background-color: rgb(255, 0, 0);");
                this->pb93Status=false;
            }else{
                ui->pushButton_93->setStyleSheet("background-color: rgb(0, 255, 0);");
                this->pb93Status=true;
            }
            continue;
        }
        if(rData.addr==566){
            if(rData.des.uintValue==0){
                ui->pushButton_94->setStyleSheet("background-color: rgb(255, 0, 0);");
                this->pb94Status=false;
            }else{
                ui->pushButton_94->setStyleSheet("background-color: rgb(0, 255, 0);");
                this->pb94Status=true;
            }
            continue;
        }

        //显示状态图片
        if(rData.addr >= 2256 && rData.addr <= 2270){
            //            MY_DEBUG << dataShow << "---" << rData.addr;
            switch (rData.addr) {
            case 2256:
                if(rData.des.floatValue==0){
                    ui->label_101->setPixmap(QPixmap(":/images/images/tr-1.png"));
                }else{
                    ui->label_101->setPixmap(QPixmap(":/images/images/tg-1.png"));
                }
                break;
            case 2258:
                if(rData.des.floatValue==0){
                    ui->label_102->setPixmap(QPixmap(":/images/images/tr-2.png"));
                }else{
                    ui->label_102->setPixmap(QPixmap(":/images/images/tg-2.png"));
                }
                break;
            case 2260:
                if(rData.des.floatValue==0){
                    ui->label_103->setPixmap(QPixmap(":/images/images/tr-3.png"));
                }else{
                    ui->label_103->setPixmap(QPixmap(":/images/images/tg-3.png"));
                }
                break;
            case 2262:
                if(rData.des.floatValue==0){
                    ui->label_104->setPixmap(QPixmap(":/images/images/tr-4.png"));
                }else{
                    ui->label_104->setPixmap(QPixmap(":/images/images/tg-4.png"));
                }
                break;
            case 2264:
                if(rData.des.floatValue==0){
                    ui->label_105->setPixmap(QPixmap(":/images/images/tr-5.png"));
                }else{
                    ui->label_105->setPixmap(QPixmap(":/images/images/tg-5.png"));
                }
                break;
            case 2266:
                if(rData.des.floatValue==0){
                    ui->label_106->setPixmap(QPixmap(":/images/images/tr-6.png"));
                }else{
                    ui->label_106->setPixmap(QPixmap(":/images/images/tg-6.png"));
                }
                break;
            case 2268:
                if(rData.des.floatValue==0){
                    ui->label_107->setPixmap(QPixmap(":/images/images/tr-7.png"));
                }else{
                    ui->label_107->setPixmap(QPixmap(":/images/images/tg-7.png"));
                }
                break;
            case 2270:
                if(rData.des.floatValue==0){
                    ui->label_108->setPixmap(QPixmap(":/images/images/tr-8.png"));
                }else{
                    ui->label_108->setPixmap(QPixmap(":/images/images/tg-8.png"));
                }
                break;
            }

            continue;
        }

        //参数设置显示
        {
            int parameSet[] = {528, 530, 532, 536, 548, 550, 552, 554};
            for(int k=0; k<sizeof(parameSet)/sizeof(int); k++){
                if(rData.addr == parameSet[k]){
                    this->m_gridPowerList.at(k)->setText(dataShow);
                    continue;
                }
            }
        }
    }
}

/**
 * @brief SystemTestForm2::on_pushButton_90_clicked--机侧pwm手动控制
 */
void SystemTestForm2::on_pushButton_90_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 556){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(this->pb90Status){   //当前是按下状态--弹起Para group5 AddrOffset279 = 0
                reg.src.uintValue = 0;
            }else{                  //当前是弹起状态--按下Para group5 AddrOffset279 = 1
                reg.src.uintValue = 1;
            }
            //下发到modbus
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);
            reg.rw = 1;
            bc.addData(reg);
            invoker->exec(bc);
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《机侧pwm手动控制》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_pushButton_91_clicked--网侧pwm手动控制
 */
void SystemTestForm2::on_pushButton_91_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 558){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(this->pb91Status){   //当前是按下状态--弹起Para group5 AddrOffset280 = 0
                reg.src.uintValue = 0;
            }else{                  //当前是弹起状态--按下Para group5 AddrOffset280 = 1
                reg.src.uintValue = 1;
            }
            //下发到modbus
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);
            reg.rw = 1;
            bc.addData(reg);
            invoker->exec(bc);
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《网侧pwm手动控制》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_pushButton_92_clicked--充电开关手动控制
 */
void SystemTestForm2::on_pushButton_92_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 562){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(this->pb92Status){   //当前是按下状态--弹起Para group5 AddrOffset282 = 0
                reg.src.uintValue = 0;
            }else{                  //当前是弹起状态--按下Para group5 AddrOffset282 = 1
                reg.src.uintValue = 1;
            }
            //下发到modbus
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);
            reg.rw = 1;
            bc.addData(reg);
            invoker->exec(bc);
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《充电开关手动控制》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_pushButton_93_clicked--网侧开关手动控制
 */
void SystemTestForm2::on_pushButton_93_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 564){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(this->pb93Status){   //当前是按下状态--弹起Para group5 AddrOffset283 = 0
                reg.src.uintValue = 0;
            }else{                  //当前是弹起状态--按下Para group5 AddrOffset283 = 1
                reg.src.uintValue = 1;
            }
            //下发到modbus
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);
            reg.rw = 1;
            bc.addData(reg);
            invoker->exec(bc);
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《网侧开关手动控制》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_pushButton_94_clicked--机侧开关手动控制
 */
void SystemTestForm2::on_pushButton_94_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 566){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(this->pb94Status){   //当前是按下状态--弹起Para group5 AddrOffset284 = 0
                reg.src.uintValue = 0;
            }else{                  //当前是弹起状态--按下Para group5 AddrOffset284 = 1
                reg.src.uintValue = 1;
            }
            //下发到modbus
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);
            reg.rw = 1;
            bc.addData(reg);
            invoker->exec(bc);
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《机侧开关手动控制》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_toolButton_10_clicked--单步调试--停止
 */
void SystemTestForm2::on_toolButton_10_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 524){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(!this->tb10Set){
                //下发到modbus
                BusinessClass bc;
                bc.setLevel(BusinessClass::MODBUS_HANDLER);
                reg.src.uintValue = 0;
                reg.rw = 1;
                bc.addData(reg);
                invoker->exec(bc);
            }
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《停止》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_toolButton_11_clicked--直流母线充电
 */
void SystemTestForm2::on_toolButton_11_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
//    if(this->tb10Status){   //停止状态下不操作
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("当前为停止状态！"));
//        return;
//    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 524){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(!this->tb11Set){
                //下发到modbus
                BusinessClass bc;
                bc.setLevel(BusinessClass::MODBUS_HANDLER);
                reg.src.uintValue = 2;
                reg.rw = 1;
                bc.addData(reg);
                invoker->exec(bc);
            }
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《直流母线充电》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_toolButton_12_clicked--网侧滤波电容充电
 */
void SystemTestForm2::on_toolButton_12_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
//    if(this->tb10Status){   //停止状态下不操作
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("当前为停止状态！"));
//        return;
//    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 524){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(!this->tb12Set){
                //下发到modbus
                BusinessClass bc;
                bc.setLevel(BusinessClass::MODBUS_HANDLER);
                reg.src.uintValue = 3;
                reg.rw = 1;
                bc.addData(reg);
                invoker->exec(bc);
            }
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《网侧滤波电容充电》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_toolButton_13_clicked--并网
 */
void SystemTestForm2::on_toolButton_13_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
//    if(this->tb10Status){   //停止状态下不操作
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("当前为停止状态！"));
//        return;
//    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 524){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(!this->tb13Set){
                //下发到modbus
                BusinessClass bc;
                bc.setLevel(BusinessClass::MODBUS_HANDLER);
                reg.src.uintValue = 4;
                reg.rw = 1;
                bc.addData(reg);
                invoker->exec(bc);
            }
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《并网》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_toolButton_14_clicked--网侧运行
 */
void SystemTestForm2::on_toolButton_14_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
//    if(this->tb10Status){   //停止状态下不操作
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("当前为停止状态！"));
//        return;
//    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 524){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(!this->tb14Set){
                //下发到modbus
                BusinessClass bc;
                bc.setLevel(BusinessClass::MODBUS_HANDLER);
                reg.src.uintValue = 5;
                reg.rw = 1;
                bc.addData(reg);
                invoker->exec(bc);
            }
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《网侧运行》参数，请检查！"));
        return;
    }
}

/**
 * @brief SystemTestForm2::on_toolButton_15_clicked--机侧运行
 */
void SystemTestForm2::on_toolButton_15_clicked()
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }
//    if(this->tb10Status){   //停止状态下不操作
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("当前为停止状态！"));
//        return;
//    }
    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == 524){
            ParameterClass::RegisterData reg = p->getRegisterData();
            if(!this->tb15Set){
                //下发到modbus
                BusinessClass bc;
                bc.setLevel(BusinessClass::MODBUS_HANDLER);
                reg.src.uintValue = 6;
                reg.rw = 1;
                bc.addData(reg);
                invoker->exec(bc);
            }
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《机侧运行》参数，请检查！"));
        return;
    }
}

///**
// * @brief SystemTestForm2::on_lineEdit_51_returnPressed--机侧给定1
// */
//void SystemTestForm2::on_lineEdit_51_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_51->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 528){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《机侧给定1》参数，请检查！"));
//        return;
//    }
//}

///**
// * @brief SystemTestForm2::on_lineEdit_52_returnPressed--机侧给定2
// */
//void SystemTestForm2::on_lineEdit_52_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_52->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 530){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《机侧给定2》参数，请检查！"));
//        return;
//    }
//}

///**
// * @brief SystemTestForm2::on_lineEdit_53_returnPressed--直流母线电压给定
// */
//void SystemTestForm2::on_lineEdit_53_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_53->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 532){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《直流母线电压给定》参数，请检查！"));
//        return;
//    }
//}

///**
// * @brief SystemTestForm2::on_lineEdit_54_returnPressed--网侧无功给定
// */
//void SystemTestForm2::on_lineEdit_54_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_54->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 536){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《网侧无功给定》参数，请检查！"));
//        return;
//    }
//}

///**
// * @brief SystemTestForm2::on_lineEdit_55_returnPressed--d轴电流给定
// */
//void SystemTestForm2::on_lineEdit_55_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_55->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 548){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《d轴电流给定》参数，请检查！"));
//        return;
//    }
//}

///**
// * @brief SystemTestForm2::on_lineEdit_56_returnPressed--q轴电流给定
// */
//void SystemTestForm2::on_lineEdit_56_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_56->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 550){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《q轴电流给定》参数，请检查！"));
//        return;
//    }
//}

///**
// * @brief SystemTestForm2::on_lineEdit_57_returnPressed--PWM d轴给定
// */
//void SystemTestForm2::on_lineEdit_57_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_57->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 552){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《PWM d轴给定》参数，请检查！"));
//        return;
//    }
//}

///**
// * @brief SystemTestForm2::on_lineEdit_58_returnPressed--PWM q轴给定
// */
//void SystemTestForm2::on_lineEdit_58_returnPressed()
//{
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
//        return;
//    }
//    bool ok=false;
//    float value = ui->lineEdit_58->text().toFloat(&ok);
//    if(!ok){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误！"));
//        return;
//    }
//    bool isFind=false;
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId == 554){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            //下发到modbus
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);
//            reg.src.floatValue = value;
//            reg.rw = 1;
//            bc.addData(reg);
//            invoker->exec(bc);
//            isFind=true;
//            break;
//        }
//    }
//    if(!isFind){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《PWM q轴给定》参数，请检查！"));
//        return;
//    }
//}

/***
 * 工作模式选择
 */
void SystemTestForm2::on_pushButton_clicked()
{
    MyListChooiceDialog* dialog = new MyListChooiceDialog(this);
    QStringList contents;
    contents.append(QStringLiteral("停止"));
    contents.append(QStringLiteral("自动运行"));
    contents.append(QStringLiteral("直流母线充电"));
    contents.append(QStringLiteral("主滤波电容充电"));
    contents.append(QStringLiteral("并网"));
    contents.append(QStringLiteral("网侧运行"));
    contents.append(QStringLiteral("机侧运行"));
    contents.append(QStringLiteral("测试模式"));
    dialog->setItems(contents);
    connect(dialog, &MyListChooiceDialog::listItemChooice, this, &SystemTestForm2::on_workModelUpdate);
    dialog->showNormal();
}

void SystemTestForm2::on_workModelUpdate(int index)
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
        return;
    }
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId==524){
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);

            ParameterClass::RegisterData reg = p->getRegisterData();
            reg.rw = 1;
//                MY_DEBUG << "write reg is " << index;
            reg.src.uintValue = index;
            bc.addData(reg);
            invoker->exec(bc);

            break;
        }
    }
}

/*****
 * 系统模式
 *
 */
void SystemTestForm2::on_pushButton_2_clicked()
{
    MyListChooiceDialog* dialog = new MyListChooiceDialog(this);
    QStringList contents;
    contents.append(QStringLiteral("发电模式"));
    contents.append(QStringLiteral("电动模式"));
    contents.append(QStringLiteral("孤岛模式"));
    dialog->setItems(contents);
    connect(dialog, &MyListChooiceDialog::listItemChooice, this, &SystemTestForm2::on_systemModelUpdate);
    dialog->showNormal();
}

void SystemTestForm2::on_systemModelUpdate(int index)
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
        return;
    }
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId==526){
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);

            ParameterClass::RegisterData reg = p->getRegisterData();
            reg.rw = 1;
//                MY_DEBUG << "write reg is " << index;
            reg.src.uintValue = index;
            bc.addData(reg);
            invoker->exec(bc);

            break;
        }
    }
}

/******
 * 网侧模式
 */
void SystemTestForm2::on_pushButton_3_clicked()
{
    MyListChooiceDialog* dialog = new MyListChooiceDialog(this);
    QStringList contents;
    contents.append(QStringLiteral("直流母线控制"));
    contents.append(QStringLiteral("功率控制"));
    contents.append(QStringLiteral("电压频率比控制"));
    contents.append(QStringLiteral("下垂控制"));
    contents.append(QStringLiteral("电流闭环控制"));
    contents.append(QStringLiteral("电压开环控制"));
    contents.append(QStringLiteral("孤岛控制"));
    contents.append(QStringLiteral("电压频率比开环控制"));
    dialog->setItems(contents);
    connect(dialog, &MyListChooiceDialog::listItemChooice, this, &SystemTestForm2::on_netModelUpdate);
    dialog->showNormal();
}

void SystemTestForm2::on_netModelUpdate(int index)
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
        return;
    }
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId==544){
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);

            ParameterClass::RegisterData reg = p->getRegisterData();
            reg.rw = 1;
//                MY_DEBUG << "write reg is " << index;
            reg.src.uintValue = index;
            bc.addData(reg);
            invoker->exec(bc);

            break;
        }
    }
}

/**
 * @brief SystemTestForm2::on_pushButton_4_clicked--机侧模式
 */
void SystemTestForm2::on_pushButton_4_clicked()
{
    MyListChooiceDialog* dialog = new MyListChooiceDialog(this);
    QStringList contents;
    contents.append(QStringLiteral("功率(转矩)控制"));
    contents.append(QStringLiteral("电压频率比控制"));
    contents.append(QStringLiteral("直流母线控制"));
    contents.append(QStringLiteral("电流闭环控制"));
    contents.append(QStringLiteral("电压开环控制"));
    contents.append(QStringLiteral("孤岛控制"));
    contents.append(QStringLiteral("电压频率比开环控制"));
    contents.append(QStringLiteral("保留"));
    dialog->setItems(contents);
    connect(dialog, &MyListChooiceDialog::listItemChooice, this, &SystemTestForm2::on_machineModelUpdate);
    dialog->showNormal();
}

void SystemTestForm2::on_machineModelUpdate(int index)
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
        return;
    }
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId==546){
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);

            ParameterClass::RegisterData reg = p->getRegisterData();
            reg.rw = 1;
//                MY_DEBUG << "write reg is " << index;
            reg.src.uintValue = index;
            bc.addData(reg);
            invoker->exec(bc);

            break;
        }
    }
}


void SystemTestForm2::on_pushButton_5_clicked()
{
    MyListChooiceDialog* dialog = new MyListChooiceDialog(this);
    QStringList contents;
    contents.append(QStringLiteral("软件仿真"));
    contents.append(QStringLiteral("半实物仿真"));
    contents.append(QStringLiteral("动模测试"));
    contents.append(QStringLiteral("目标柜体"));
    dialog->setItems(contents);
    connect(dialog, &MyListChooiceDialog::listItemChooice, this, &SystemTestForm2::on_simulationModelUpdate);
    dialog->showNormal();
}

void SystemTestForm2::on_simulationModelUpdate(int index)
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
        return;
    }
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId==574){
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);

            ParameterClass::RegisterData reg = p->getRegisterData();
            reg.rw = 1;
//                MY_DEBUG << "write reg is " << index;
            reg.src.uintValue = index;
            bc.addData(reg);
            invoker->exec(bc);

            break;
        }
    }
}



void SystemTestForm2::on_pushButton_6_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            float fv = value.toFloat(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误(该参数为浮点型)！"));
                return;
            }
            writeReg(528, fv);
        }
}

void SystemTestForm2::on_pushButton_7_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            float fv = value.toFloat(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误((该参数为浮点型))！"));
                return;
            }
            writeReg(530, fv);
        }
}

void SystemTestForm2::on_pushButton_8_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            float fv = value.toFloat(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误((该参数为浮点型))！"));
                return;
            }
            writeReg(532, fv);
        }
}

void SystemTestForm2::on_pushButton_9_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            float fv = value.toFloat(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误((该参数为浮点型))！"));
                return;
            }
            writeReg(536, fv);
        }
}

void SystemTestForm2::on_pushButton_10_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            uint32_t fv = value.toUInt(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误(该参数为无符号整型)！"));
                return;
            }
            writeRegUint(548, fv);
        }
}

void SystemTestForm2::on_pushButton_11_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            uint32_t fv = value.toUInt(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误(该参数为无符号整型)！"));
                return;
            }
            writeRegUint(550, fv);
        }
}

void SystemTestForm2::on_pushButton_12_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            uint32_t fv = value.toUInt(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误(该参数为无符号整型)！"));
                return;
            }
            writeRegUint(552, fv);
        }
}

void SystemTestForm2::on_pushButton_13_clicked()
{
    QInputDialog *inputDialog = new QInputDialog(this);
    bool getInfo;
    QString value = inputDialog->getText(this,QStringLiteral("请输入寄存器值"),QStringLiteral("请输入寄存器值"),
                   QLineEdit::Normal,"",&getInfo,Qt::WindowFlags(0),Qt::ImhNone);
        if(getInfo){
            bool ok=false;
            uint32_t fv = value.toUInt(&ok);
            if(!ok){
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数输入有误(该参数为无符号整型)！"));
                return;
            }
            writeRegUint(554, fv);
        }
}

void SystemTestForm2::writeRegUint(int reg, uint32_t value)
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }

    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == reg){
            ParameterClass::RegisterData reg = p->getRegisterData();
            //下发到modbus
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);
            reg.src.uintValue = value;
            reg.rw = 1;
            bc.addData(reg);
            invoker->exec(bc);
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《机侧给定1》参数，请检查！"));
        return;
    }
}

void SystemTestForm2::writeReg(int reg, float value)
{
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数错误，请检查！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查！"));
        return;
    }

    bool isFind=false;
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId == reg){
            ParameterClass::RegisterData reg = p->getRegisterData();
            //下发到modbus
            BusinessClass bc;
            bc.setLevel(BusinessClass::MODBUS_HANDLER);
            reg.src.floatValue = value;
            reg.rw = 1;
            bc.addData(reg);
            invoker->exec(bc);
            isFind=true;
            break;
        }
    }
    if(!isFind){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件中未找到《机侧给定1》参数，请检查！"));
        return;
    }
}

