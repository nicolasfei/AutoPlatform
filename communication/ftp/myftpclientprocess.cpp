#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#ifndef _MSC_VER
#include <unistd.h>
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

#if defined(_WIN32)
#include <io.h>
#include <process.h>
#include <ws2def.h>
#include <direct.h>
#endif

#include "config.h"
#include "data_trans.h"
#include "adapter.h"
#include "ftp_server.h"

#include "myftpserver.h"
#include "myftpclientprocess.h"
#include "common/commonclass.h"
#include "common/cabinetmanagement.h"

MyFtpClientProcess::MyFtpClientProcess(SOCKET fd,struct sockaddr_in addr, QObject *parent)
    : QThread(parent)
{
    this->connect_fd = fd;
    this->client = addr;
}

void MyFtpClientProcess::run()
{
    struct usr_info my_usr;

    char rcv_buf[512];
    char err_buf[5];
    char port_client_ip[16];
    int port_client_port;
    int count = 0;
    int cmd_type, login = 0;
    int i;
    int mode = PASV;
    int type = TYPEI;
    char log[512];
    char *ip1, *ip2, *ip3, *ip4,*port_h, *port_l;

    snprintf(log, sizeof(log), "connection from %s", inet_ntoa(client.sin_addr));
    //    do_log(log);

    memset(&(my_usr), '\0', sizeof(my_usr));
    MY_DEBUG << "220 " << "FTP server Ready start read-->" << connect_fd;
    while(1)
    {
        //        for(i = 0; i < 512; i++)
        //        {
        //            rcv_buf[i] ='\0';
        //        }
        memset(rcv_buf, '\0', sizeof (rcv_buf));

        count = _read(connect_fd, rcv_buf, 510);
        if(count <= 0)
            break;

        rcv_buf[strlen(rcv_buf)-2] = '\0';
        MY_DEBUG << QString::fromUtf8(rcv_buf);
        cmd_type = check_usr_cmd(rcv_buf);
        switch(cmd_type)
        {
        case 1:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            server_list(connect_fd,&my_usr, mode, port_client_ip, port_client_port);
            break;
        case 2:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            server_pwd(connect_fd, &my_usr);
            break;
        case 3:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            server_cwd(connect_fd, &my_usr, rcv_buf);
            break;
        case 4:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            if (my_usr.level < 2)
            {
                send_reply("550 ", "Permission denied", connect_fd);
            }
            else
            {
                get_file_location(&my_usr);
                store_file(get_file_name(), get_rest_offset(), get_data_sock(mode, port_client_ip, port_client_port, connect_fd), connect_fd);
            }
            break;
        case 5:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            if (my_usr.level != 1 && my_usr.level != 3)
            {
                send_reply("550 ", "Permission denied.", connect_fd);
                break;
            }
            get_file_location(&my_usr);
            if (local_is_dir(get_file_name()))
            {
                send_reply("550 ", "Target is directory.", connect_fd);
                break;
            }

            retr_file(get_file_name(), get_rest_offset(), get_data_sock(mode, port_client_ip, port_client_port, connect_fd), connect_fd);

            break;
        case 45:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            send_reply(get_rest_reply(), NULL, connect_fd);
            break;
        case 6:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            send_reply("221 ", "Goodbye~", connect_fd);
            _close(connect_fd);
            snprintf(log,sizeof(log),"user %s from %s, quit",\
                     my_usr.usr, inet_ntoa(client.sin_addr));
            do_log(log);
            break;
        case 7:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            server_size(rcv_buf, connect_fd);
            break;
        case 8:
            if(!login)
            {
                send_reply("550 ", "Please login first!", connect_fd);
                break;
            }
            if(rcv_buf[6] == 'A')
            {
                type = TYPEA;
                send_reply("200 ", "TYPE set to A", connect_fd);
            }
            else
            {
                type =  TYPEI;
                send_reply("200 ", "TYPE set to I", connect_fd);
            }
            break;
        case 11:
            memset(&(my_usr.usr), '\0', sizeof(my_usr.usr));
            snprintf(my_usr.usr, strlen(rcv_buf) - 4, rcv_buf+5);
            if(check_usr_passwd(&my_usr) == 1)
            {
                send_reply("331 ", "username OK, please send your password" ,connect_fd);
            }
            else
            {
                send_reply("332 ","username ERROR!", connect_fd);

            }
            break;
        case 12:
            memset(&(my_usr.passwd), '\0', sizeof(my_usr.passwd));
            snprintf(my_usr.passwd, strlen(rcv_buf) - 4, rcv_buf+5);
            if(check_usr_passwd(&my_usr) == 2)
            {
                send_reply("230 ", "User login!" ,connect_fd);
                snprintf(log,sizeof(log),"user %s from %s, login",\
                         my_usr.usr, inet_ntoa(client.sin_addr));
                do_log(log);
                login = 1;
            }
            else
            {
                send_reply("530 ","password ERROR!", connect_fd);
                snprintf(log,sizeof(log),"user %s from %s, password error",\
                         my_usr.usr, inet_ntoa(client.sin_addr));
                do_log(log);

            }
            break;
        case 21:
            mode = PASV;
            open_pasv_mode(connect_fd);
            break;
        case 22:
            ip1 = strchr(rcv_buf, ' ');
            ip1 ++;
            ip2 = strchr(ip1, ',');
            *ip2 = '\0';
            ip2++;
            ip3 = strchr(ip2, ',');
            *ip3 = '\0';
            ip3++;
            ip4 = strchr(ip3, ',');
            *ip4 = '\0';
            ip4++;
            port_h = strchr(ip4, ',');
            *port_h = '\0';
            port_h ++;
            port_l =  strchr(port_h, ',');
            *port_l = '\0';
            port_l ++;
            port_client_port = atoi(port_h)*256 + atoi(port_l);
            snprintf(port_client_ip, sizeof(port_client_ip), \
                     "%s.%s.%s.%s", ip1, ip2, ip3, ip4);
            if(strncmp(port_client_ip, (char *)inet_ntoa(client.sin_addr),strlen(port_client_ip)))
            {
                send_reply("500 Illegal PORT command: ", NULL, connect_fd);
            }
            mode = PORT;
            send_reply("200 ", "PORT command successful", connect_fd);
            break;
        default:
            snprintf(err_buf, 4, rcv_buf);
            send_reply("500 Illegal command: ", err_buf, connect_fd);
        }
    }
}
