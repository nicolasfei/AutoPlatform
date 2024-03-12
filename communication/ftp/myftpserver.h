#ifndef MYFTPSERVER_H
#define MYFTPSERVER_H

#include "ftp_server.h"
#include "data_trans.h"
#include "adapter.h"
#include "config.h"
#include <QThread>

class MyFtpServer : public QThread
{
public:
    explicit MyFtpServer(QObject *parent = nullptr);
    ~MyFtpServer();
private:
    bool isQuit;
    void run() override;
};

#endif // MYFTPSERVER_H
