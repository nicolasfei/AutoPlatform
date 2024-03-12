#ifndef TIMERTASK_H
#define TIMERTASK_H

#include <QObject>
#include <QMutex>
#include <QTimer>

class TimerTask : public QObject
{
    Q_OBJECT
public:
    static TimerTask* instance(){
        static QMutex mutex;
        static QScopedPointer<TimerTask> inst;
        if (Q_UNLIKELY(!inst)) {
            mutex.lock();
            if (!inst) {
                inst.reset(new TimerTask);
            }
            mutex.unlock();
        }
        return inst.data();
    }

    ~TimerTask();

private:
    explicit TimerTask(QObject *parent = nullptr);      //禁止构造函数。
    TimerTask(const TimerTask &);               //禁止拷贝构造函数。
    TimerTask & operator=(const TimerTask &);   //禁止赋值拷贝函数。

    QTimer* m_taskTimer;    //任务定时器
signals:
    void taskIsTime();          //到了任务时间
private slots:
    /**
     * @brief myTimeOut--数据查询任务
     */
    void myTimeOut();
};

#endif // TIMERTASK_H
