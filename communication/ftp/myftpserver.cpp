#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#if defined(_WIN32)
#include <io.h>
#include <process.h>
#include <direct.h>
#endif

#if defined(_WIN32)
/* Already set in modbus-tcp.h but it seems order matters in VS2005 */
# include <winsock2.h>
# include <ws2tcpip.h>
# define SHUT_RDWR 2
//# define close closesocket
#else
# include <sys/socket.h>
# include <sys/ioctl.h>

#if defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ < 5)
# define OS_BSD
# include <netinet/in_systm.h>
#endif

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#include "config.h"
#include "ftp_server.h"
#include "data_trans.h"
#include "adapter.h"
#include "common/commonclass.h"
#include "common/cabinetmanagement.h"
#include "myftpclientprocess.h"
#include "myftpserver.h"

MyFtpServer::MyFtpServer(QObject *parent) : QThread(parent)
  ,isQuit(false)
{

}

MyFtpServer::~MyFtpServer()
{
    this->isQuit=true;
    this->quit();
}

void MyFtpServer::run()
{
    int opt;
    int oldmask;
    SOCKET listen_fd, connect_fd;
    int sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in server, client;

    BOOL bOptVal = FALSE;
    int bOptLen = sizeof (BOOL);

    _chdir(WORK_HOME);
    _umask_s(_S_IREAD|_S_IWRITE, &oldmask);

    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        MY_DEBUG << "Creating socket failed. 600";
        return;
    }

    opt = SO_REUSEADDR;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal, bOptLen))
    {
        MY_DEBUG << "setsockopt() " << strerror(errno);
    }

    //fill the server information
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

//    CabinetManagement *manager = CabinetManagement::instance();
    //ip addr port
    server.sin_port = htons(CONTROL_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listen_fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        MY_DEBUG << "bind() error! 601->" << strerror(errno);
        return;
    }
    if(listen(listen_fd, MAX_LOGIN) == -1)
    {
        MY_DEBUG << "listen() error! 602->" << strerror(errno);
        return;
    }
    MY_DEBUG << "Start ftp_server.";
    while(!this->isQuit)
    {
        connect_fd = accept(listen_fd, (struct sockaddr *)&client, (socklen_t *)&sin_size);
        if(connect_fd == -1)
        {
            MY_DEBUG <<  "accept() error: " << strerror(errno) << errno;
            continue;
        }
        MyFtpClientProcess *clientProcess = new MyFtpClientProcess(connect_fd, client);
        clientProcess->start();
//        if((client_pid = fork()) > 0)
//        {
//            _close(connect_fd);
//            continue;
//        }
//        else
//        {
//            if(client_pid == 0)
//            {
//                _close(listen_fd);
//                client_process(connect_fd, client);
//                exit(0);
//            }
//            else
//            {
//                MY_DEBUG << "fork() error!";
//                _close(connect_fd);
//                continue;
//            }
//        }
    }
}
