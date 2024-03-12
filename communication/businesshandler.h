#ifndef BUSINESSHANDLER_H
#define BUSINESSHANDLER_H

#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QMetaType>
#include <QWaitCondition>
#include "businessclass.h"
#include "businessresponse.h"
#include "common/commonclass.h"

class BusinessHandler : public QThread
{
    Q_OBJECT
public:
    explicit BusinessHandler(QObject *parent = nullptr);
    ~BusinessHandler();

public:
    void handleBusiness(BusinessClass vo);      //业务处理，异步响应
    void setNext(BusinessHandler *_operator);   //设置下一个处理者

    void resume()
    {
        sync.lock();
        is_pause = false;
        sync.unlock();
        pauseCond.wakeAll();
    }

    void pause()
    {
        sync.lock();
        is_pause = true;
        sync.unlock();
    }

    virtual bool isLinked()
    {
        return this->isLink;
    }
    BusinessClass outQueue();       //获取指令
private:
    BusinessHandler *nextOperator;      //下一个业务处理者

protected:
    QQueue<BusinessClass> m_cmdQueue;    //业务执行队列
    BusinessClass::LEVEL level;         //我能处理的业务级别
    QMutex m_mutex;                     //队列处理锁
    bool ipPortChange;          //ip,port改变
    bool cabinetSwitch;         //机柜被切换
    bool netError;
    bool isLink;

    BusinessClass m_currentBis;         //当前业务指令

    //线程run控制
    bool isThreadQuit;      //退出

    QMutex sync;
    QWaitCondition pauseCond;
    bool is_pause;
signals:
    void responseBusiness(BusinessResponse rep);            //业务响应信号
    void netLinkStatusChange(bool linkStatus);              //网络状态改变

public slots:
    void on_serverAddressUpdate();                          //服务器地址发送改变槽函数
    void on_socketError(QAbstractSocket::SocketError error);    //网络发生错误
    void on_cabinetIdSwitch();                              //机柜被切换了
};

#endif // BUSINESSHANDLER_H
