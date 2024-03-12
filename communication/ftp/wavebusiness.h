#ifndef WAVEBUSINESS_H
#define WAVEBUSINESS_H

#include <QTcpServer>
#include "communication/businesshandler.h"
#include "mytcpserver.h"
#include "mytcpsocket.h"

class WaveBusiness : public BusinessHandler
{
    Q_OBJECT
public:
    explicit WaveBusiness(QObject *parent = nullptr);

    bool isLinked() override;
private:
    MyTcpServer *m_tcpServer;
    bool stopThread=false;
    QString downParameterFile;

    void run() override;

    QByteArray buildRequestData(BusinessClass::COMMAND_TYPE type, QList<QString> dataList);
    bool writeClientData(QByteArray request, BusinessClass::COMMAND_TYPE updateID);

protected slots:
    void on_clientUploadData(BusinessResponse resp);
};

#endif // WAVEBUSINESS_H
