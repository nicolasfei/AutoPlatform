#include "communicationset10form.h"
#include "ui_communicationset10form.h"
#include "common/devicemanager.h"
#include "common/cabinetmanagement.h"
#include "common/commondialog.h"
#include "common/tool.h"
#include "communication/businessinvoker.h"
#include "communication/businessclass.h"

#include <QFileDialog>
#include <QStandardPaths>

CommunicationSet10Form::CommunicationSet10Form(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::CommunicationSet10Form)
{

    ui->setupUi(this);
    this->initUiView();

    ui->ftpRootDir->setStyleSheet("text-align: left;");
    //隐藏端口号
    //    ui->label_3->setVisible(false);
    //    ui->label_4->setVisible(false);
    //    ui->lineEdit_2->setVisible(false);
    //    ui->lineEdit_3->setVisible(false);
    //    ui->label_13->setVisible(false);
    //    ui->lineEdit_11->setVisible(false);
}

CommunicationSet10Form::~CommunicationSet10Form()
{
    delete ui;
}

/**
 * @brief CommunicationSet10Form::initUiView--初始化ui view
 */
void CommunicationSet10Form::initUiView()
{
    //设置样式
    ui->gridFrame_2->setStyleSheet("#gridFrame_2{border-image: url(:/images/images/communica_g.png);}");
    ui->gridFrame->setStyleSheet("#gridFrame{border-image: url(:/images/images/communica_g.png);}");
}

/**
 * @brief CommunicationSet10Form::initData -- 初始化数据
 */
void CommunicationSet10Form::initData()
{

}

/**
 * @brief CommunicationSet10Form::startBusiness -- 开始业务逻辑
 */
void CommunicationSet10Form::startBusiness()
{
    CabinetManagement *manager = CabinetManagement::instance();
    //网络配置数据结构
    CabinetManagement::CabinetDevice::Net_config *net = manager->getNetConfig(manager->getCurrentCabinetID());

    if(net==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取网络参数失败，请检查参数文件"));
        return;
    }
    //显示主控ip设置
    QString armIP = net->m_masterIP;
    if(!armIP.isEmpty()){
        ui->lineEdit->setText(armIP);
    }
    QString mask = net->m_masterMASK;
    if(!mask.isEmpty()){
        ui->lineEdit_8->setText(mask);
    }
    QString gate = net->m_masterGATEWAY;
    if(!gate.isEmpty()){
        ui->lineEdit_9->setText(gate);
    }
    int armPort = net->m_masterModbusPort;
    if(armPort>0){
        ui->lineEdit_2->setText(QString::number(armPort));
    }
    int armTcpPort = net->m_masterTcpPort;
    if(armTcpPort>0){
        ui->lineEdit_3->setText(QString::number(armTcpPort));
    }

    //显示ftp配置
    QString ip = net->m_ftpIP;
    if(!ip.isEmpty()){
        ui->lineEdit_10->setText(ip);
    }
    int port = net->m_ftpPort;
    if(port>0){
        ui->lineEdit_11->setText(QString::number(port));
    }
    QString user = net->m_ftpUser;
    if(!user.isEmpty()){
        ui->lineEdit_5->setText(user);
    }
    QString login = net->m_ftpLogin;
    if(!login.isEmpty()){
        ui->lineEdit_6->setText(login);
    }
    QString path = net->m_ftpRootDir;
    if(!path.isEmpty()){
        ui->ftpRootDir->setText(path);
    }

    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &CommunicationSet10Form::on_queryDataResponse);       //绑定数据查询响应
    //唤醒modbus线程
    invoker->wakeup(BusinessClass::MODBUS_HANDLER);
}

/**
 * @brief CommunicationSet10Form::endBusiness -- 结束业务逻辑
 */
void CommunicationSet10Form::endBusiness()
{
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &CommunicationSet10Form::on_queryDataResponse);       //绑定数据查询响应
    //挂起数据接收线程
    invoker->suspend(BusinessClass::MODBUS_HANDLER);
}

/**
 * @brief on_queryDataResponse
 * @param success
 * @param data
 * @param errorID
 * @param errorMsg
 */
void CommunicationSet10Form::on_queryDataResponse(BusinessResponse data)
{
    if(data.type!=BusinessClass::REGISTER_RW){
        MY_DEBUG << "this is not net config register response";
        return;
    }
    CabinetManagement *manager = CabinetManagement::instance();
    QList<ParameterClass*>* config = manager->getLmNetConfig();
    QList<ParameterClass::RegisterData> regs = data.getRegisterData();
    if(regs.size()!=config->size()){
        MY_DEBUG << "this is not net config register response";
        return;
    }
    bool ok=true;
    for(int i=0; i<config->size();i++){
        ParameterClass* pc = config->at(i);
        if(pc->regData.addr!=regs.at(i).addr){
            ok=false;
            break;
        }
    }
    if(!ok){
        MY_DEBUG << "this is not net config register response";
        return;
    }
    if(!data.result){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("Modbus读写失败：")+ data.errorMsg);
    }else{
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("配置成功")+ data.errorMsg);
    }
}

