#include "mainwindow.h"
#include "login.h"
#include "common/commonclass.h"
#include "common/commondialog.h"

#include <QApplication>
#include <QTextCodec>

#include "windows.h"
#include <Windows.h>//这个一定要添加，不然会报"No Target Architecture" 错误
#include <WinUser.h>
#include <wingdi.h>
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);//控制图片缩放质量

    int width = GetSystemMetrics(SM_CXSCREEN);//这个是Windows平台用来获取屏幕宽度的代码，因为在qApplication实例初始化之前，QGuiApplication::screens();无法使用。
    int height = GetSystemMetrics(SM_CYSCREEN);
    //    HWND hwd = GetDesktopWindow();
    //    HDC hdc = GetDC(hwd);
    //    int width = GetDeviceCaps(hdc, DESKTOPHORZRES);
    //    int height = GetDeviceCaps(hdc, DESKTOPVERTRES);
    MY_DEBUG << "CX is " << width;
    MY_DEBUG << "Cy is " << height;
//    if(width < 1366){
//        CommonDialog::showErrorDialog(NULL, QString::fromLocal8Bit("不支持的分辨率，软件支持的最小分辨率为1366x768"));
//    }
    qreal scalex = ((qreal)width) / 1366;
    qreal scaley = ((qreal)height) / 768;
    MY_DEBUG << "scaleX is " << scalex;
    MY_DEBUG << "scaley is " << scaley;
    qreal scale = qMax(scalex, scaley);
    if(scale<1){
        scale=1;
    }else if(scale<1.4){
        scale=1.4;
    }
    MY_DEBUG << "QString::number(scale) is " << QString::number(scale);
//    char *tmp = QString::number(scalex).toLatin1();
    qputenv("QT_SCALE_FACTOR", QString::number(scale).toLatin1());

    QApplication a(argc, argv);
    Login login;
    MainWindow mainWindow;

    QObject::connect(&login, &Login::showMain, &mainWindow, &MainWindow::receiverShow);
    QObject::connect(&mainWindow, &MainWindow::showLogin, &login, &Login::receiverShow);
    QObject::connect(&login, &Login::quit, &a, &QApplication::quit);
    QObject::connect(&mainWindow, &MainWindow::quit, &a, &QApplication::quit);

    login.show();
    return a.exec();
}
