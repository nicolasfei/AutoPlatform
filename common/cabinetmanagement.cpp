#include "cabinetmanagement.h"
#include "tool.h"
#include "common/commonclass.h"
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <QStandardPaths>

#define CONFIG_DIR  "/config"
#define CONFIG_BAK_DIR  "/configBak"
#define NET_CONFIG_FILE  "/netConfig"
#define NET_CONFIG_BAK_FILE  "/netConfigBak"

#define OSCILL_DATA_DIR "/ocsill"
#define WAVE_DATA_DIR "/wave"

#define FTP_CONFIG "/ftpConfig"
#define FTP_CONFIG_BAK_FILE  "/ftpConfigBak"

#define WAVE_CONFIG "/waveChannelConfig"
#define WAVE_CONFIG_BAK_FILE "/waveChannelConfigbak"

#define DESCRIBE_DIR "/describe"

CabinetManagement::CabinetManagement(QObject *parent) : QObject(parent)
{
    bool copyOk=false;
    /**
     * 需要判断是否已经copy一份原始配置文件
     **/
    {
        QDir original(QDir::currentPath()+ CONFIG_DIR);
        if(original.exists()){
            MY_DEBUG;
            for(int i=0; i<2; i++){
                QDir copyDir((QDir::currentPath()+CONFIG_BAK_DIR + QString::number(i)));
                if(!copyDir.exists()){
                    copyOk = Tool::copyDirectoryFiles((QDir::currentPath()+ CONFIG_DIR),
                                                      (QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(i)),true);
                    MY_DEBUG << copyOk;
                }else{
                    copyOk=true;
                }
            }
        }
    }

    //ftp配置文件--如果没有则创建
    {
        QFile ftpConfig(QDir::currentPath() + FTP_CONFIG);
        if(!ftpConfig.exists()){
            if(ftpConfig.open(QFile::WriteOnly | QFile::Truncate)){
                QTextStream out(&ftpConfig);//文本流
                out.setCodec(QTextCodec::codecForName("UTF-8"));
                out << "root"  << endl;              //ftp user
                out << "123456"  << endl;              //ftp login
                //默认打开我的文档路径
                QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
                out << dir  << endl;             //ftp root path
                out.flush();
                ftpConfig.close();
            }
        }
        if(ftpConfig.exists()){
            MY_DEBUG;
            for(int i=0; i<2; i++){
                if(!QFile::exists(QDir::currentPath()+FTP_CONFIG_BAK_FILE + QString::number(i))){
                    copyOk = Tool::copyFileToPath(QDir::currentPath()+FTP_CONFIG,
                                                  QDir::currentPath()+FTP_CONFIG_BAK_FILE + QString::number(i),true);
                    MY_DEBUG << copyOk;
                }else{
                    copyOk=true;
                }
            }
        }
    }

    //故障录波通道名字文件--如果没有则创建
    {
        QFile waveChannelConfig(QDir::currentPath() + WAVE_CONFIG);
        if(!waveChannelConfig.exists()){
            if(waveChannelConfig.open(QFile::WriteOnly | QFile::Truncate)){
                QTextStream out(&waveChannelConfig);//文本流
                out.setCodec(QTextCodec::codecForName("UTF-8"));
                for(int k=0; k<64; k++){
                    QString id = QString::number(k);
                    out << id << ":channel"+ id <<endl;
                }
                out.flush();
                waveChannelConfig.close();
            }
        }
        if(waveChannelConfig.exists()){
            MY_DEBUG;
            for(int i=0; i<2; i++){
                if(!QFile::exists(QDir::currentPath()+WAVE_CONFIG_BAK_FILE + QString::number(i))){
                    copyOk = Tool::copyFileToPath(QDir::currentPath()+WAVE_CONFIG,
                                                  QDir::currentPath()+WAVE_CONFIG_BAK_FILE + QString::number(i),true);
                    MY_DEBUG << copyOk;
                }else{
                    copyOk=true;
                }
            }
        }
    }

    if(copyOk){
        /**
         * 读取配置文件数据
         **/
        MY_DEBUG;
        for(int i=0; i<2; i++)
        {
            CabinetDevice *device = new CabinetDevice(i);

            //初始化设备
            {
                QString fileDirPath = QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(i);
                QDir dir(fileDirPath);
                dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);  //QDir::NoDotAndDotDot 不列出.文件，即指向当前目录的软链接
                dir.setSorting(QDir::Name | QDir::Reversed);

                QFileInfoList list = dir.entryInfoList();
                for (int k = 0; k < list.size(); ++k)
                {
                    QFileInfo fileInfo = list.at(k);
                    if(fileInfo.isFile()){
                        //配置文件
                        QString name = fileInfo.baseName();
                        //读取参数文件
                        ParameterTable *table = this->readParameterFile(device, fileInfo.absoluteFilePath(), name);
                        MY_DEBUG << "device "<< k << "main para size is " << device->m_mainFormParameterList->size();
                        if(table!=NULL && !table->isHaveError){
                            device->m_fileConfig->insert(fileInfo.baseName(), table);
                            //检查table是否有加入自定义参数列表和示波器的参数
                            QMap<QString, QList<ParameterClass*>*>::iterator iter = table->tableMap->begin();
                            while (iter != table->tableMap->end())
                            {
                                QList<ParameterClass*>* list = iter.value();
                                for(int j=0; j<list->size(); j++)
                                {
                                    ParameterClass* pc = list->at(j);
                                    if(pc->isOsc)
                                    {
                                        device->m_oscilloscopeList->append(pc);
                                    }
                                    if(pc->isAddCustom())
                                    {
                                        device->m_customList->append(pc);
                                    }
                                }
                                iter++;
                            }
                        }
                    }
                }
            }

            //ftp配置信息
            {
                QString ftpPath = QDir::currentPath()+ FTP_CONFIG_BAK_FILE + QString::number(i);
                QFile ftpcofig(ftpPath);
                if(ftpcofig.open(QIODevice::ReadOnly)){
                    QTextStream * out = new QTextStream(&ftpcofig);//文本流
                    out->setCodec(QTextCodec::codecForName("UTF-8"));

                    //FTP 用户名
                    QString userLine = out->readLine().trimmed();
                    if(!userLine.isEmpty()){
                        device->netConfig->m_ftpUser = userLine;
                    }
                    //FTP 密码
                    QString loginLine = out->readLine().trimmed();
                    if(!loginLine.isEmpty()){
                        device->netConfig->m_ftpLogin = loginLine;
                    }
                    //FTP 根目录
                    QString rootPathLine = out->readLine().trimmed();
                    if(!rootPathLine.isEmpty()){
                        if(rootPathLine.at(rootPathLine.size()-1) == '/'){
                            rootPathLine = rootPathLine.mid(0, rootPathLine.size()-1);
                        }
                        device->netConfig->m_ftpRootDir = rootPathLine;
                        //创建故障录波上传文件夹
                        QString faultUpload = device->netConfig->m_ftpRootDir + FAULT_DIR + QString::number(i);
                        QDir faultUploadDir(faultUpload);
                        if(!faultUploadDir.exists()){
                            if(!faultUploadDir.mkdir(faultUpload)){
                                MY_DEBUG << "make dir " << faultUpload << "fault";
                            }
                        }
                        device->netConfig->m_ftpFaultUploadDir = faultUpload;   //保存故障录波上传目录
                        //创建参数文件下载目录
                        QString parameterDownload = device->netConfig->m_ftpRootDir + PARAMETER_DIR + QString::number(i);
                        QDir parameterDownloadDir(parameterDownload);
                        if(!parameterDownloadDir.exists()){
                            if(!parameterDownloadDir.mkdir(parameterDownload)){
                                MY_DEBUG << "make dir " << parameterDownload << "fault";
                            }
                        }
                        device->netConfig->m_ftpParameterDownloadDir = parameterDownload;   //保存参数文件下载目录
                    }
                    ftpcofig.close();
                }
            }

            //故障录波通道名
            {
                QString waveChannelConfigPath = QDir::currentPath() + WAVE_CONFIG_BAK_FILE + QString::number(i);
                QFile wavaChannelCofig(waveChannelConfigPath);
                if(wavaChannelCofig.open(QIODevice::ReadOnly)){
                    QTextStream * out = new QTextStream(&wavaChannelCofig);//文本流
                    out->setCodec(QTextCodec::codecForName("UTF-8"));
                    while(1){
                        QString tmp = out->readLine();
                        if(tmp.isEmpty()){
                            break;
                        }
                        device->m_waveChannelNameList->append(tmp);
                    }
                    wavaChannelCofig.close();
                }
            }

            //示波器数据保存目录
            {
                QDir oscillDataDir((QDir::currentPath()+ OSCILL_DATA_DIR + QString::number(i)));
                if(!oscillDataDir.exists()){
                    oscillDataDir.mkdir(oscillDataDir.absolutePath());
                }
                device->m_oscillDataDir = oscillDataDir.absolutePath();
            }
            //故障录波数据保存目录
            {
                QDir waveDataDir((QDir::currentPath()+ WAVE_DATA_DIR + QString::number(i)));
                if(!waveDataDir.exists()){
                    waveDataDir.mkdir(waveDataDir.absolutePath());
                }
                device->m_waveDataDir = waveDataDir.absolutePath();
            }
            //参数配置文件项bit位描述文件夹
            {
                QDir describe((QDir::currentPath()+ DESCRIBE_DIR + QString::number(i)));
                if(!describe.exists()){
                    describe.mkdir(describe.absolutePath());
                }
                device->m_describeDir = describe.absolutePath();
            }
            //加入设备List
            this->m_cabinetList.append(device);
        }
    }

    //默认当前机柜为0
    this->currentCabinetID=0;
}

