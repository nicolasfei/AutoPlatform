#ifndef OSCILLDATAQUEUE_H
#define OSCILLDATAQUEUE_H

#include <QObject>
#include <QMutex>
#include <QQueue>

#include "communication/businessresponse.h"

class OscillDataQueue : public QObject
{
    Q_OBJECT
public:
    static OscillDataQueue* instance(){
        static QMutex mutex;
        static QScopedPointer<OscillDataQueue> inst;
        if (Q_UNLIKELY(!inst))
        {
            mutex.lock();
            if (!inst)
            {
                inst.reset(new OscillDataQueue);
            }
            mutex.unlock();
        }
        return inst.data();
    }

    void inQueue(BusinessResponse data)
    {
        m_mutex.lock();
        m_queue.enqueue(data);
        m_mutex.unlock();
    }
    BusinessResponse outQueue()
    {
        BusinessResponse data;
        m_mutex.lock();
        if(!m_queue.isEmpty()){
            data = m_queue.dequeue();
        }
        m_mutex.unlock();
        return data;
    }
private:
    QQueue<BusinessResponse> m_queue;
    QMutex m_mutex;

private:
    explicit OscillDataQueue(QObject *parent = nullptr);
    OscillDataQueue(const OscillDataQueue &);                   //禁止拷贝构造函数。
    OscillDataQueue & operator=(const OscillDataQueue &);       //禁止赋值拷贝函数。
};

#endif // OSCILLDATAQUEUE_H
