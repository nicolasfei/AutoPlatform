#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QTcpServer>
#include <QHash>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr, int numConnections = 2);
    ~MyTcpServer();
    void setMaxPendingConnections(int numConnections);//重写设置最大连接数函数
    bool issueInstructions(QString addr, QByteArray data, BusinessClass::COMMAND_TYPE updateID);
    bool isHaveClient();    //是否有客户端连接

signals:
    void connectClient(const int , const QString & ,const quint16 );//发送新用户连接信息
    void readData(const int,const QString &, quint16, const QByteArray &);//发送获得用户发过来的数据
//    void sockDisConnect(int ,QString ,quint16);//断开连接的用户信息
    void sentData(const QByteArray &,const int, BusinessClass::COMMAND_TYPE updateID);//向scoket发送消息
    void sentDisConnect(int i); //断开特定连接，并释放资源，-1为断开所有。
    void clientUploadData(BusinessResponse resp);   //上传客户端的数据

public slots:
    void clear(); //断开所有连接，线程计数器请0
protected slots:
    void sockDisConnectSlot(int handle,const QString & ip, quint16 prot, QThread *th);//断开连接的用户信息
    void on_uploadData(BusinessResponse resp);

protected:
    void incomingConnection(qintptr socketDescriptor);//覆盖已获取多线程
private:
    QHash<int,MyTcpSocket *> * tcpClient;//管理连接的map
    int maxConnections;
};

#endif // MYTCPSERVER_H