CabinetManagement::~CabinetManagement()
{
    for(int i=0; i<this->m_cabinetList.size(); i++){
        delete this->m_cabinetList.at(i);
    }
}

ParameterTable * CabinetManagement::readParameterFile(CabinetDevice *device, QString file, QString fileName)
{
    MY_DEBUG;
    ParameterTable *table;
//    MY_DEBUG << "MAIN SIZE IS " << device->m_mainFormParameterList->size();
    //加载mastar文件
    if(fileName == "master")
    {
        MY_DEBUG << "MAIN SIZE IS " << device->m_mainFormParameterList->size();
        /**
         * 1.主控数据表的AddrOffset偏移地址值为1200;WR寄存器ID偏移地址：2400,R寄存器ID偏移地址：0;FPGA（上位机WR）偏移地址:2400，FPGA（上位机R）偏移地址:2616。
         * 2.褐色条目：寄存器ID为用于modbus通信所用的寄存器ID，FPGA为FPGA的变量地址编号
         * 3.主控数据表在软件设计上预留403个变量，根据读写属性，预留200个读写变量，203个只读变量,共计520个变量
         */
        table = new ParameterTable(1200,2400,0,2400,2616,file,currentCabinetID);
        for(int k=0; k<table->m_list->size(); k++){
            ParameterClass* tmp = table->m_list->at(k);
            //主页面---/故障复位（2401）
            if(tmp->registerId == 2416 || tmp->registerId == 2401)
            {
                for(int m=0; m<device->m_mainFormParameterList->size(); m++){
                    ParameterClass* t = device->m_mainFormParameterList->at(m);
                    if(t->registerId == tmp->registerId){
                        device->m_mainFormParameterList->removeAt(m);
                    }
                }
                //主页面故障复位--Master AddrOffset1320 / Para AddrOffset262
                device->m_mainFormParameterList->append(tmp);
            }

            //故障查看界面所需的参数
            if(tmp->registerId>=2418 && tmp->registerId<=2423)
            {
                for(int m=0; m<device->m_faultParameterList->size(); m++){
                    ParameterClass* t = device->m_faultParameterList->at(m);
                    if(t->registerId == tmp->registerId){
                        device->m_faultParameterList->removeAt(m);
                    }
                }
                device->m_faultParameterList->append(tmp);
            }
            //告警查看界面
            if(tmp->registerId>=45 && tmp->registerId<=52)
            {
                for(int m=0; m<device->m_alarmParameterList->size(); m++){
                    ParameterClass* t = device->m_alarmParameterList->at(m);
                    if(t->registerId == tmp->registerId){
                        device->m_alarmParameterList->removeAt(m);
                    }
                }
                device->m_alarmParameterList->append(tmp);
            }
        }
        MY_DEBUG << "MAIN SIZE IS " << device->m_mainFormParameterList->size();
        MY_DEBUG;
    }
    else if(fileName == "para")
    {       //加载para参数
        MY_DEBUG;
        MY_DEBUG << "MAIN SIZE IS " << device->m_mainFormParameterList->size();
        /**
         * 1.para表的AddrOffset偏移地址值为0；WR寄存器ID(03功能码)、FPGA偏移地址均为0。
           2.褐色条目：寄存器ID为用于modbus通信所用的寄存器ID，FPGA为FPGA的变量地址编号
           3.para在软件设计上预留200个变量，共计1200个变量
         */
        table = new ParameterTable(0,0,0,0,0,file,currentCabinetID);
        if(table!=NULL && !table->isHaveError){

            //            device->m_lmNetConfig->clear();
            //            device->m_doFormParameterList->clear();
            //            device->m_waterFormParameterList->clear();

            for(int k=0; k<table->m_list->size(); k++){
                ParameterClass* tmp = table->m_list->at(k);
                //主页面--告警字(rid=48)/故障复位/(本地/远程控制切换)
                if(tmp->registerId == 48 || tmp->registerId == 522 || tmp->registerId == 434)
                {
                    for(int m=0; m<device->m_mainFormParameterList->size(); m++){
                        ParameterClass* t = device->m_mainFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_mainFormParameterList->removeAt(m);
                        }
                    }
                    //主页面故障复位--Master AddrOffset1320 / Para AddrOffset262
                    device->m_mainFormParameterList->append(tmp);
                }
                //主页面水冷参数
                //1)泵启动/通信
                if(tmp->registerId == 46 || tmp->registerId == 108 )
                {
                    for(int m=0; m<device->m_mainFormParameterList->size(); m++){
                        ParameterClass* t = device->m_mainFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_mainFormParameterList->removeAt(m);
                        }
                    }
                    device->m_mainFormParameterList->append(tmp);
                }
                //2)主水流量/供水温度/供水压力/回水温度/回水压力/水电导率/阀厅温度/阀厅湿度/三通阀开度
                if(tmp->registerId==80 || tmp->registerId==82 || tmp->registerId==84 ||
                        tmp->registerId==86 || tmp->registerId==88 || tmp->registerId==90 ||
                        tmp->registerId==96 || tmp->registerId==98 ||tmp->registerId==106)
                {
                    for(int m=0; m<device->m_mainFormParameterList->size(); m++){
                        ParameterClass* t = device->m_mainFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_mainFormParameterList->removeAt(m);
                        }
                    }
                    device->m_mainFormParameterList->append(tmp);
                }

                //水冷页面参数
                if(tmp->registerId>=80 && tmp->registerId<=118){
                    for(int m=0; m<device->m_waterFormParameterList->size(); m++){
                        ParameterClass* t = device->m_waterFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_waterFormParameterList->removeAt(m);
                        }
                    }
                    device->m_waterFormParameterList->append(tmp);
                }

                //调试页面所需参数
                //1）模式下拉菜单
                if(tmp->registerId==524 || tmp->registerId==526 || tmp->registerId==544 ||
                        tmp->registerId==546 || tmp->registerId==574){
                    for(int m=0; m<device->m_systemFormParameterList->size(); m++){
                        ParameterClass* t = device->m_systemFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_systemFormParameterList->removeAt(m);
                        }
                    }
                    device->m_systemFormParameterList->append(tmp);
                }
                //2）按钮控制
                if(tmp->registerId==556 || tmp->registerId==558 || tmp->registerId==562 ||
                        tmp->registerId==564 || tmp->registerId==566){
                    for(int m=0; m<device->m_systemFormParameterList->size(); m++){
                        ParameterClass* t = device->m_systemFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_systemFormParameterList->removeAt(m);
                        }
                    }
                    device->m_systemFormParameterList->append(tmp);
                }
                //3）参数设置
                /*机侧转矩给定 机侧转速给定 直流电压给定 网侧无功给定 d轴电流给定 q轴电流给定 PWM d轴给定 PWM q轴给定*/
                if(tmp->registerId==528 || tmp->registerId==530 || tmp->registerId==532 || tmp->registerId==536 ||
                        tmp->registerId==548 || tmp->registerId==550 || tmp->registerId==552 || tmp->registerId==554){
                    for(int m=0; m<device->m_systemFormParameterList->size(); m++){
                        ParameterClass* t = device->m_systemFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_systemFormParameterList->removeAt(m);
                        }
                    }
                    device->m_systemFormParameterList->append(tmp);
                }


                //下位机IP地址设置寄存器--ip,掩码,网关,qt FTP服务器地址
                if(tmp->registerId == 1930 || tmp->registerId == 1932 || tmp->registerId == 1934 || tmp->registerId == 1948){
                    device->m_lmNetConfig->append(tmp);
                    //下位机IP地址
                    if(tmp->registerId == 1930){
                        uint32_t value = tmp->m_dataValue.uintValue;
                        QString valueS = QString::number((value>>24&0xFF)) + "." +QString::number((value>>16&0xFF))+ "." +
                                QString::number((value>>8&0xFF)) + "." + QString::number((value>>0&0xFF));
                        device->netConfig->m_masterIP = valueS;
                    }
                    //下位机IP掩码
                    if(tmp->registerId == 1932){
                        uint32_t value = tmp->m_dataValue.uintValue;
                        QString valueS = QString::number((value>>24&0xFF)) + "." +QString::number((value>>16&0xFF))+ "." +
                                QString::number((value>>8&0xFF)) + "." + QString::number((value>>0&0xFF));
                        device->netConfig->m_masterMASK = valueS;
                    }
                    //下位机IP网关
                    if(tmp->registerId == 1934){
                        uint32_t value = tmp->m_dataValue.uintValue;
                        QString valueS = QString::number((value>>24&0xFF)) + "." +QString::number((value>>16&0xFF))+ "." +
                                QString::number((value>>8&0xFF)) + "." + QString::number((value>>0&0xFF));
                        device->netConfig->m_masterGATEWAY = valueS;
                    }
                    //QT FTP服务器地址
                    if(tmp->registerId == 1948){
                        uint32_t value = tmp->m_dataValue.uintValue;
                        QString valueS = QString::number((value>>24&0xFF)) + "." +QString::number((value>>16&0xFF))+ "." +
                                QString::number((value>>8&0xFF)) + "." + QString::number((value>>0&0xFF));
                        device->netConfig->m_ftpIP = valueS;
                    }
                }
                //下位机参数保存寄存器
                if(tmp->registerId==2000){
                    device->m_lowerMachinePC = tmp;
                    MY_DEBUG << "m_lowerMachinePC addr is " << device->m_lowerMachinePC->fpgaID << device->m_lowerMachinePC->addrOffset;
                }

                //DO
                if(tmp->registerId >=2002 && tmp->registerId <= 2006){
                    for(int m=0; m<device->m_doFormParameterList->size(); m++){
                        ParameterClass* t = device->m_doFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_doFormParameterList->removeAt(m);
                        }
                    }
                    device->m_doFormParameterList->append(tmp);
                }

                //下位机重启寄存器
                if(tmp->registerId==2010){
                    device->resetMaterControl = tmp;
                    MY_DEBUG << "resetMaterControl addr is " << device->resetMaterControl->fpgaID << device->resetMaterControl->addrOffset;
                }
            }
        }
        MY_DEBUG << "MAIN SIZE IS " << device->m_mainFormParameterList->size();
        MY_DEBUG;
    }
    else if(fileName == "var")
    {
        MY_DEBUG;
        MY_DEBUG << "MAIN SIZE IS " << device->m_mainFormParameterList->size();
        /**
         * 1.Var表的AddrOffset偏移地址值为1720;R寄存器ID（04功能码）:304；FPGA偏移地址：2920。
         * 2.褐色条目：寄存器ID为用于modbus通信所用的寄存器ID，FPGA为FPGA的变量地址编号
         * 3.Var在软件设计上预留200个变量,共计1200个变量
         */
        table = new ParameterTable(1720,0,304,0,2920,file,currentCabinetID);
        if(table!=NULL && !table->isHaveError){
            //获取主页面所需参数数据
            QList<ParameterClass*>* list = table->m_list;
            //            device->m_mainFormParameterList->clear();
            //            device->m_diFormParameterList->clear();
            for(int k=0; k<table->m_list->size(); k++){
                ParameterClass* tmp = table->m_list->at(k);
                //主页面告警
                if(tmp->registerId == 442)
                {
                    for(int m=0; m<device->m_mainFormParameterList->size(); m++){
                        ParameterClass* t = device->m_mainFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_mainFormParameterList->removeAt(m);
                        }
                    }
                    device->m_mainFormParameterList->append(tmp);
                }
                //主页面状态图标/调试页面图标显示
                if(tmp->registerId >= 2256 && tmp->registerId <= 2270)
                {
                    //主页面状态图标
                    for(int m=0; m<device->m_mainFormParameterList->size(); m++){
                        ParameterClass* t = device->m_mainFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_mainFormParameterList->removeAt(m);
                        }
                    }
                    device->m_mainFormParameterList->append(tmp);

                    //调试页面图标
                    for(int m=0; m<device->m_systemFormParameterList->size(); m++){
                        ParameterClass* t = device->m_systemFormParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_systemFormParameterList->removeAt(m);
                        }
                    }
                    device->m_systemFormParameterList->append(tmp);
                }
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
                for(int i=0; i<sizeof (mainForm)/sizeof(int); i++)
                {
                    if(tmp->registerId == mainForm[i]){
                        for(int m=0; m<device->m_mainFormParameterList->size(); m++){
                            ParameterClass* t = device->m_mainFormParameterList->at(m);
                            if(t->registerId == tmp->registerId){
                                device->m_mainFormParameterList->removeAt(m);
                            }
                        }
                        device->m_mainFormParameterList->append(tmp);
                    }
                }

                //Di
                int diForm[] = {2304, 2306, 2308};
                for(int i=0; i<sizeof (diForm)/sizeof(int); i++){
                    if(tmp->registerId == diForm[i]){
                        for(int m=0; m<device->m_diFormParameterList->size(); m++){
                            ParameterClass* t = device->m_diFormParameterList->at(m);
                            if(t->registerId == tmp->registerId){
                                device->m_diFormParameterList->removeAt(m);
                            }
                        }
                        device->m_diFormParameterList->append(tmp);
                    }
                }
                //故障查看界面所需的参数
                if(tmp->registerId>=458 && tmp->registerId<=488){
                    for(int m=0; m<device->m_faultParameterList->size(); m++){
                        ParameterClass* t = device->m_faultParameterList->at(m);
                        if(t->registerId == tmp->registerId){
                            device->m_faultParameterList->removeAt(m);
                        }
                    }
                    device->m_faultParameterList->append(tmp);
                }
            }
        }
        MY_DEBUG << "MAIN SIZE IS " << device->m_mainFormParameterList->size();
        MY_DEBUG;
    }else{
        MY_DEBUG;
    }
    MY_DEBUG;
    return table;
}

