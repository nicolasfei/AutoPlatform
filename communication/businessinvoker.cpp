#include "businessinvoker.h"
#include "tcp/oscillbusinesssigne.h"
#include "ftp/wavebusiness.h"
#include "ftp/myftpserver.h"
#include "modbus/modbusbusiness.h"

BusinessInvoker::BusinessInvoker(QObject *parent) : QObject(parent)
{
    this->tcpHandler = new OscillBusinessSigne();
    this->modbusHandler = new modbusBusiness();
    this->waveHandler = new WaveBusiness();

    connect(tcpHandler,&BusinessHandler::responseBusiness,this,&BusinessInvoker::on_ResponseBusiness);
//    connect(tcpHandler,&BusinessHandler::errorBusiness,this,&BusinessInvoker::on_ResponseError);
    connect(tcpHandler, &BusinessHandler::finished, tcpHandler, &QObject::deleteLater);

    connect(modbusHandler,&BusinessHandler::responseBusiness,this,&BusinessInvoker::on_ResponseBusiness);
//    connect(modbusHandler,&BusinessHandler::errorBusiness,this,&BusinessInvoker::on_ResponseError);
    connect(modbusHandler, &BusinessHandler::finished, modbusHandler, &QObject::deleteLater);

    connect(waveHandler,&BusinessHandler::responseBusiness,this,&BusinessInvoker::on_ResponseBusiness);
//    connect(waveHandler,&BusinessHandler::errorBusiness,this,&BusinessInvoker::on_ResponseError);
    connect(waveHandler, &BusinessHandler::finished, waveHandler, &QObject::deleteLater);

    connect(tcpHandler, &BusinessHandler::netLinkStatusChange, this, &BusinessInvoker::on_NetLinkStatusChange);
    connect(modbusHandler, &BusinessHandler::netLinkStatusChange, this, &BusinessInvoker::on_NetLinkStatusChange);

    tcpHandler->setNext(modbusHandler);
    modbusHandler->setNext(waveHandler);
    waveHandler->setNext(NULL);
    this->handler = tcpHandler;

    this->tcpHandler->start(QThread::NormalPriority);
    this->tcpHandler->pause();
    this->modbusHandler->start(QThread::NormalPriority);
    this->modbusHandler->pause();
    this->waveHandler->start(QThread::NormalPriority);
    this->waveHandler->pause();

    //MyFtpServer *ftp = new MyFtpServer();   //启动FTP服务器
    //ftp->start(QThread::NormalPriority);
}

BusinessInvoker::~BusinessInvoker()
{
    disconnect(tcpHandler,&BusinessHandler::responseBusiness,this,&BusinessInvoker::on_ResponseBusiness);
//    disconnect(tcpHandler,&BusinessHandler::errorBusiness,this,&BusinessInvoker::on_ResponseError);

    disconnect(modbusHandler,&BusinessHandler::responseBusiness,this,&BusinessInvoker::on_ResponseBusiness);
//    disconnect(modbusHandler,&BusinessHandler::errorBusiness,this,&BusinessInvoker::on_ResponseError);

    disconnect(waveHandler,&BusinessHandler::responseBusiness,this,&BusinessInvoker::on_ResponseBusiness);
//    disconnect(waveHandler,&BusinessHandler::errorBusiness,this,&BusinessInvoker::on_ResponseError);

    disconnect(tcpHandler, &BusinessHandler::netLinkStatusChange, this, &BusinessInvoker::on_NetLinkStatusChange);
    disconnect(modbusHandler, &BusinessHandler::netLinkStatusChange, this, &BusinessInvoker::on_NetLinkStatusChange);

    delete this->tcpHandler;
    delete this->modbusHandler;
    delete this->waveHandler;
}

void BusinessInvoker::exec(BusinessClass req)
{
    this->handler->handleBusiness(req);
}

void BusinessInvoker::suspend(BusinessClass::LEVEL level)
{
    switch(level)
    {
    case BusinessClass::MODBUS_HANDLER:
        this->modbusHandler->pause();
        break;
    case BusinessClass::TCP_IP_HANDLER:
        this->tcpHandler->pause();
        break;
    case BusinessClass::FTP_HANDLER:
        this->waveHandler->pause();
        break;
    default:
        break;
    }
}

void BusinessInvoker::wakeup(BusinessClass::LEVEL level)
{
    switch(level)
    {
    case BusinessClass::MODBUS_HANDLER:
        this->modbusHandler->resume();
        break;
    case BusinessClass::TCP_IP_HANDLER:
        this->tcpHandler->resume();
        break;
    case BusinessClass::FTP_HANDLER:
        this->waveHandler->resume();
        break;
    default:
        break;
    }
}

/**
 * @brief isLink--是否连接
 * @param level
 */
bool BusinessInvoker::isLink(BusinessClass::LEVEL level)
{
    bool link=false;
    switch(level)
    {
    case BusinessClass::MODBUS_HANDLER:
        link = this->modbusHandler->isLinked();
        break;
    case BusinessClass::TCP_IP_HANDLER:
        link = this->tcpHandler->isLinked();
        break;
    case BusinessClass::FTP_HANDLER:
        link = this->waveHandler->isLinked();
        break;
    default:
        break;
    }
    return link;
}
/**
 * @brief BusinessInvoker::on_ResponseBusiness
 * @param rep
 */
void BusinessInvoker::on_ResponseBusiness(BusinessResponse rep)
{
    emit response(rep);
}

///**
// * @brief BusinessInvoker::on_ResponseError 接收到执行错误
// * @param errorID
// * @param errorMsg
// */
//void BusinessInvoker::on_ResponseError(int errorID, BusinessResponse rep, QString errorMsg)
//{
//    emit response(false, BusinessResponse(), errorID, errorMsg);
//}

/**
 * @brief BusinessInvoker::on_NetLinkStatusChange--网络连接状态
 * @param netLinked
 */
void BusinessInvoker::on_NetLinkStatusChange(bool netLinked)
{
    emit netStates(netLinked);
}
