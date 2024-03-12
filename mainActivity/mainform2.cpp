#include "mainform2.h"
#include "ui_mainform2.h"
#include "common/cabinetmanagement.h"
#include "common/commondialog.h"
#include "common/commonclass.h"
#include "common/timertask.h"
#include "communication/businessinvoker.h"
#include "faultalarmdialog.h"

#include <commonView/mylistchooicedialog.h>

MainForm2::MainForm2(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::MainForm2)
{
    ui->setupUi(this);

    //设置背景色
    //ui->verticalFrame->setStyleSheet("#verticalFrame{background-color: rgb(6, 24, 64);}");

    ui->frame_1->setStyleSheet("#frame_1{border-image: url(:/images/images/mainForm-1.png);}");
    //ui->label->setStyleSheet("#label{border-image: url(:/images/images/mainForm-2.png);}");
    ui->frame_3->setStyleSheet("#frame_3{border-image: url(:/images/images/mainForm-3.png);}");
    ui->frame_4->setStyleSheet("#frame_4{border-image: url(:/images/images/mainForm-4.png);}");

    this->m_businessRunning = false;
    this->initData();
    this->initUiView();
}

MainForm2::~MainForm2()
{
    this->m_businessRunning=false;
    delete ui;
}

/**
 * @brief initUiView--初始化ui view
 */
void MainForm2::initUiView()
{
    MY_DEBUG;
    QString myString;

    //    myString.append("<p style='margin:0px'>");
    //    myString.append( tr ( "chopp" ) );
    //    myString.append("</p>");

    //    myString.append("<p style='margin:0px'>");
    //    myString.append( QString::fromLocal8Bit("er电压"));
    //    myString.append("</p>");

    //    ui->label_29->setText(myString);

    if(this->m_gridPowerList.size()>0){
        this->m_gridPowerList.clear();
    }

    //初始化界面组件
    this->m_gridPowerList.append(ui->lineEdit);  //电网有功功率
    this->m_gridPowerList.append(ui->lineEdit_2);  //电网无功功率

    this->m_gridPowerList.append(ui->lineEdit_4);  //电网Uab
    this->m_gridPowerList.append(ui->lineEdit_5);  //电网Ubc
    this->m_gridPowerList.append(ui->lineEdit_34); //电网Uca
    this->m_gridPowerList.append(ui->lineEdit_6);  //网侧lga
    this->m_gridPowerList.append(ui->lineEdit_7);  //网侧lgb
    this->m_gridPowerList.append(ui->lineEdit_31);  //网侧lgc
    //网侧桥臂电流
    this->m_gridPowerList.append(ui->lineEdit_8);  //lconva
    this->m_gridPowerList.append(ui->lineEdit_9);  //lconvb
    this->m_gridPowerList.append(ui->lineEdit_10);  //lconvc
    this->m_gridPowerList.append(ui->lineEdit_19);  //lcapa
    this->m_gridPowerList.append(ui->lineEdit_20);  //lcapb
    this->m_gridPowerList.append(ui->lineEdit_32);  //lcapc
    //母线电压和chopper电流
    this->m_gridPowerList.append(ui->lineEdit_11);  //母线正
    this->m_gridPowerList.append(ui->lineEdit_12);  //母线负
    this->m_gridPowerList.append(ui->lineEdit_35);  //母线总
    this->m_gridPowerList.append(ui->lineEdit_13);  //chopper电压正
    this->m_gridPowerList.append(ui->lineEdit_37);  //chopper电压负
    //机侧桥臂电流
    this->m_gridPowerList.append(ui->lineEdit_14);  //lra
    this->m_gridPowerList.append(ui->lineEdit_15);  //lrb
    this->m_gridPowerList.append(ui->lineEdit_16);  //lrc

    this->m_gridPowerList.append(ui->lineEdit_17);  //机侧Uuv
    this->m_gridPowerList.append(ui->lineEdit_18);  //机侧Uvw
    this->m_gridPowerList.append(ui->lineEdit_36);  //机侧Uwu


    this->m_gridPowerList.append(ui->lineEdit_3);  //电机有功功率
    this->m_gridPowerList.append(ui->lineEdit_21);  //电机无功功率
    this->m_gridPowerList.append(ui->lineEdit_22);  //电机转速

    if(this->m_waterCoolList.size()>0){
        this->m_waterCoolList.clear();
    }

    //水冷参数
    this->m_waterCoolList.append(ui->lineEdit_23);  //主水流量
    this->m_waterCoolList.append(ui->lineEdit_24);  //供水湿度
    this->m_waterCoolList.append(ui->lineEdit_25);  //供水压力

    this->m_waterCoolList.append(ui->lineEdit_27);  //回水温度
    this->m_waterCoolList.append(ui->lineEdit_29);  //回水压力
    this->m_waterCoolList.append(ui->lineEdit_28);  //水电导率

    this->m_waterCoolList.append(ui->lineEdit_30);  //阀厅温度
    this->m_waterCoolList.append(ui->lineEdit_26);  //阀厅湿度
    this->m_waterCoolList.append(ui->lineEdit_33);  //三通阀开度
    MY_DEBUG;
}