/**
 * @brief CabinetManagement::updateWaveChannel--更新故障录波通道名
 * @param id
 * @param newName
 */
void CabinetManagement::updateWaveChannel(int id, QString newName)
{
    QStringList * list = this->m_cabinetList.at(currentCabinetID)->m_waveChannelNameList;
    for(int i=0; i<list->size(); i++){
        QString tmp = list->at(i);
        if(tmp.split(":").at(0) == QString::number(id)){
            list->removeAt(i);
            list->insert(i,(QString::number(id)+":"+newName));
            break;
        }
    }
    //写入文件
    QFile waveChannelConfig(QDir::currentPath() + WAVE_CONFIG_BAK_FILE + QString::number(currentCabinetID));
    if(waveChannelConfig.exists()){
        if(waveChannelConfig.open(QFile::WriteOnly | QFile::Truncate)){
            QTextStream out(&waveChannelConfig);//文本流
            out.setCodec(QTextCodec::codecForName("UTF-8"));
            for(int i=0; i<list->size(); i++){
                out << list->at(i) <<endl;
            }
            out.flush();
            waveChannelConfig.close();
        }
    }
}

/**
 * @brief saveWaterCoolingParamet--保存水冷参数
 */
void CabinetManagement::saveWaterCoolingParamet()
{
    //保存para表
    this->m_cabinetList.at(currentCabinetID)->m_fileConfig->value("para")->updateTableMap2File();
}

