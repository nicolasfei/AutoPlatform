#include "oscillbusinesssigne.h"
#include "common/commonclass.h"
#include "common/cabinetmanagement.h"
#include "oscilloscopeActivity/oscilldatasavetask.h"
#include "oscilloscopeActivity/oscilldataqueue.h"
#include <QDateTime>
#include <QIODevice>
#include <QNetworkProxy>

OscillBusinessSigne::OscillBusinessSigne(QObject *parent) :
    BusinessHandler(parent),
    m_tcpSocket(Q_NULLPTR),
    tcpIPChange(false)
{
    this->level = BusinessClass::TCP_IP_HANDLER;        //指定业务level
    this->status=NO_TASK;  //连接状态
    this->isThreadQuit=false;
    //    this->start();      //启动本线程
}

OscillBusinessSigne::~OscillBusinessSigne()
{
    this->isThreadQuit=true;
    //this->quit();   //线程退出
    BusinessHandler::~BusinessHandler();
}

QByteArray OscillBusinessSigne::buildRequestData(BusinessClass::COMMAND_TYPE type,
                                                 QList<ParameterClass::RegisterData> dataList)
{
    static uint16_t m_transactCount=0;
    QByteArray array;
    uint16_t dataLen=0;

    if(m_transactCount==65535){
        m_transactCount=0;
    }
    array.append(m_transactCount>>8&0xFF).append(m_transactCount&0xFF);     //事务标识
    array.append(0x55).append(0xAA);     //协议标识
    switch(type)
    {
    case BusinessClass::OSCILL_CH_CONFIG:
    {
        dataLen = dataList.size()*4 + 3;
        array.append((char)(dataLen>>8&0xFF)).append((char)(dataLen&0xFF));     //数据长度
        array.append(0x01);     //单元标识
        array.append(0x60);     //功能码
        array.append(0x01);     //子功能码
        //数据
        for(int i=0; i<dataList.size(); i++)
        {
            ParameterClass::RegisterData rData = dataList.at(i);
            //通道配置数据----通道编号1个字节,通道地址（FPGA地址）2个字节，通道数据类型1个字节
            array.append(i).append((char)(rData.fpgaID>>8&0xFF)).append((char)(rData.fpgaID&0xFF)).append(rData.type);
        }
    }
        break;
    case BusinessClass::OSCILL_SAMPLE:
    {
        dataLen = 3;
        array.append((char)(dataLen>>8&0xFF)).append((char)(dataLen&0xFF));     //数据长度
        array.append(0x01);     //单元标识
        array.append(0x70);     //功能码
        array.append(0x01);     //子功能码
    }
        break;
    case BusinessClass::OSCILL_SAMPLE_STOP:
    {
        dataLen = 3;
        array.append((char)(dataLen>>8&0xFF)).append((char)(dataLen&0xFF));     //数据长度
        array.append(0x01);     //单元标识
        array.append(0x70);     //功能码
        array.append(0x03);     //子功能码
    }
        break;
    default:
        break;
    }
    return array;
}

/**
 * @brief TcpIpBusiness::run--qthread run
 */
