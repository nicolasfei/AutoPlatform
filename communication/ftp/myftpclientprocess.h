#ifndef MYFTPCLIENTPROCESS_H
#define MYFTPCLIENTPROCESS_H

#include <QThread>
#include <WinSock2.h>

class MyFtpClientProcess : public QThread
{
    Q_OBJECT
public:
    explicit MyFtpClientProcess(SOCKET client,struct sockaddr_in addr,  QObject *parent = nullptr);

private:
    void run() override;

private:
    struct sockaddr_in client;
    SOCKET connect_fd;
};

#endif // MYFTPCLIENTPROCESS_H