/**
 * @brief saveWaterCoolingParamet--保存Dio参数
 */
void CabinetManagement::saveDioCoolingParamet()
{
    //保存para表
    this->m_cabinetList.at(currentCabinetID)->m_fileConfig->value("para")->updateTableMap2File();
}

/**
 * @brief CabinetManagement::getWaveChannelNameList--获取故障录波通道名List
 * @return
 */
QStringList *CabinetManagement::getWaveChannelNameList()
{
    return this->m_cabinetList.at(currentCabinetID)->m_waveChannelNameList;
}

/**
 * @brief CabinetManagement::checkParameterFile--参数文件检查
 * @param filePath
 * @return
 */
bool CabinetManagement::checkParameterFile(QString filePath)
{
    QFileInfo file(filePath);
    bool ret=false;
    int argCount = 0;
    if(!file.exists()){
        return false;
    }
    QString fileName = file.baseName();
    MY_DEBUG << fileName;
    if(fileName != "master" && fileName != "para" && fileName != "var"){
        return false;
    }
    //读取参数文件
    ParameterTable *table;
    if(fileName == "master"){
        /**
         * 1.主控数据表的AddrOffset偏移地址值为1200;WR寄存器ID偏移地址：2400,R寄存器ID偏移地址：0;FPGA（上位机WR）偏移地址:2400，FPGA（上位机R）偏移地址:2616。
         * 2.褐色条目：寄存器ID为用于modbus通信所用的寄存器ID，FPGA为FPGA的变量地址编号
         * 3.主控数据表在软件设计上预留403个变量，根据读写属性，预留200个读写变量，203个只读变量,共计520个变量
         */
        table = new ParameterTable(1200,2400,0,2400,2616,file.absoluteFilePath(),0);
        if(table!=NULL && !table->isHaveError){
            ret=true;
        }
    }else if(fileName == "para"){       //加载para参数
        /**
         * 1.para表的AddrOffset偏移地址值为0；WR寄存器ID(03功能码)、FPGA偏移地址均为0。
           2.褐色条目：寄存器ID为用于modbus通信所用的寄存器ID，FPGA为FPGA的变量地址编号
           3.para在软件设计上预留200个变量，共计1200个变量
         */
        table = new ParameterTable(0,0,0,0,0,file.absoluteFilePath(),0);
        if(table!=NULL && !table->isHaveError){
            for(int k=0; k<table->m_list->size(); k++){
                ParameterClass* tmp = table->m_list->at(k);
                if(tmp->registerId == 1930 || tmp->registerId == 1932 || tmp->registerId == 1934 || tmp->registerId == 1948){
                    argCount++;
                }
                if(tmp->registerId==2000){
                    argCount++;
                }
                if(tmp->registerId==2010){
                    argCount++;
                }
            }
            if(argCount==6){
                ret=true;
            }
        }
    }else{
        /**
         * 1.Var表的AddrOffset偏移地址值为1720;R寄存器ID（04功能码）:304；FPGA偏移地址：2920。
         * 2.褐色条目：寄存器ID为用于modbus通信所用的寄存器ID，FPGA为FPGA的变量地址编号
         * 3.Var在软件设计上预留200个变量,共计1200个变量
         */
        table = new ParameterTable(1720,0,304,0,2920,file.absoluteFilePath(),0);
        if(table!=NULL && !table->isHaveError){
            //获取主页面所需参数数据
            QList<ParameterClass*>* list = table->m_list;
            if(list->size()>626){
                int mainForm[] = {607, 608, 609, 610, 612, 613, 615, 616, 617, 618, 619,
                                  620, 621, 622, 623, 624, 625, 626};
                if(list->size()>626){
                    ret=true;
                }
            }
        }
    }
    return ret;
}

