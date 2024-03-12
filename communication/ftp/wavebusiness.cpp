#include "wavebusiness.h"
#include "common/commonclass.h"
#include "common/cabinetmanagement.h"
#include "common/tool.h"

#include <QTextCodec>
#include <QFileInfo>

WaveBusiness::WaveBusiness(QObject *parent) : BusinessHandler(parent),
    m_tcpServer(Q_NULLPTR)
{
    this->level = BusinessClass::FTP_HANDLER;        //指定业务level
    this->isThreadQuit=false;
    //    this->isLink=false;

    //1):初始化服务器
    m_tcpServer = new MyTcpServer(this);
    m_tcpServer->setMaxPendingConnections(2);
    connect(m_tcpServer, &MyTcpServer::clientUploadData, this, &WaveBusiness::on_clientUploadData);

    CabinetManagement *manager = CabinetManagement::instance();
    CabinetManagement::CabinetDevice::Net_config *net = manager->getNetConfig(manager->getCurrentCabinetID());

    int port = net->m_ftpPort;
    MY_DEBUG << "server port is " << port;
    //可以使用 QHostAddress::Any 监听所有地址的对应端口
    //开始监听，并判断是否成功
    if(!m_tcpServer->listen(QHostAddress::AnyIPv4, port)){
        MY_DEBUG << "server listen failed";
        m_tcpServer->clear();
    }
    MY_DEBUG << "server listen success";
}

