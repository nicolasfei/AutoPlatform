#ifndef BUSINESSINVOKER_H
#define BUSINESSINVOKER_H

#include <QObject>
#include <QMutex>
#include "businessclass.h"
#include "businesshandler.h"
#include "businessresponse.h"

class BusinessInvoker : public QObject
{
    Q_OBJECT
public:
    static BusinessInvoker* instance(){
        static QMutex mutex;
        static QScopedPointer<BusinessInvoker> inst;
        if (Q_UNLIKELY(!inst))
        {
            mutex.lock();
            if (!inst)
            {
                inst.reset(new BusinessInvoker);
            }
            mutex.unlock();
        }
        return inst.data();
    }
    ~BusinessInvoker();
    void exec(BusinessClass req);
    void suspend(BusinessClass::LEVEL level);
    void wakeup(BusinessClass::LEVEL level);
    bool isLink(BusinessClass::LEVEL level);
private:
    explicit BusinessInvoker(QObject *parent = nullptr);
    BusinessInvoker(const BusinessInvoker &);       //禁止拷贝构造函数。
    BusinessInvoker & operator=(const BusinessInvoker &);       //禁止赋值拷贝函数。
    BusinessHandler *handler;

    BusinessHandler *tcpHandler;
    BusinessHandler *modbusHandler;
    BusinessHandler *waveHandler;

signals:
    void response(BusinessResponse data);    //响应
    void netStates(bool linkStatus);

public slots:
    void on_ResponseBusiness(BusinessResponse rep);             //接收到执行响应
    void on_NetLinkStatusChange(bool netLinked);                //网络连接状态
};

#endif // BUSINESSINVOKER_H
