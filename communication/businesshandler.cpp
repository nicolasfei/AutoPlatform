#include "businesshandler.h"
#include "common/commonclass.h"
#include <QDebug>

BusinessHandler::BusinessHandler(QObject *parent)
    : QThread(parent)
    , m_currentBis()
{
    qRegisterMetaType<BusinessResponse>("BusinessResponse");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

    this->isThreadQuit = false;
    this->ipPortChange = false;
    this->cabinetSwitch = false;
    this->netError=false;
    this->is_pause=true;
    this->isLink=false;
}

BusinessHandler::~BusinessHandler()
{
    isThreadQuit = true;
    this->quit();
}

void BusinessHandler::handleBusiness(BusinessClass vo)      //业务处理，异步响应
{
    //判断是否是本类处理的业务
    if(vo.getLevel() == this->level){
        if(!isLinked()){
            BusinessResponse resp;
            resp.errorID = BusinessResponse::NET_ERROE;
            resp.errorMsg = QString::fromLocal8Bit("网络连接错误");
            resp.result=false;
            resp.type = vo.getCommandType();
            emit responseBusiness(resp);
        }else{
            m_mutex.lock();      //队列上锁
            //只保留50个命令
            if(this->m_cmdQueue.size()>50)
            {
                this->m_cmdQueue.clear();
            }
            this->m_cmdQueue.enqueue(vo);        //业务加入队列
            m_mutex.unlock();
        }
    }else{
        if(this->nextOperator != NULL){
            this->nextOperator->handleBusiness(vo);
        }else{
            BusinessResponse resp;
            resp.errorID = BusinessResponse::NOT_HANDLER;
            resp.errorMsg = QString::fromLocal8Bit("无业务处理者");
            resp.result=false;
            resp.type = vo.getCommandType();
            emit responseBusiness(resp);
        }
    }
}

BusinessClass BusinessHandler::outQueue()
{
    BusinessClass bs;
    m_mutex.lock();
    if(this->m_cmdQueue.size()>0){
        bs = m_cmdQueue.dequeue();
    }
    m_mutex.unlock();
    return bs;
}

void BusinessHandler::setNext(BusinessHandler *_operator)   //设置下一个处理者
{
    this->nextOperator = _operator;
}

void BusinessHandler::on_serverAddressUpdate()         //服务器地址发送改变槽函数
{
    this->ipPortChange = true;
}

/**
 * @brief on_cabinetIdSwitch--机柜被切换了
 * @param cabinetID 新的机柜ID
 */
void BusinessHandler::on_cabinetIdSwitch()
{
    this->cabinetSwitch=true;
}

/**
 * @brief BusinessHandler::onSocketError--网络错误
 * @param err errID
 */
void BusinessHandler::on_socketError(QAbstractSocket::SocketError err)
{
    //    this->netError=true;
    QString netErr = QString::fromLocal8Bit("网络错误:").append(QString::number(err)).append("!");
    //    emit errorBusiness(NET_ERROE,netErr);
}
