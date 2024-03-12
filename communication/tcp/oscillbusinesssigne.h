#ifndef OSCILLBUSINESSSIGNE_H
#define OSCILLBUSINESSSIGNE_H

#include "communication/businesshandler.h"
#include <QObject>

class OscillBusinessSigne : public BusinessHandler
{
    Q_OBJECT
public:
    explicit OscillBusinessSigne(QObject *parent = nullptr);
    ~OscillBusinessSigne();

    enum OSCILL_STATUS{
        NO_TASK=0,
        CONFIG_STATUS,
        RUNNING_STATUS
    };
private:
    QTcpSocket *m_tcpSocket;
    bool stopThread=false;
    bool tcpIPChange=false;
    QList<ParameterClass::RegisterData> m_regList;        //柜1的通道配置寄存器数据
    bool isCabinetStart=false;      //柜1是否启动数据读取
    OSCILL_STATUS status;

    void run() override;
    QByteArray buildRequestData(BusinessClass::COMMAND_TYPE type,QList<ParameterClass::RegisterData> dataList);
private slots:
    /**
     * @brief on_serverAddressUpdate -- ip地址改变
     */
    void on_serverAddressUpdate();
    //网络0错误
    void on_socketError(QAbstractSocket::SocketError error);
};

#endif // OSCILLBUSINESSSIGNE_H
