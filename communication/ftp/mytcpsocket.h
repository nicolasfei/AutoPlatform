#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QFutureWatcher>
#include <QQueue>
#include <QByteArray>
#include <QMetaObject>
#include <QString>

#include "communication/businessresponse.h"
#include "communication/businessclass.h"
#include "communication/businesshandler.h"

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(qintptr socketDescriptor, QObject *parent = nullptr);
    ~MyTcpSocket();

signals:
    //void readData(const int,const QString &,const quint16,const QByteArray &);
    void sockDisConnect(const int, const QString &, const quint16, QThread *);
    void uploadData(BusinessResponse resp);     //上传数据

public slots:
    void sentDataToClient(const QByteArray & ,const int, BusinessClass::COMMAND_TYPE updateID);
    void disConTcp(int i);

protected slots:
    void readData();//接收数据

private:
    qintptr socketID;
    QMetaObject::Connection dis;
    BusinessClass::COMMAND_TYPE currentUpdateID;        //当前升级ID---参数文件升级/固件升级
};

#endif // MYTCPSOCKET_H
