#include "waveitemwidget.h"
#include "ui_waveitemwidget.h"
#include <QInputDialog>
#include <QDir>

WaveItemWidget::WaveItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaveItemWidget)
{
    ui->setupUi(this);
}

WaveItemWidget::~WaveItemWidget()
{
    delete ui;
}

/**
 * @brief setData   设置数据
 * @param itemId    id
 * @param itemName  name
 * @param isShow    显示波形
 * @param isZoom    缩放
 */
void WaveItemWidget::setData(int itemId, uint32_t id, const QString& itemName, bool isShow, bool isFix, bool isZoom)
{
    this->mItemID = itemId;
    ui->label->setText(QString::number(this->mItemID));

    this->mItemName = itemName;
    ui->pushButton->setText(this->mItemName);

    this->mIsFixName = isFix;
    ui->pushButton->setEnabled(this->mIsFixName);     //是否可以修改通道名字

    this->id = id;  //标志

    if(isShow){
        ui->toolButton_2->setIcon(QIcon(":/images/images/eys_open.svg"));
    }else{
        ui->toolButton_2->setIcon(QIcon(":/images/images/eys_close.svg"));
    }
    this->mIsShow = isShow;

//    if(isZoom){
//        ui->toolButton_3->setIcon(QIcon(":/images/images/zoom_out_small.svg"));
//    }else{
//        ui->toolButton_3->setIcon(QIcon(":/images/images/zoom_in_small.svg"));
//    }
//    this->mIsZoom = isZoom;
//    ui->pushButton->
}

void WaveItemWidget::setShow(bool show)
{
    if(show){
        ui->toolButton_2->setIcon(QIcon(":/images/images/eys_open.svg"));
    }else{
        ui->toolButton_2->setIcon(QIcon(":/images/images/eys_close.svg"));
    }
    this->mIsShow = show;
}
/**
 * @brief WaveItemWidget::x 取消通道
 */
void WaveItemWidget::on_toolButton_clicked()
{
    emit deleteItem(this->id);  //发送信号
}

/**
 * @brief WaveItemWidget::睁眼/闭眼 -- 显示隐藏通道
 */
void WaveItemWidget::on_toolButton_2_clicked()
{
    if(this->mIsShow){
        ui->toolButton_2->setIcon(QIcon(":/images/images/eys_close.svg"));
        this->mIsShow = false;
    }else{
        ui->toolButton_2->setIcon(QIcon(":/images/images/eys_open.svg"));
        this->mIsShow = true;
    }
    emit isShowWave(this->id, this->mIsShow);
}

/**
 * @brief WaveItemWidget::on_pushButton_clicked--修改通道名字
 */
void WaveItemWidget::on_pushButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, QString::fromLocal8Bit("通道名修改"),
                                         QString::fromLocal8Bit("通道名:"), QLineEdit::Normal,
                                         mItemName, &ok, Qt::Dialog);
    if (ok && !text.isEmpty()){
        this->mItemName = text;
        ui->pushButton->setText(this->mItemName);
        emit fixName(this->id, this->mItemName);
    }
}