void WaveBusiness::run()
{
    //读取上层指令
    while(!this->isThreadQuit)
    {
        sync.lock();
        if(is_pause){
            pauseCond.wait(&sync); // in this place, your thread will stop to execute until someone calls resume
        }
        sync.unlock();

        QByteArray request;
        //查看队列中是否有新的指令
        this->m_currentBis= this->outQueue();
        if(this->m_currentBis.getLevel()!=BusinessClass::NONE_HANDLER){
            switch (this->m_currentBis.getCommandType()){
            case BusinessClass::WAVE_QUERY:       //故障录波查询
            {
                MY_DEBUG << "WAVE_QUERY request-->" << m_currentBis.getFtpUser() << m_currentBis.getFtpLogin()
                         << m_currentBis.getFtpPath();
                QList<QString> arg;
                arg << this->m_currentBis.getFtpUser() << this->m_currentBis.getFtpLogin()
                    << this->m_currentBis.getFtpPath();
                request =  buildRequestData(BusinessClass::WAVE_QUERY, arg);
                //发送数据
                if(!writeClientData(request, m_currentBis.getCommandType())){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::NET_ERROE;
                    resp.errorMsg = QString::fromLocal8Bit("故障录波查询失败，网络错误");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                }
            }
                break;
            case BusinessClass::WAVE_OBTAIN:        //故障录波获取
                MY_DEBUG << "WAVE_OBTAIN request";
                request =  buildRequestData(BusinessClass::WAVE_OBTAIN, this->m_currentBis.getWaveData());
                //发送数据
                if(!writeClientData(request, m_currentBis.getCommandType())){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::NET_ERROE;
                    resp.errorMsg = QString::fromLocal8Bit("故障录波文件获取失败，网络错误");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                }
                break;
            case BusinessClass::PARAMETER_DOWN:{
                CabinetManagement *manager = CabinetManagement::instance();
                CabinetManagement::CabinetDevice::Net_config *net = manager->getNetConfig(manager->getCurrentCabinetID());

                //下发参数文件到下位机
                QString ftpPath = "ftp://" + net->m_ftpIP + PARAMETER_DIR +
                        QString::number(manager->getCurrentCabinetID()) + "/" + downParameterFile +".csv";
                MY_DEBUG << "PARAMETER_DOWN ftp Down file path is " << ftpPath;
                //crc校验
                QString copyFile = net->m_ftpParameterDownloadDir + "/" + downParameterFile + ".csv";
                MY_DEBUG << "PARAMETER_DOWN ftp Down file path is " << copyFile;
                QByteArray ba = copyFile.toLatin1();
                char *crcPath = ba.data();
                uint crc = Tool::get_crc32(crcPath);
                MY_DEBUG << "crc is " << crc;
                if(crc==0){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::DATA_ERROR;
                    resp.errorMsg = QString::fromLocal8Bit("参数文件crc校验失败");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                    break;
                }

                //发送下载帧
                QList<QString> arg;
                arg << net->m_ftpUser << net->m_ftpLogin << ftpPath << QString::number(crc);
                request =  buildRequestData(BusinessClass::PARAMETER_DOWN, arg);
                MY_DEBUG << "writeClientData start";
                //发送数据--配置帧
                if(!writeClientData(request, m_currentBis.getCommandType())){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::NET_ERROE;
                    resp.errorMsg = QString::fromLocal8Bit("参数文件下载失败，网络错误");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                }
                MY_DEBUG << "writeClientData finish";
            }
                break;
            case BusinessClass::PARAMETER_DOWN_START:{     //参数文件下发启动帧
                MY_DEBUG << "PARAMETER_DOWN_START request";
                QString file = this->m_currentBis.getFilePath();
                MY_DEBUG << "PARAMETER_DOWN file path is " << file;
                QFileInfo mFile(file);
                if(!mFile.exists()){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::DATA_ERROR;
                    resp.errorMsg = QString::fromLocal8Bit("参数文件下载失败，文件不存在");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                    break;
                }
                //文件名检测
                downParameterFile = mFile.baseName();
                QStringList paraList = CabinetManagement::instance()->getParamterList();
                bool findOK=false;
                for(QString item:paraList){
                    if(downParameterFile == item){
                        findOK=true;
                        break;
                    }
                }
                if(!findOK){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::DATA_ERROR;
                    resp.errorMsg = QString::fromLocal8Bit("参数文件下载失败，文件名错误，所选取的文件并非指定的参数文件，请选取合适的参数文件");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                    break;
                }

                //拷贝文件到ftp下载目录
                CabinetManagement *manager = CabinetManagement::instance();
                CabinetManagement::CabinetDevice::Net_config *net = manager->getNetConfig(manager->getCurrentCabinetID());
                QString copyFile = net->m_ftpParameterDownloadDir + "/" + downParameterFile + ".csv";
                MY_DEBUG << "copy ftp Down file is " << copyFile;
                if(!Tool::copyFileToPath(file, copyFile, true)){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::DATA_ERROR;
                    resp.errorMsg = QString::fromLocal8Bit("参数文件下载失败，拷贝参数文件到ftp下载目录失败");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                    break;
                }

                //检测文件格式是否有误
                if(!manager->checkParameterFile(copyFile)){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::DATA_ERROR;
                    resp.errorMsg = QString::fromLocal8Bit("参数文件下载失败，参数文件格式有误，请检查");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                    break;
                }

                //发送启动帧
                MY_DEBUG << "send down start frame start";
                QByteArray downStart = buildRequestData(BusinessClass::PARAMETER_DOWN_START, QList<QString>());
                if(!writeClientData(downStart, m_currentBis.getCommandType())){
                    BusinessResponse resp;
                    resp.errorID = BusinessResponse::DATA_ERROR;
                    resp.errorMsg = QString::fromLocal8Bit("参数文件下载失败，网络错误");
                    resp.result=false;
                    resp.type = m_currentBis.getCommandType();
                    emit responseBusiness(resp);
                    break;
                }
                MY_DEBUG << "send down start frame finish";

            }
                break;
            default:
                break;
            }
        }
        msleep(500);
    }
    m_tcpServer->clear();
    this->exec();
}

/**
 * @brief WaveBusiness::on_clientUploadData--上传客户端的数据到上层应用
 * @param error
 * @param resp
 * @param msg
 */
void WaveBusiness::on_clientUploadData(BusinessResponse resp)
{
    emit responseBusiness(resp);

}

/**
 * @brief WaveBusiness::buildRequestData--组件命令
 * @param type 类型
 * @param dataList 数据
 * @return QByteArray命令
 */
