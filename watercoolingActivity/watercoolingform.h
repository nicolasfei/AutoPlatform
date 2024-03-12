#ifndef WATERCOOLINGFORM_H
#define WATERCOOLINGFORM_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMutex>

#include "commonView/mybaseqwidget.h"
#include "communication/businessresponse.h"

namespace Ui {
class WaterCoolingForm;
}

class WaterCoolingForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit WaterCoolingForm(QWidget *parent = nullptr);
    ~WaterCoolingForm();

    void initData();
    void initUiView();
    void startBusiness();
    void endBusiness();
//    void initData() override;
//    void initUiView() override;
//    void startBusiness() override;   //开始业务逻辑
//    void endBusiness() override;     //结束业务逻辑
//    void release();         //释放资源

protected slots:
    void on_pushButton_clicked();
    void on_TableDoubleClicked(const QModelIndex &index);
    void on_parameterFixed();
    void on_parameterQueryResponse(BusinessResponse data);
    void on_timerTaskTimeout();

private:
    Ui::WaterCoolingForm *ui;

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

#endif // WATERCOOLINGFORM_H