/**
 * @brief initData--初始化数据
 */
void MainForm2::initData()
{
    //    CabinetManagement *manager = CabinetManagement::instance();
    //    this->m_parameterList = manager->getMainFormParameters();
}

/**
 * @brief startBusiness--开始业务逻辑
 */
void MainForm2::startBusiness()
{

    CabinetManagement *manager = CabinetManagement::instance();
    this->m_parameterList = manager->getMainFormParameters();
    MY_DEBUG << "MainForm startBusiness, parameter size is "<< this->m_parameterList->size();
    if(this->m_parameterList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数文件失败，请检查参数文件"));
        return;
    }

    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &MainForm2::mainForm2QueryResponse); //绑定数据查询响应
    //唤醒数据接收线程
    invoker->wakeup(BusinessClass::MODBUS_HANDLER);

    //接收定时任务
    TimerTask *task = TimerTask::instance();
    connect(task, &TimerTask::taskIsTime, this, &MainForm2::myQueryParameterTask);         //定时任务响应
    this->m_businessRunning=true;
}

/**
 * @brief endBusiness--结束业务逻辑
 */
void MainForm2::endBusiness()
{
    MY_DEBUG << "MainForm endBusiness";
    if(this->m_parameterList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数文件失败，请检查参数文件"));
    }
    this->m_businessRunning=false;
    //解除绑定数据查询响应
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &MainForm2::mainForm2QueryResponse);
    //挂起线程
    invoker->suspend(BusinessClass::MODBUS_HANDLER);
    //解除定时任务绑定
    TimerTask *task = TimerTask::instance();
    disconnect(task, &TimerTask::taskIsTime, this, &MainForm2::myQueryParameterTask);
}

/**
 * @brief MainForm::queryMyParameterTask
 */
void MainForm2::myQueryParameterTask()
{
    if(!this->m_businessRunning)
    {
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        return;
    }
    if(this->m_parameterList->size()==0)
    {
        //解除定时任务绑定
        TimerTask *task = TimerTask::instance();
        disconnect(task, &TimerTask::taskIsTime, this, &MainForm2::myQueryParameterTask);
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数配置文件失败！"));
        return;
    }
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    for(int i=0; i<this->m_parameterList->size(); i++)
    {
        ParameterClass::RegisterData reg = this->m_parameterList->at(i)->getRegisterData();
        reg.rw = 0;
        //        MY_DEBUG << "addr " << reg.addr << "fpgaID " << reg.fpgaID;
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
void MainForm2::mainForm2QueryResponse(BusinessResponse data)
{
    //    MY_DEBUG << "mainFormQueryResponse " << success;
    if(data.type!=BusinessClass::REGISTER_RW){
        return;
    }
    if(!data.result){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("查询数据失败：") + data.errorMsg);
        return;
    }
    int findFault=0;
    bool isFault=false;
    QList<ParameterClass::RegisterData> dataList = data.getRegisterData();
    for (int i=0; i<dataList.size(); i++)
    {
        ParameterClass::RegisterData rData = dataList.at(i);
        if(rData.rw==1){
            switch (rData.addr) {
            case 2401:
            case 522:
                CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("故障复位成功！"));
                break;
            case 434:
                CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("本地/远程控制切换成功！"));
                break;
            default:
                break;
            }
        }
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
            dataShow = QString::number(round(rData.des.floatValue*1000)/1000);
        }
            break;
        }

        //显示到界面
        //1）故障与告警---主页面---故障字(rid=442/2416)/告警字(rid=48)
