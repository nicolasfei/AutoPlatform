#ifndef OSCILLDATASAVETASK_H
#define OSCILLDATASAVETASK_H

#include <QThread>
#include <QMutex>
#include "communication/businessresponse.h"



class OscillDataSaveTask : public QThread
{
    Q_OBJECT
public:
    explicit OscillDataSaveTask(QObject *parent = nullptr);
    ~OscillDataSaveTask();

    void run() override;
    void inQueue(BusinessResponse resp);
    void taskFinish();
private:
    QList<BusinessResponse> *m_dataQueue;  //数据队列
    QMutex m_qmutex;    //锁
    bool isQuit;
    bool isCainetSwitch;
};

#endif // OSCILLDATASAVETASK_H