/**
 * @brief CabinetManagement::loadParameterFile--加载配置文件
 * @param filePath--文件路径
 */
void CabinetManagement::loadParameterFile(QString filePath)
{
    QFileInfo file(filePath);
    if(!file.exists()){
        emit this->parameterLoadResult(false, QString::fromLocal8Bit("文件不存在"));
        return;
    }
    QString fileName = file.baseName();
    MY_DEBUG << fileName;
    if(fileName != "master" && fileName != "para" && fileName != "var"){
        emit this->parameterLoadResult(false, QString::fromLocal8Bit("文件错误，请导入：master,para,var中的一种文件"));
        return;
    }
    //拷贝文件到对应的device目录下
    QString copyTo = QDir::currentPath()+CONFIG_BAK_DIR + QString::number(currentCabinetID)+"/"+fileName+".csv";
    bool ret = Tool::copyFileToPath(filePath, copyTo, true);
    if(!ret){
        emit this->parameterLoadResult(false, QString::fromLocal8Bit("替换文件失败"));
        return;
    }
    //加载文件到device的m_fileConfig
    CabinetDevice *device;
    if(this->m_cabinetList.size()==2){
        device = this->m_cabinetList.at(currentCabinetID);
    }else{
        emit this->parameterLoadResult(false, QString::fromLocal8Bit("缺少原始参数文件，导入失败"));
        return;
    }
    //读取参数文件
    ParameterTable *table = this->readParameterFile(device, copyTo, fileName);
    if(table!=NULL && !table->isHaveError){
        //插入配置文件map
        if(device->m_fileConfig->contains(fileName)){
            device->m_fileConfig->remove(fileName);
            device->m_fileConfig->insert(fileName, table);
        }else{
            delete table;
            emit this->parameterLoadResult(true, QString::fromLocal8Bit("加载文件失败，错误的文件名"));
            return;
        }
    }else{
        emit this->parameterLoadResult(true, QString::fromLocal8Bit("加载文件失败，文件格式错误"));
        return;
    }
    //发送信号
    emit this->parameterLoadResult(true, QString::fromLocal8Bit("加载文件成功"));
}