/**
 * @brief CommunicationSet10Form::on_pushButton_2_clicked--确认修改通信地址
 */
void CommunicationSet10Form::on_pushButton_2_clicked()
{
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败"));
        return;
    }

    bool toint=true;
    CabinetManagement *manager = CabinetManagement::instance();
    CabinetManagement::CabinetDevice::Net_config *net = manager->getNetConfig(manager->getCurrentCabinetID());
    QString ip = ui->lineEdit->text().trimmed();
    if(ip.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入IP地址"));
        return;
    }
    if(!Tool::isIpAddrLegal(ip)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("IP地址错误,请检查"));
        return;
    }
    QString mask = ui->lineEdit_8->text().trimmed();
    if(mask.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入地址掩码"));
        return;
    }
    if(!Tool::isIpAddrLegal(mask)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("IP地址掩码错误,请检查"));
        return;
    }
    QString gateway = ui->lineEdit_9->text().trimmed();
    if(gateway.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入网关地址"));
        return;
    }
    if(!Tool::isIpAddrLegal(gateway)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网关地址错误,请检查"));
        return;
    }
    int port = ui->lineEdit_2->text().trimmed().toInt(&toint);
    if(!toint || port<=0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("Modbus端口号错误,请检查"));
        return;
    }
    int tcpPort = ui->lineEdit_3->text().trimmed().toInt(&toint);
    if(!toint || tcpPort<=0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("动态示波器端口号错误,请检查"));
        return;
    }

//        if(ip != net->m_masterIP || mask != net->m_masterMASK || gateway != net->m_masterGATEWAY ||
//                port != net->m_masterModbusPort || tcpPort != net->m_masterTcpPort){
    //下发到下位机
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络未连接"));
        return;
    }
    QList<ParameterClass*>* lmList = manager->getLmNetConfig();
    if(lmList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取下位机网络设置modbus地址失败"));
        return;
    }
    bool ok=false;
    uint32_t netBuf[3]={0};

    //ip地址
    QStringList ipList = ip.split(".");
    uint32_t ipValue =0;
    for (int k=0; k<ipList.size(); k++) {
        uint8_t tmp = (uint8_t)(ipList.at(k).trimmed().toUInt(&ok));
        if(!ok){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("ip配置错误"));
            return;
        }
        ipValue = (ipValue << 8) | tmp;
    }
    netBuf[0] = ipValue;

    //ip地址掩码
    QStringList maskList = mask.split(".");
    uint32_t maskValue =0;
    for (int k=0; k<maskList.size(); k++) {
        uint8_t tmp = (uint8_t)maskList.at(k).trimmed().toUInt(&ok);
        if(!ok){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("ip掩码配置错误"));
            return;
        }
        maskValue = (maskValue << 8) | tmp;
    }
    netBuf[1] = maskValue;

    //ip网关
    QStringList gateList = gateway.split(".");
    uint32_t gateValue =0;
    for (int k=0; k<gateList.size(); k++) {
        uint8_t tmp = (uint8_t)gateList.at(k).trimmed().toUInt(&ok);
        if(!ok){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("ip网关配置错误"));
            return;
        }
        gateValue = (gateValue << 8) | tmp;
    }
    netBuf[2] = gateValue;

    //先下发ip gate mask配置到下位机
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    for(int i=0; i<sizeof(netBuf)/sizeof(uint32_t); i++){
        ParameterClass *pc = lmList->at(i);
        ParameterClass::RegisterData reg = pc->getRegisterData();
        reg.rw = 1;
        reg.src.uintValue = netBuf[i];
        bc.addData(reg);
    }
    invoker->exec(bc);

    //下位机保存参数到文件
    if(!this->saveLowConfig()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("主控设备参数保存到文件失败"));
        return;
    }

    //保存本地文件
    net->m_masterIP = ip;
    net->m_masterMASK = mask;
    net->m_masterGATEWAY = gateway;
    net->m_masterModbusPort = port;
    net->m_masterTcpPort = tcpPort;
    manager->updateDeviceNetConfigFile(manager->getCurrentCabinetID());
    //修改成功是否重启主控设备
    resetMasterDevice();
//        }else{
//            CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("参数未改变"));
//        }
}

/**
 * @brief CommunicationSet10Form::on_pushButton_clicked--更新ftp服务器配置
 */
