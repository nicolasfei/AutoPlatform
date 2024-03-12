#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QMutex>

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    static DeviceManager* instance(){
        static QMutex mutex;
        static QScopedPointer<DeviceManager> inst;
        if (Q_UNLIKELY(!inst)) {
            mutex.lock();
            if (!inst) {
                inst.reset(new DeviceManager);
            }
            mutex.unlock();
        }
        return inst.data();
    }

    void updateArmIpPort(QString armIP,int armPort, int tcpPort, int ftpPort);
    void updateNetIpPort(QString armIP,int armPort);
    void updateEngineIpPort(QString armIP,int armPort);

    QString getArmIP()
    {
        return this->armIP;
    }
    int getArmPort()
    {
        return this->armPort;
    }
    int getArmTcpPort()
    {
        return this->armTcpPort;
    }
    int getArmFtpPort()
    {
        return this->armFtpPort;
    }

    QString getNetIP()
    {
        return this->netIP;
    }
    int getNetPort()
    {
        return this->netPort;
    }

    QString getEngineIP()
    {
        return this->engineIP;
    }
    int getEnginePort()
    {
        return this->enginePort;
    }

private:
    explicit DeviceManager(QObject *parent = nullptr);
    DeviceManager(const DeviceManager &);//禁止拷贝构造函数。
    DeviceManager & operator=(const DeviceManager &);   //禁止赋值拷贝函数。

    //arm主控ip,port
    QString armIP = "192.168.0.249";
    int armPort = 502;      //modbusTCP
    int armTcpPort = 8090;  //动态示波器
    int armFtpPort = 8060;  //FTP
    //网侧ip,port
    QString netIP;
    int netPort;
    //机侧ip,port
    QString engineIP;
    int enginePort;

signals:
    void armIpPortChange(QString armIP,int armPort);
    void armTcpIpPortChange(QString armIP,int armTcpPort);
    void armFtpIpPortChange(QString armIP,int armFtpPort);
    void netIpPortChange(QString armIP,int armPort);
    void engineIpPortChange(QString armIP,int armPort);
};

#endif // DEVICEMANAGER_H
