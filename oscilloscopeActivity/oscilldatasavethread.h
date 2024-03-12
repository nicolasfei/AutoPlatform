#ifndef OSCILLDATASAVETHREAD_H
#define OSCILLDATASAVETHREAD_H

#include <QThread>

#include "communication/businessresponse.h"

class OscillDataSaveThread : public QThread
{
    Q_OBJECT
public:
    explicit OscillDataSaveThread(QList<BusinessResponse::OscillResponse> &oscillData, QString savePath, QObject *parent = nullptr);

    void run() override;

signals:
     void resultReady(bool success, QString msg);

private:
     QList<BusinessResponse::OscillResponse> &oscillData;
     QString savePath;
};

#endif // OSCILLDATASAVETHREAD_H
