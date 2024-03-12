#include "mywaitdialog.h"

MyWaitDialog::MyWaitDialog(QWidget*parent):
    QDialog(parent)
{
    label = new QLabel(this);
    this->setFixedSize(180,180);

    setWindowOpacity(0.5); //设置透明用；
    this->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);

    //取消对话框标题
    //this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    //取消对话框标题和边框
    //this->setAutoFillBackground(true);
//    this->setContentsMargins(0,0,0,0);
//    label->setContentsMargins(0,0,0,0);
    /*QPalettepalette;palette.setBrush(QPalette::Background,QBrush(QPixmap("E:/qml/imgdialog/loading.gif")));
            this->setPalette(palette)*/;
    movie = new QMovie(":/animations/animations/loading.gif");
    movie->setScaledSize(this->size());
    label->setMovie(movie);
    movie->start();
}

MyWaitDialog::~MyWaitDialog()
{
    delete label;
    delete movie;
}