//保存参数数据到文件
void CabinetManagement::updateParameterConfigFile(int cabinetID, QString fileName)
{
    if(this->m_cabinetList.size()==0){
        return;
    }
    QMap<QString, ParameterTable*>::Iterator it = m_cabinetList.at(cabinetID)->m_fileConfig->begin();
    while(it!=m_cabinetList.at(cabinetID)->m_fileConfig->end())
    {
        if(it.key() == fileName){
            it.value()->updateTableMap2File();
            break;
        }
    }
}

/**
 * @brief CabinetManagement::getParameterBitDescribe--根据参数提供的fpga地址获取参数的位描述
 * @param fpga--fpga地址
 * @return
 */
QStringList CabinetManagement::getParameterBitDescribe(uint32_t fpga)
{
    QStringList parameterList;
    QString desFilePath = m_cabinetList.at(currentCabinetID)->m_describeDir + "/" +QString::number(fpga);
    QFile des(desFilePath);
    if(!des.exists()){
        for(int i=0; i<32; i++){
            parameterList.append(QString::fromLocal8Bit("保留"));
        }
    }else{
        if(!des.open(QFile::ReadOnly)){
            MY_DEBUG << desFilePath << "is not exists?" <<endl;
            return QStringList();
        }
        QTextStream * out = new QTextStream(&des);//文本流
        out->setCodec(QTextCodec::codecForName("UTF-8"));
        while(1){
            QString desValue = out->readLine();
            if(desValue.isEmpty()){
                break;
            }
            parameterList << desValue;
        }
        des.close();
    }
    return parameterList;
}

