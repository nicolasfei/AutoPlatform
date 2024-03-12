#include "waitdialog.h"
#include "ui_waitdialog.h"

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitDialog)
{
    ui->setupUi(this);
    setWindowOpacity(0.5); //设置透明用；
    this->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);

    //取消对话框标题
    //this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    //取消对话框标题和边框
    //this->setAutoFillBackground(true);
    this->setContentsMargins(0,0,0,0);
    ui->label->setContentsMargins(0,0,0,0);
    /*QPalettepalette;palette.setBrush(QPalette::Background,QBrush(QPixmap("E:/qml/imgdialog/loading.gif")));
            this->setPalette(palette)*/;
    movie = new QMovie(":/animations/animations/loading.gif");
    ui->label->setMovie(movie);
//    ui->label_2->setText(QString::fromLocal8Bit("加载中..."));
    movie->start();
}

void WaitDialog::setHint(QString hint)
{
//    ui->label_2->setText(hint);
}

WaitDialog::~WaitDialog()
{
    delete movie;
    delete ui;
}