//        if(rData.addr == 442 || rData.addr == 2416){       //故障
//            //            MY_DEBUG << dataShow << "---" << rData.addr;
//            isFault |= (dataShow!="0");
//            findFault++;
//            if(findFault==2){
//                if(isFault){
//                    ui->fault2->setIcon(QIcon(":/images/images/rad.svg"));
//                }else{
//                    ui->fault2->setIcon(QIcon(":/images/images/green.svg"));
//                }
//            }
//            continue;
//        }
        if(rData.addr == 442){
            if(rData.des.uintValue == 0){
                ui->fault2->setIcon(QIcon(":/images/images/green.svg"));
            }else{
                ui->fault2->setIcon(QIcon(":/images/images/rad.svg"));
            }
            continue;
        }
        if(rData.addr == 48){       //告警
            //MY_DEBUG << dataShow << "---" << rData.addr<< "---"<<rData.des.uintValue;
            if(rData.des.uintValue == 0){
                ui->alarm2->setIcon(QIcon(":/images/images/green.svg"));
            }else{
                ui->alarm2->setIcon(QIcon(":/images/images/rad.svg"));
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

        //常用变量显示
        {
            //主页面电网功率项
            int mainForm[] = {586/*电网有功功率*/, 588/*无功功率*/,
                              /*电网UABC*/
                              574, 576, 578,
                              /*网侧Igabc*/
                              638, 640, 642,
                              /*Iconvabc*/
                              580, 582, 584,
                              /*Icapabc*/
                              598, 600, 602,
                              /*母线正负总*/
                              616, 618, 620,
                              /*chopper正负*/
                              652,654,
                              /*机侧桥臂电流*/
                              832, 834, 836,
                              /*机侧Uuv Uvw Uwu*/
                              826, 828, 830,
                              /*电机 有功 无功 转速*/
                              840,842,844
                             };
            for(int k=0; k<sizeof(mainForm)/sizeof(int); k++){
                if(rData.addr == mainForm[k]){
                    this->m_gridPowerList.at(k)->setText(dataShow);
                    continue;
                }
            }
        }
        //本地远程切换/故障复位
        {
            if(rData.addr == 434){
//                MY_DEBUG << "------" << rData.des.uintValue;
                switch (rData.des.uintValue) {
                //远程控制
                case 0:
                    ui->pushButton_2->setText(QStringLiteral("远程控制"));
                    break;
                    //本地控制
                case 1:
                    ui->pushButton_2->setText(QStringLiteral("本地控制"));
                    break;
                    //测试模式
                case 2:
                    ui->pushButton_2->setText(QStringLiteral("测试模式"));
                    break;
                }
                continue;
            }
        }
        //水冷参数显示
        {
            //指示灯--泵启动/通信
            if(rData.addr == 46 || rData.addr == 108){
                //泵启动--参数等于0--红色 / 参数不等于0--绿色
                if(rData.addr == 46){
                    if(rData.des.uintValue != 0){
                        ui->label_75->setPixmap(QPixmap(":/images/images/green.svg"));
                    }else{
                        ui->label_75->setPixmap(QPixmap(":/images/images/rad.svg"));
                    }
//                    if((rData.des.uintValue & 0x02) == 2){
//                        ui->label_75->setPixmap(QPixmap(":/images/images/green.svg"));
//                    }else{
//                        ui->label_75->setPixmap(QPixmap(":/images/images/rad.svg"));
//                    }
                }
                //通信--参数等于0--红色 / 参数不等于0--绿色
                if(rData.addr == 108){
                    if(rData.des.uintValue != 0){
                        ui->label_74->setPixmap(QPixmap(":/images/images/green.svg"));
                    }else{
                        ui->label_74->setPixmap(QPixmap(":/images/images/rad.svg"));
                    }
                }
                continue;
            }
            //水冷参数
            /*主水流量/供水温度/供水压力/回水温度/回水压力/水电导率/阀厅温度/阀厅湿度/三通阀开度*/
            int warterCoolForm[] = {80, 82,84, 86, 88, 90, 96, 98, 106};
            for(int k=0; k< sizeof(warterCoolForm)/sizeof(int); k++){
                if(rData.addr == warterCoolForm[k]){
                    this->m_waterCoolList.at(k)->setText(dataShow);
                    break;
                }
            }
        }
    }
}

