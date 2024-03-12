#include "mytcpserver.h"
#include "common/commonclass.h"

#include <QThread>
#include <QMetaType>

MyTcpServer::MyTcpServer(QObject *parent, int numConnections)
    : QTcpServer(parent)
{
    tcpClient = new QHash<int,MyTcpSocket *>();
    setMaxPendingConnections(numConnections);
}

MyTcpServer::~MyTcpServer()
{
    delete tcpClient;
}

void MyTcpServer::setMaxPendingConnections(int numConnections)
{
    this->QTcpServer::setMaxPendingConnections(numConnections);//调用Qtcpsocket函数，设置最大连接数，主要是使maxPendingConnections()依然有效
    this->maxConnections = numConnections;
}

/**
  * @brief isHaveClient--是否有客户端连接
  * @return
  */
bool MyTcpServer::isHaveClient()
{
    MY_DEBUG << this->tcpClient->size();
    return this->tcpClient->size()>0;
}

/**
 * @brief MyTcpServer::incomingConnection--覆盖已获取多线程，多线程必须在此函数里捕获新连接
 * @param socketDescriptor
 */
void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    MY_DEBUG << "find new socket link";
    if (tcpClient->size() > maxPendingConnections()){//继承重写此函数后，QTcpServer默认的判断最大连接数失效，自己实现
        QTcpSocket tcp;
        tcp.setSocketDescriptor(socketDescriptor);
        tcp.disconnectFromHost();
        return;
    }
    MyTcpSocket *tcpTemp = new MyTcpSocket(socketDescriptor);
    connect(tcpTemp,&MyTcpSocket::sockDisConnect,this,&MyTcpServer::sockDisConnectSlot);//NOTE:断开连接的处理，从列表移除，并释放断开的Tcpsocket，此槽必须实现，线程管理计数也是考的他
    connect(this,&MyTcpServer::sentDisConnect,tcpTemp,&MyTcpSocket::disConTcp);//断开信号

    qRegisterMetaType<BusinessClass::COMMAND_TYPE>("BusinessClass::COMMAND_TYPE");      //信号槽注册自定义参数类型
    connect(this, &MyTcpServer::sentData, tcpTemp, &MyTcpSocket::sentDataToClient);      //发送数据信号
    connect(tcpTemp, &MyTcpSocket::uploadData, this, &MyTcpServer::on_uploadData);      //上传客户端的数据到应用层

    QThread *th = new QThread(tcpTemp);

    QString ip =  tcpTemp->peerAddress().toString();
    qint16 port = tcpTemp->peerPort();

    MY_DEBUG << "add MyTcpSocket to thread";
    tcpTemp->moveToThread(th);//把tcp类移动到新的线程，从线程管理类中获取
    tcpClient->insert(socketDescriptor,tcpTemp);//插入到连接信息中
    th->start();
    emit connectClient(socketDescriptor,ip,port);
    //        emit newConnection();   //文档要求继承本函数需要发射此信号，此处没有与此信号连接的槽
}

/**
 * @brief MyTcpServer::issueInstructions--发送数据到客户端
 * @param data
 */
bool MyTcpServer::issueInstructions(QString addr, QByteArray data, BusinessClass::COMMAND_TYPE updateID)
{
//    MY_DEBUG;
    bool ret=false;
    if(tcpClient->size()>0){
//        MY_DEBUG << "find client";
        QHash<int,MyTcpSocket *>::const_iterator iter = tcpClient->constBegin();
        while(iter != tcpClient->constEnd()){
            MyTcpSocket * socket = iter.value();
            MY_DEBUG << socket->peerAddress().toString() << socket->peerPort();
            if(socket->peerAddress().toString() == addr){
                emit sentData(data, iter.key(), updateID);
                ret=true;
                break;
            }
            iter++;
        };
    }
    return ret;
}

void MyTcpServer::on_uploadData(BusinessResponse resp)
{
    MY_DEBUG;
    emit clientUploadData(resp);
}

/**
 * @brief MyTcpServer::sockDisConnectSlot--客户端断开连接
 * @param handle
 * @param ip
 * @param prot
 * @param th
 */
void MyTcpServer::sockDisConnectSlot(int handle,const QString & ip, quint16 prot,QThread * th)
{
    tcpClient->remove(handle);//连接管理中移除断开连接的socket
    //ThreadHandle::getClass().removeThread(th); //告诉线程管理类那个线程里的连接断开了
    th->exit(0);
    //    emit sockDisConnect(handle,ip,prot);
}

/**
 * @brief MyTcpServer::clear--清空
 */
void MyTcpServer::clear()
{
    emit this->sentDisConnect(-1);
    //    ThreadHandle::getClass().clear();
    tcpClient->clear();
}
