#include "devicemanager.h"
#include "common/tool.h"

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{

}

void DeviceManager::updateArmIpPort(QString ip,int armPort, int tcpPort, int ftpPort)
{
    if(ip.compare(this->armIP)!=0)
    {
        this->armIP = ip;
        this->armPort = armPort;
        this->armTcpPort = armPort;
        this->armFtpPort = ftpPort;
        emit armIpPortChange(this->armIP,this->armPort);
        emit armTcpIpPortChange(this->armIP,this->armTcpPort);
        emit armFtpIpPortChange(this->armIP,this->armFtpPort);
    }else{
        if(this->armPort!=armPort){
            this->armPort = armPort;
            emit armIpPortChange(this->armIP,this->armPort);
        }
        if(this->armTcpPort!=tcpPort){
            this->armTcpPort = tcpPort;
            emit armTcpIpPortChange(this->armIP,this->armTcpPort);
        }
        if(this->armFtpPort!=ftpPort){
            this->armFtpPort = ftpPort;
            emit armFtpIpPortChange(this->armIP,this->armFtpPort);
        }
    }
}

void DeviceManager::updateNetIpPort(QString ip,int port)
{
    if(ip.compare(this->netIP)!=0 || (port>0 && port!=this->netPort))
    {
        this->netIP = ip;
        this->netPort = port;
        emit netIpPortChange(this->netIP,this->netPort);
    }
}

void DeviceManager::updateEngineIpPort(QString ip,int port)
{
    if(ip.compare(this->engineIP)!=0 || (port>0 && port!=this->enginePort))
    {
        this->engineIP = ip;
        this->enginePort = port;
        emit engineIpPortChange(this->engineIP,this->armPort);
    }
}
