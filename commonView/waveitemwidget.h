#ifndef WAVEITEMWIDGET_H
#define WAVEITEMWIDGET_H

#include <QWidget>
#include "common/parameterclass.h"

namespace Ui {
class WaveItemWidget;
}

class WaveItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveItemWidget(QWidget *parent = nullptr);
    ~WaveItemWidget();

private:
    Ui::WaveItemWidget *ui;

private:
    bool mIsShow = true;   //默认显示
    bool mIsZoom = true;   //默认放大
    bool mIsFixName = false;  //默认不能修改通道名字
    int mItemID = 0;        //通道编号
    uint32_t id;             //通道id--唯一标识
    QString mItemName = "";

private slots:
    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_pushButton_clicked();

signals:
    void deleteItem(uint32_t id);            //删除item按钮
    void isShowWave(uint32_t id, bool show); //是否显示--true/显示 false/隐藏
    void zoomWave(uint32_t id);              //设置缩放信号
    void fixName(uint32_t id, QString newName);    //通道名字被修改
public:
    void setData(int itemId, uint32_t id, const QString& itemName, bool isShow = true, bool isFix=false, bool isZoom=false);  //设置数据
    void setShow(bool show);
    QString getName(){
        return this->mItemName;
    }
    uint32_t getId(){
        return this->id;
    }
};

#endif // WAVEITEMWIDGET_H