/**
 * @brief MainForm2::on_comboBox_currentIndexChanged--本地/远程切换
 * @param index--0/1 本地/远程
 */
//void MainForm2::on_comboBox_currentIndexChanged(int index)
//{
//    MY_DEBUG << index;
//    if(isMySetControlMode){
//        isMySetControlMode=false;
//        MY_DEBUG << isMySetControlMode;
//        return;
//    }
//    //些控制寄存器
//    //Para AddrOffset218 = 数值
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
//        return;
//    }
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId==434){
//            BusinessClass bc;
//            bc.setLevel(BusinessClass::MODBUS_HANDLER);

//            ParameterClass::RegisterData reg = p->getRegisterData();
//            reg.rw = 1;
//            MY_DEBUG << "write reg is " << index;
//            reg.src.uintValue = index;
//            bc.addData(reg);
//            invoker->exec(bc);

//            break;
//        }
//    }
//}

/**
 * @brief MainForm2::on_pushButton_clicked--故障复位点击
 */
void MainForm2::on_pushButton_clicked()
{
//    MY_DEBUG;
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
//        return;
//    }

//    BusinessClass bc;
//    bc.setLevel(BusinessClass::MODBUS_HANDLER);
//    //故障复位（2401）/故障复位（522）
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId==2401){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            reg.rw = 1;
//            if(this->isFaultReset){     //当前处于故障复位状态--弹起操作
//                reg.src.ushortValue = 0;
//            }else{                      //当前未处于故障复位状态--按下操作
//                reg.src.ushortValue = 1;
//            }

//            bc.addData(reg);
//        }
//        if(p->registerId==522){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            reg.rw = 1;
//            if(this->isFaultReset){     //当前处于故障复位状态--弹起操作
//                reg.src.uintValue = 0;
//            }else{                      //当前未处于故障复位状态--按下操作
//                reg.src.uintValue = 1;
//            }
//            bc.addData(reg);
//        }
//    }
//    if(bc.getData().size()!=2){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
//        return;
//    }
//    invoker->exec(bc);
//    this->isFaultReset=!this->isFaultReset;
//    if(this->isFaultReset){
//        ui->pushButton->setIcon(QIcon(":/images/images/reset_on.png"));
//    }else{
//        ui->pushButton->setIcon(QIcon(":/images/images/reset_off.png"));
//    }
}

/**
 * @brief MainForm2::on_pushButton_pressed--故障复位按下--//故障复位（2401）/故障复位（522）
 */
void MainForm2::on_pushButton_pressed()
{
    MY_DEBUG;
    if(this->m_parameterList->size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
        return;
    }
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    for(int i=0; i<this->m_parameterList->size(); i++){
        ParameterClass *p = this->m_parameterList->at(i);
        if(p->registerId==2401){
            ParameterClass::RegisterData reg = p->getRegisterData();
            reg.rw = 1;
            reg.src.ushortValue = 1;
            bc.addData(reg);

        }
        if(p->registerId==522){
            ParameterClass::RegisterData reg = p->getRegisterData();
            reg.rw = 1;
            reg.src.uintValue = 1;
            bc.addData(reg);

        }
    }
    if(bc.getData().size()!=2){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
        return;
    }
    invoker->exec(bc);
}

/**
 * @brief MainForm2::on_pushButton_released--故障复位弹起
 */
void MainForm2::on_pushButton_released()
{
    MY_DEBUG;
//    if(this->m_parameterList->size()==0){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
//        return;
//    }
//    BusinessInvoker *invoker = BusinessInvoker::instance();
//    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
//        return;
//    }
//    BusinessClass bc;
//    bc.setLevel(BusinessClass::MODBUS_HANDLER);
//    for(int i=0; i<this->m_parameterList->size(); i++){
//        ParameterClass *p = this->m_parameterList->at(i);
//        if(p->registerId==2401){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            reg.rw = 1;
//            reg.src.ushortValue = 0;
//            bc.addData(reg);

//        }
//        if(p->registerId==522){
//            ParameterClass::RegisterData reg = p->getRegisterData();
//            reg.rw = 1;
//            reg.src.uintValue = 0;
//            bc.addData(reg);

//        }
//    }
//    if(bc.getData().size()!=2){
//        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
//        return;
//    }
//    invoker->exec(bc);
}

/**
 * @brief MainForm2::on_alarm2_clicked--告警点击，显示告警对话框
 */
void MainForm2::on_alarm2_clicked()
{
    this->endBusiness();
    CabinetManagement *manager = CabinetManagement::instance();
    FaultAlarmDialog dialog;
    dialog.setWindowTitle(QString::fromLocal8Bit("告警查看界面"));
    dialog.setShowData(*(manager->getAlarmFormParameters()));
    dialog.exec();
    this->startBusiness();
}

/**
 * @brief MainForm2::on_fault2_clicked--故障点击，显示故障对话框
 */
void MainForm2::on_fault2_clicked()
{
    this->endBusiness();
    CabinetManagement *manager = CabinetManagement::instance();
    FaultAlarmDialog dialog;
    dialog.setWindowTitle(QString::fromLocal8Bit("故障查看界面"));
    dialog.setShowData(*(manager->getFaultFormParameters()));
    dialog.exec();
    this->startBusiness();
}

/**
 * @brief MainForm2::on_pushButton_2_clicked--远程控制
 */
void MainForm2::on_pushButton_2_clicked()
{
    MyListChooiceDialog* dialog = new MyListChooiceDialog(this);
    QStringList contents;
    contents.append(QStringLiteral("远程控制"));
    contents.append(QStringLiteral("本地控制"));
    contents.append(QStringLiteral("测试模式"));
    dialog->setItems(contents);
    connect(dialog, &MyListChooiceDialog::listItemChooice, this, &MainForm2::on_controlUpdate);
    dialog->showNormal();
}

void MainForm2::on_controlUpdate(int index){
        //些控制寄存器
        //Para AddrOffset218 = 数值
        if(this->m_parameterList->size()==0){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数文件读取有误，未获取有效参数！"));
            return;
        }
        BusinessInvoker *invoker = BusinessInvoker::instance();
        if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败，请检测网络！"));
            return;
        }
        BusinessClass bc;
        bc.setLevel(BusinessClass::MODBUS_HANDLER);
        for(int i=0; i<this->m_parameterList->size(); i++){
            ParameterClass *p = this->m_parameterList->at(i);
            if(p->registerId==434){
                ParameterClass::RegisterData reg = p->getRegisterData();
                reg.rw = 1;
                reg.src.uintValue = index;
                MY_DEBUG << "write 434 reg is " << reg.src.uintValue << "  fpga is " << reg.fpgaID;
                bc.addData(reg);
                break;
            }
        }
        invoker->exec(bc);
}