void CommunicationSet10Form::on_pushButton_clicked()
{
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败"));
        return;
    }

    bool toint=true;
    CabinetManagement *manager = CabinetManagement::instance();
    CabinetManagement::CabinetDevice::Net_config *net = manager->getNetConfig(manager->getCurrentCabinetID());

    QString ip = ui->lineEdit_10->text().trimmed();
    if(ip.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入IP地址"));
        return;
    }
    if(!Tool::isIpAddrLegal(ip)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("IP地址错误,请检查"));
        return;
    }
    int ftpPort = ui->lineEdit_11->text().trimmed().toInt(&toint);
    if(!toint || ftpPort<=0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("ftp端口号错误,请检查"));
        return;
    }
    QString user = ui->lineEdit_5->text().trimmed();
    if(user.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入用户名"));
        return;
    }
    QString login = ui->lineEdit_6->text().trimmed();
    if(login.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入密码"));
        return;
    }
    QString path = ui->ftpRootDir->text().trimmed();
    if(path.isEmpty()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("请输入根目录路径"));
        return;
    }

//        if(ip != config->m_ip || ftpPort != config->m_masterFtpPort ||
//                user != config->user || login != config->login || path != config->root){
    //下发到下位机
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络未连接"));
        return;
    }
    //先下发保存指令
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    QList<ParameterClass*>* lmList = manager->getLmNetConfig();
    if(lmList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取下位机网络设置modbus地址失败"));
        return;
    }
    bool ok=false;
    //ftp ip地址 下发到下位机
    QStringList ipList = ip.split(".");
    uint32_t ipValue =0;
    for (int k=0; k<ipList.size(); k++) {
        uint8_t tmp = (uint8_t)ipList.at(k).trimmed().toUInt(&ok);
        if(!ok){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("ip配置错误"));
            return;
        }
        ipValue = (ipValue << 8) | tmp;
    }
    ParameterClass *pc = lmList->at(lmList->size()-1);
    ParameterClass::RegisterData reg = pc->getRegisterData();
    reg.rw = 1;
    reg.src.uintValue = ipValue;
    bc.addData(reg);
    invoker->exec(bc);

    //下位机保存参数到文件
    if(!this->saveLowConfig()){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("主控设备参数保存到文件失败"));
        return;
    }

    //本地保存
    net->m_ftpIP = ip;
    net->m_ftpPort = ftpPort;
    net->m_ftpUser = user;
    net->m_ftpLogin = login;
    net->m_ftpRootDir = path;
    manager->updateDeviceFtpConfigFile(manager->getCurrentCabinetID());
    //CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("修改成功"));
    //修改成功是否重启主控设备
    resetMasterDevice();
//        }else{
//            CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("参数未改变"));
//        }
}

/**
 * @brief CommunicationSet10Form::on_pushButton_3_clicked--取消ftp服务器配置更新
 */
void CommunicationSet10Form::on_pushButton_3_clicked()
{
    //    this->startBusiness();
}

/**
 * @brief CommunicationSet10Form::on_pushButton_4_clicked--通信设置取消
 */
void CommunicationSet10Form::on_pushButton_4_clicked()
{
    //    this->startBusiness();
}

/**
 * @brief CommunicationSet10Form::saveLowConfig--下位机主控保存参数到文件
 * @return
 */
bool CommunicationSet10Form::saveLowConfig()
{
    //下发给下位机保存参数到文件指令
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络未连接"));
        return false;
    }
    CabinetManagement *manager = CabinetManagement::instance();
    BusinessClass saveBC;
    saveBC.setLevel(BusinessClass::MODBUS_HANDLER);
    ParameterClass *savePC = manager->getSaveLowerMachineREG();
    if(savePC==Q_NULLPTR){
        CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("获取下位机参数保存modbus地址失败"));
        return false;
    }
    ParameterClass::RegisterData saveReg = savePC->getRegisterData();
    saveReg.rw = 1;
    saveReg.src.uintValue = 1;
    saveBC.addData(saveReg);
    invoker->exec(saveBC);
    return true;
}

/**
 * @brief CommunicationSet10Form::resetMasterDevice--重启主控设备
 */
void CommunicationSet10Form::resetMasterDevice(){
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败"));
        return;
    }
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromLocal8Bit("重启主控设备"));
    msgBox.setText(QString::fromLocal8Bit("网络配置成功，重启主控设备后网络配置生效，同时请保证设备工作已经完成，重启设备不会造成故障！"));
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
 * @brief CommunicationSet10Form::on_ftpRootDir_clicked--选择ftp根目录
 */
void CommunicationSet10Form::on_ftpRootDir_clicked()
{
    //默认打开我的文档路径
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString title = QString::fromLocal8Bit("请选择FTP根目录");
    QString dirpath = QFileDialog::getExistingDirectory(this, title, dir, QFileDialog::ShowDirsOnly);
    if(!dirpath.isEmpty()){
        ui->ftpRootDir->setText(dirpath);
    }
}

