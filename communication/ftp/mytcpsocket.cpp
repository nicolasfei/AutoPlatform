#include "mytcpsocket.h"
#include "common/commonclass.h"
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QHostAddress>

MyTcpSocket::MyTcpSocket(qintptr socketDescriptor, QObject *parent) : QTcpSocket(parent),
    socketID(socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
    connect(this,&MyTcpSocket::readyRead,this,&MyTcpSocket::readData);
    dis = connect(this, &MyTcpSocket::disconnected, [&](){
        MY_DEBUG << "disconnect" ;
        emit sockDisConnect(socketID,this->peerAddress().toString(),this->peerPort(),QThread::currentThread());//发送断开连接的用户信息
        //        this->deleteLater();
    });
    MY_DEBUG << "new connect :" << this->peerAddress().toString() << this->peerPort();
}

MyTcpSocket::~MyTcpSocket()
{
}

void MyTcpSocket::sentDataToClient(const QByteArray &data, const int id, BusinessClass::COMMAND_TYPE updateID)
{
    if(id == socketID){
        this->currentUpdateID=updateID;
        write(data);
        flush();
    }
}

void MyTcpSocket::disConTcp(int i)
{
    if (i == socketID){
        this->disconnectFromHost();
    }
    else if (i == -1){ //-1为全部断开
        disconnect(dis); //先断开连接的信号槽，防止二次析构
        this->disconnectFromHost();
        this->deleteLater();
    }
}

void MyTcpSocket::readData()
{
    MY_DEBUG;
    uint16_t dataLen=0;
    char fileCount=0;
    QList<QString> list;

    //没有可读的数据就返回
    if(this->bytesAvailable()<=0){
        return;
    }
    QByteArray buf =  this->readAll();
    char *data = buf.data();
    //数据头
    if(data[0] == 0x68  && data[1] == 0x68){
        //高在前，低在后
        dataLen = data[3];
        dataLen = dataLen<<8|data[4];
        //功能码
        switch(data[2]){
        case 0x02:{          //龙芯回复目前有的故障文件名列表
            //文件个数
            fileCount=data[5];
            MY_DEBUG << "fault file Count is " << QString::number(fileCount);
            QByteArray frame = buf.mid(6);
            //注意收发两端文本要使用对应的编解码
            //1015 is UTF-16, 1014 UTF-16LE, 1013 UTF-16BE, 106 UTF-8)
            QString dataAsString = QTextCodec::codecForMib(106)->toUnicode(frame);
            for(int i=0; i<fileCount; i++)
            {
                QString faultFile = dataAsString.mid(i*27,27);
                MY_DEBUG << faultFile;
                list.append(faultFile);
            }
            //发送响应
            BusinessResponse resp;
            resp.type = BusinessClass::WAVE_QUERY;
            if(list.size()>0){
                resp.m_waveData.clear();
                resp.m_waveData.append(data);
                resp.result=true;
            }else{
                resp.result=false;
                resp.errorMsg = QString::fromLocal8Bit("故障录波文件为空！");
            }
            emit uploadData(resp);
        }
            break;
        case 0x03:{     //主控单元上传故障录波文件完成后给QT发送上送反馈码码07表示上传成功
            //发送响应
            BusinessResponse resp;
            resp.type = BusinessClass::WAVE_OBTAIN;
            resp.result = false;
            switch(data[5]){
            case 0x01:{     //01表示“确认收到启动开始下载帧”
                MY_DEBUG;
            }
                break;
            case 0x02:{     //02表示“账号/密码错误”
                resp.result = false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("账号/密码错误");
            }
                break;
            case 0x03:{     //03表示“路径不存在”
                resp.result = false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("路径不存在");
            }
                break;
            case 0x04:{     //04表示“确认登陆信息帧/确认升级变量正确”
                //emit uploadData(BusinessHandler::NOt_ERROR, resp, QString::fromLocal8Bit("确认登陆信息帧/确认升级变量正确"));
            }
                break;
            case 0x05:{     //05表示“crc文件校验错误”
                resp.result = false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("crc文件校验错误");
            }
                break;
            case 0x06:{     //06表示“回复下载成功”
                resp.result = true;
                resp.errorID = BusinessResponse::NOt_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("回复下载成功");
            }
                break;
            case 0x07:{     //07表示“上传完成”
                resp.result = true;
                resp.errorID = BusinessResponse::NOt_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("上传完成");
            }
                break;
            case 0x08:{     //08表示“ftp服务器不在线”
                resp.result = false;
                resp.errorID = BusinessResponse::NET_ERROE;
                resp.errorMsg = QString::fromLocal8Bit("ftp服务器不在线");
            }
                break;
            default:
                resp.result = false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("未知错误");
                break;
            }
            emit uploadData(resp);
        }
            break;
        case 0x01:{         //主控单元回复的配置文件升级/固件文件升级响应
            MY_DEBUG << (uint8_t)data[5];
            BusinessResponse resp;
            resp.type = this->currentUpdateID;
            resp.respType = data[5];
            switch(data[5]){
            case 0x01:{     //01表示“确认收到启动开始下载帧”
                MY_DEBUG;
                resp.result=true;
                resp.errorID = BusinessResponse::NOt_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("确认收到启动开始下载帧");
            }
                break;
            case 0x02:{     //02表示“账号/密码错误”
                resp.result=false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("账号/密码错误");
            }
                break;
            case 0x03:{     //03表示“路径不存在”
                resp.result=false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("路径不存在");
            }
                break;
            case 0x04:{     //04表示“确认登陆信息帧/确认升级变量正确”
                //                resp.result=true;
                //                resp.errorMsg = QString::fromLocal8Bit("确认登陆信息帧/确认升级变量正确");
            }
                break;
            case 0x05:{     //05表示“crc文件校验错误”
                resp.result=false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("crc文件校验错误");
            }
                break;
            case 0x06:{     //06表示“回复下载成功”
                resp.result=true;
                resp.errorID = BusinessResponse::NOt_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("回复下载成功");
            }
                break;
            case 0x07:{     //07表示“上传完成”
                resp.result=true;
                resp.errorID = BusinessResponse::NOt_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("上传完成");
            }
                break;
            case 0x08:{     //08表示“ftp服务器不在线”
                resp.result=false;
                resp.errorID = BusinessResponse::NET_ERROE;
                resp.errorMsg = QString::fromLocal8Bit("ftp服务器不在线");
            }
                break;
            default:
                resp.result = false;
                resp.errorID = BusinessResponse::DATA_ERROR;
                resp.errorMsg = QString::fromLocal8Bit("未知错误");
                break;
            }
            emit uploadData(resp);
        }
            break;
        default:
            MY_DEBUG;
            break;
        }

    }else{
        MY_DEBUG << "get client data is error format";
    }
}