void OscillBusinessSigne::run()
{
    bool ret;
    int res;
    //    static int intervalReadCount=0;
    while (!isThreadQuit)
    {
        //初始化变量
        status = NO_TASK;
        ipPortChange = false;
        netError=false;
        isCabinetStart=false;
        this->isLink=false;

        //1):连接到服务器
        if(m_tcpSocket!=Q_NULLPTR && m_tcpSocket->state() == QAbstractSocket::ConnectedState){
            //先断开连接
            m_tcpSocket->disconnectFromHost();
            m_tcpSocket->waitForDisconnected();
            m_tcpSocket->close();
        }
        m_tcpSocket = new QTcpSocket(this);
        if(m_tcpSocket==Q_NULLPTR){
            thread()->msleep(10);
            continue;
        }
        //获取IP Port
        CabinetManagement *management = CabinetManagement::instance();
        CabinetManagement::CabinetDevice::NetConfig* net = management->getNetConfig(management->getCurrentCabinetID());
        if(net==Q_NULLPTR){
            MY_DEBUG << "NetConfig is null ";
            return;
        }

        MY_DEBUG << "connect To Host "<< net->m_masterIP << " " << net->m_masterTcpPort;
        //连接到host
        m_tcpSocket->setProxy(QNetworkProxy::NoProxy);
        m_tcpSocket->connectToHost(net->m_masterIP, net->m_masterTcpPort);
        //等待连接完成
        ret = m_tcpSocket->waitForConnected();
        if(!ret){
            MY_DEBUG <<  m_tcpSocket->errorString();
            MY_DEBUG << "link to host failed!";
            m_tcpSocket->close();
            this->isLink=false;
            emit netLinkStatusChange(false);
            thread()->msleep(10);
            continue;
        }
        //监听网络连接错误
        connect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
        MY_DEBUG << "link to host success";
        this->isLink=true;
        emit netLinkStatusChange(true);
        float xCount=0;
        float &newXCount = xCount;
        //2):等待指令，并做相应的操作
        while (!isThreadQuit && !ipPortChange && !netError)
        {   //线程未退出，ip地址没变，网络连接无错误
            sync.lock();
            if(is_pause){
                pauseCond.wait(&sync); // in this place, your thread will stop to execute until someone calls resume
                xCount=0;
            }
            sync.unlock();
            //1)查看是否有新的指令需要发送给下位机
            BusinessClass command = this->outQueue();
            if(command.getLevel() != BusinessClass::NONE_HANDLER){  //如果是无效指令不处理
                //发送指令
                QByteArray req =  buildRequestData(command.getCommandType(), command.getData());
                //配置前先清空缓存
                if(command.getCommandType()==BusinessClass::OSCILL_CH_CONFIG){
                    m_tcpSocket->readAll();
                }
                res = m_tcpSocket->write(req);
                m_tcpSocket->waitForBytesWritten();
                //解析结果
                switch (command.getCommandType()) {
                case BusinessClass::OSCILL_CH_CONFIG:       //配置通道指令
                    if(res==-1){
                        MY_DEBUG << "send config command failed";
                        netError=true;
                        BusinessResponse resp;
                        resp.errorID = BusinessResponse::NET_ERROE;
                        resp.errorMsg = QString::fromLocal8Bit("配置通道失败：网络错误");
                        resp.result=false;
                        resp.type = command.getCommandType();
                        emit responseBusiness(resp);
                    }
                    break;
                case BusinessClass::OSCILL_SAMPLE:
                    if(res==-1){
                        MY_DEBUG << "send start oscill command failed";
                        netError=true;
                        BusinessResponse resp;
                        resp.errorID = BusinessResponse::NET_ERROE;
                        resp.errorMsg = QString::fromLocal8Bit("启动示波器失败：网络错误");
                        resp.result=false;
                        resp.type = command.getCommandType();
                        emit responseBusiness(resp);
                    }
                    break;
                case BusinessClass::OSCILL_SAMPLE_STOP:
                    if(res==-1){
                        MY_DEBUG << "send stop oscill command failed";
                        netError=true;
                        BusinessResponse resp;
                        resp.errorID = BusinessResponse::NET_ERROE;
                        resp.errorMsg = QString::fromLocal8Bit("停止示波器失败：网络错误");
                        resp.result=false;
                        resp.type = command.getCommandType();
                        emit responseBusiness(resp);
                    }
                    break;
                default:
                    break;
                }
            }

            //2)读取下位机数据
            if(m_tcpSocket->waitForReadyRead(50)){
                char head[6] = {0};
                int frameLen=0;
                int frameLenBak=0;
                bool isReadFrameOk=false;

                res = m_tcpSocket->read(head, sizeof(head));
                if(res == -1){
                    MY_DEBUG << "read oscill head failed, net error";
                    netError=true;
                    continue;
                }
                //                MY_DEBUG;
                if(head[2] == 0x55 && (uint8_t)(head[3]) == 0xAA){  //数据头

                    //数据帧长度
                    frameLen = (uint8_t)(head[4]);
                    frameLen = frameLen << 8 | (uint8_t)(head[5]);
                    frameLenBak=frameLen;
                    char *frameBuf = (char*)malloc(frameLen);
                    if(frameBuf==NULL){
                        MY_DEBUG << "malloc failed";
                    }
                    memset(frameBuf, 0, frameLenBak);
                    res = 0;
                    int frameBufStart=0;

                    //读取帧数据
                    while(!this->isThreadQuit && !tcpIPChange && !netError)
                    {
                        res = m_tcpSocket->read(frameBuf+frameBufStart, frameLen);
                        if(res==-1){
                            MY_DEBUG << "read oscill frame failed, net error";
                            netError=true;
                            continue;
                        }
                        frameBufStart += res;
                        if(res < frameLen){
                            frameLen -= res;
                            m_tcpSocket->waitForReadyRead(50);  //等待下一帧到来
                            continue;
                        }else{
                            isReadFrameOk=true;
                            break;
                        }
                    }

                    //组建响应数据
                    if(isReadFrameOk){
                        //组件并发送数据响应信号
                        BusinessResponse resp;
                        if(newXCount<0 || newXCount==FLT_MAX){
                            newXCount=0;
                        }

                        //组件响应并发送响应
                        resp.buildOscillResponseData(frameBuf, frameLenBak, newXCount);
                        emit responseBusiness(resp);

                    }
                    free(frameBuf);
                }else{
                    MY_DEBUG << "not find head";
                }
            }
            thread()->msleep(10);
        }//end of serend while

        //3)去监听网络连接错误等释放工作
        disconnect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                   this,SLOT(on_socketError(QAbstractSocket::SocketError)));
        ipPortChange=false;
        netError=false;
        isLink=false;
        emit netLinkStatusChange(false);
        //继续新一轮
        thread()->msleep(10);
    }//end of first while
}

void OscillBusinessSigne::on_serverAddressUpdate()
{
    tcpIPChange=true;
}

//网络0错误
void OscillBusinessSigne::on_socketError(QAbstractSocket::SocketError error)
{
    //        MY_DEBUG << error;
    if(error != QAbstractSocket::SocketTimeoutError){
        netError=true;
    }
}
