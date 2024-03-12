#ifndef DIOFORM_H
#define DIOFORM_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMutex>

#include "commonView/mybaseqwidget.h"
#include "communication/businessresponse.h"

namespace Ui {
class DioForm;
}

class DioForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit DioForm(QWidget *parent = nullptr);
    ~DioForm();

    void initUiView();      //初始化ui view
    void initData();        //初始化数据
    void startBusiness();   //开始业务逻辑
    void endBusiness();     //结束业务逻辑

protected slots:
    void on_pushButton_clicked();
    void on_TableDoubleClicked(const QModelIndex &index);
    void on_parameterFixed();
    void on_parameterQueryResponse(BusinessResponse data);
    void on_timerTaskTimeout();

private:
    Ui::DioForm *ui;

    //table model
    QStandardItemModel *m_model;
    //当前双击的table item index
    QModelIndex currIndex;
    //锁
    QMutex m_mutex;
    //参数修改标志
    bool isHaveParametFix=false;

private:
    //更新参数保存按钮状态
    void updateParametSaveStatus();
    //刷新页面
    void showParameTable();
};

#endif // DIOFORM_H