QByteArray WaveBusiness::buildRequestData(BusinessClass::COMMAND_TYPE type, QList<QString> dataList)
{
    QByteArray array;

    array.append(0x68).append(0x68);     //协议标识
    switch(type)
    {
    case BusinessClass::WAVE_OBTAIN:
    {
        uint16_t dataLen = 0;
        for(int i=0; i<dataList.size(); i++)
        {
            dataLen += dataList.at(i).size();
        }
        dataLen+=1;
        array.append(0x03);     //功能码
        array.append((char)(dataLen&0xFF)).append((char)(dataLen>>8&0xFF));     //数据长度--高在前，低在后
        array.append((char)dataList.size());     //文件个数
        //数据
        for(int i=0; i<dataList.size(); i++)
        {
            QString file = dataList.at(i);
            //文件名（27字节）
            array.append(file.toUtf8());
        }
    }
        break;
    case BusinessClass::WAVE_QUERY:
    {
        if(dataList.size()!=3){
            return QByteArray();
        }
        array.append(0x02);     //功能码
        uint16_t len = 32 + 32 + 64 + 2;
        array.append((char)(len&0xFF)).append((char)(len>>8&0xFF));     //数据长度--高在前，低在后

        char buf[32] = {0};
        char *p;
        QByteArray user = dataList.at(0).toLatin1();
        p = user.data();
        memset(buf,0,sizeof(buf));
        memcpy(buf, p, strlen(p));
        array.append(buf,sizeof(buf));

        QByteArray login = dataList.at(1).toLatin1();
        p = login.data();
        memset(buf,0,sizeof(buf));
        memcpy(buf, p, strlen(p));
        array.append(buf,sizeof(buf));

        char buf1[64] = {0};
        QByteArray path = dataList.at(2).toLatin1();
        p = path.data();
        memcpy(buf1, p, strlen(p));
        array.append(buf1,sizeof(buf1));
    }
        break;
    case BusinessClass::PARAMETER_DOWN_START:{
        array.append(0x01).append((char)0x00).append(0x03).append((char)0x00);
    }
        break;
    case BusinessClass::PARAMETER_DOWN:{
        if(dataList.size()!=4){
            return QByteArray();
        }
        array.append(0x05);     //功能码   --- 配置文件升级内容帧
        uint16_t len = 32 + 32 + 64 + 4 + 2;
        array.append((char)(len&0xFF)).append((char)(len>>8&0xFF));     //数据长度--高在前，低在后

        char buf[32] = {0};
        char *p;
        QByteArray user = dataList.at(0).toLatin1();
        p = user.data();
        memset(buf,0,sizeof(buf));
        memcpy(buf, p, strlen(p));
        array.append(buf,sizeof(buf));

        QByteArray login = dataList.at(1).toLatin1();
        p = login.data();
        memset(buf,0,sizeof(buf));
        memcpy(buf, p, strlen(p));
        array.append(buf,sizeof(buf));

        char buf1[64] = {0};
        QByteArray path = dataList.at(2).toLatin1();
        p = path.data();
        memcpy(buf1, p, strlen(p));
        array.append(buf1,sizeof(buf1));
        //校验码
        bool isOk=false;
        int crc = dataList.at(3).toUInt(&isOk);
        array.append((char)(crc>>24&0xFF)).append((char)(crc>>16&0xFF)).append((char)(crc>>8&0xFF)).append((char)(crc>>0&0xFF));
    }
        break;
    default:
        break;
    }
    array.append(0x55).append(0xAA);    //协议尾
    return array;
}

/**
 * @brief WaveBusiness::writeClientData--发送数据给客户端
 * @param request 请求
 */
bool WaveBusiness::writeClientData(QByteArray request, BusinessClass::COMMAND_TYPE updateID)
{
    CabinetManagement *manager = CabinetManagement::instance();
    CabinetManagement::CabinetDevice::NetConfig *config = manager->getNetConfig(manager->getCurrentCabinetID());
    return m_tcpServer->issueInstructions(config->m_masterIP, request, updateID);
}


bool WaveBusiness::isLinked()
{
    MY_DEBUG;
    return this->m_tcpServer->isHaveClient();
}