/**
 * @brief CabinetManagement::getParameterBitDescribe--根据参数提供的fpga地址获取参数的位描述
 * @param fpga--fpga地址
 * @return
 */
bool CabinetManagement::updateParameterBitDescribe(uint32_t fpga, QStringList descList)
{
    QStringList parameterList;
    QString desFilePath = m_cabinetList.at(currentCabinetID)->m_describeDir + "/" +QString::number(fpga);
    QFile des(desFilePath);
    if(!des.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        MY_DEBUG << desFilePath << "is not exists?" <<endl;
        return false;
    }
    QTextStream out(&des);//文本流
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    for(int i=0; i<descList.size(); i++){
        out << descList.at(i) << endl;
    }
    des.close();
    return true;
}

//保存配置数据到文件--网络配置--保存para表
void CabinetManagement::updateDeviceNetConfigFile(int cabinetID)
{
    bool ok;
    if(this->m_cabinetList.size()==0){
        return;
    }
    //保存para表
    CabinetDevice *device = m_cabinetList.at(cabinetID);
    if(device->m_fileConfig->contains("para")){
        //更新IP地址信息到para表
        for(int i=0; i<device->m_lmNetConfig->size(); i++){
            ParameterClass *tmp =  device->m_lmNetConfig->at(i);
            //下位机IP地址
            if(tmp->registerId == 1930){
                uint32_t value = 0;
                QStringList valueS = device->netConfig->m_masterIP.split(".");
                for(int i=0; i<valueS.size(); i++){
                    value = value<<8 | valueS.at(i).toUInt(&ok);
                }
                tmp->m_dataValue.uintValue=value;
            }
            //下位机IP掩码
            if(tmp->registerId == 1932){
                uint32_t value = 0;
                QStringList valueS = device->netConfig->m_masterMASK.split(".");
                for(int i=0; i<valueS.size(); i++){
                    value = value<<8 | valueS.at(i).toUInt(&ok);
                }
                tmp->m_dataValue.uintValue=value;
            }
            //下位机IP网关
            if(tmp->registerId == 1934){
                uint32_t value = 0;
                QStringList valueS = device->netConfig->m_masterGATEWAY.split(".");
                for(int i=0; i<valueS.size(); i++){
                    value = value<<8 | valueS.at(i).toUInt(&ok);
                }
                tmp->m_dataValue.uintValue=value;
            }
            //QT FTP服务器地址
            if(tmp->registerId == 1948){
                uint32_t value = 0;
                QStringList valueS = device->netConfig->m_ftpIP.split(".");
                for(int i=0; i<valueS.size(); i++){
                    value = value<<8 | valueS.at(i).toUInt(&ok);
                }
                tmp->m_dataValue.uintValue=value;
            }
        }
        m_cabinetList.at(cabinetID)->m_fileConfig->value("para")->updateTableMap2File();
    }
}

