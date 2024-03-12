#include "timertask.h"
#include "common/commonclass.h"

TimerTask::TimerTask(QObject *parent) : QObject(parent)
{
    this->m_taskTimer = new QTimer(this);
    this->m_taskTimer->setInterval(CommonClass::ACTIVITY_FRESH_DELAY);        //200ms延迟
    connect(this->m_taskTimer, &QTimer::timeout, this, &TimerTask::myTimeOut);        //定时任务响应
    this->m_taskTimer->start();
}

TimerTask::~TimerTask()
{
    this->m_taskTimer->stop();
}

void TimerTask::myTimeOut()
{
    emit this->taskIsTime();
}