//保存配置数据到文件--FTP配置
void CabinetManagement::updateDeviceFtpConfigFile(int cabinetID)
{
    MY_DEBUG;
    if(this->m_cabinetList.size()==0){
        return;
    }
    //保存 para表
    if(m_cabinetList.at(cabinetID)->m_fileConfig->contains("para")){
        m_cabinetList.at(cabinetID)->m_fileConfig->value("para")->updateTableMap2File();
    }
    //保存 ftp文件
    CabinetDevice::NetConfig *currentConfig = this->m_cabinetList.at(cabinetID)->netConfig;
    QString ftpPath = QDir::currentPath()+ FTP_CONFIG_BAK_FILE + QString::number(cabinetID);
    QFile ftpcofig(ftpPath);
    if(ftpcofig.open(QIODevice::WriteOnly))
    {
        QTextStream out(&ftpcofig);//文本流
        out.setCodec(QTextCodec::codecForName("UTF-8"));
        out<<currentConfig->m_ftpUser<<endl;
        out<<currentConfig->m_ftpLogin<<endl;
        out<<currentConfig->m_ftpRootDir<<endl;
        out.flush();
        ftpcofig.close();
    }
}

//添加Parameter到示波器
bool CabinetManagement::addParameter2Oscilloscope(ParameterClass* p)
{
    if(this->m_cabinetList.size()==0){
        return false;
    }
    QList<ParameterClass*> *list = this->m_cabinetList.at(this->currentCabinetID)->m_oscilloscopeList;
    if(list->size()>=18){
        return false;
    }
    list->append(p);
    emit oscillListChanged();
    return true;
}

//删除Parameter到示波器
void CabinetManagement::removeParameter2Oscilloscope(ParameterClass* p)
{
    bool ret=false;
    if(this->m_cabinetList.size()==0){
        return;
    }
    QList<ParameterClass*> *list = this->m_cabinetList.at(this->currentCabinetID)->m_oscilloscopeList;
    for(int i=0; i<list->size(); i++){
        ParameterClass* item = list->at(i);
        if(item->registerId == p->registerId){
            item->setOscilloscope(false);
            list->removeAt(i);
            ret = true;
            break;
        }
    }
    if(ret){
        emit oscillListChanged();
    }
}

//删除Parameter到示波器
void CabinetManagement::removeAllParameter2Oscilloscope()
{
    bool ret=false;
    if(this->m_cabinetList.size()==0){
        return;
    }
    QList<ParameterClass*> *list = this->m_cabinetList.at(this->currentCabinetID)->m_oscilloscopeList;
    for(int i=0; i<list->size();){
        ParameterClass* item = list->at(i);
        item->setOscilloscope(false);
        list->removeAt(i);
    }
    if(ret){
        emit oscillListChanged();
    }
}

//添加Parameter到自定义列表
void CabinetManagement::addParameter2Custom(ParameterClass* p)
{
    if(this->m_cabinetList.size()==0){
        return;
    }
    QList<ParameterClass*> *list = this->m_cabinetList.at(this->currentCabinetID)->m_customList;
    list->append(p);
}

//删除Parameter从自定义列表
void CabinetManagement::removeParameter2Custom(ParameterClass* p)
{
    bool ret=false;
    if(this->m_cabinetList.size()==0){
        return;
    }
    QList<ParameterClass*> *list = this->m_cabinetList.at(this->currentCabinetID)->m_customList;
    for(int i=0; i<list->size(); i++)
    {
        ParameterClass* item = list->at(i);
        if(item->registerId == p->registerId){
            list->removeAt(i);
            ret = true;
            break;
        }
    }
}

//恢复出厂设置
void CabinetManagement::reset()
{
    /**
     * copy一份原始配置文件---恢复之后要求重启软件
     **/
    for(int i=0; i<2; i++)
    {
        Tool::copyDirectoryFiles((QDir::currentPath()+ CONFIG_DIR + QString::number(i)),
                                 (QDir::currentPath()+ CONFIG_BAK_DIR + QString::number(i)),true);
        Tool::copyFileToPath(QDir::currentPath()+NET_CONFIG_FILE + QString::number(i),
                             QDir::currentPath()+NET_CONFIG_BAK_FILE + QString::number(i),true);
    }
}

/**
 * @brief getMasterContrlReset--重启主控设备
 */
ParameterClass* CabinetManagement::getMasterContrlReset()
{
    return this->m_cabinetList.at(this->currentCabinetID)->resetMaterControl;
}

void CabinetManagement::on_activitySwitchFinish(int activityID)
{
    emit cabinetSwitchFinish();
}
