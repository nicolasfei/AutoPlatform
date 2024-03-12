#ifndef MYBASEQWIDGET_H
#define MYBASEQWIDGET_H

#include <QWidget>
#include "commonView/mywaitdialog.h"
#include "common/parameterclass.h"

/**
 * @brief The MyBaseQWidget class--所有form页面的基类，提供统一的ui/data初始化，业务逻辑加载/取消
 */
class MyBaseQWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyBaseQWidget(QWidget *parent = nullptr);
    ~MyBaseQWidget();

    virtual void initUiView();      //初始化ui view
    virtual void initData();        //初始化数据
    virtual void startBusiness();   //开始业务逻辑
    virtual void endBusiness();     //结束业务逻辑
//    virtual void release();         //释放资源

protected:
    int activityID;             //页面ID--由子类赋值
    bool m_businessRunning;     //业务是否正在进行中
    QList<ParameterClass*> *m_parameterList;       //所需的参数
    MyWaitDialog *waitDialog;    //进度对话框

signals:
    void activitySwitchFinish(int activityID);     //机柜切换页面加载完成

private slots:
    void on_cabinetIdSwitch();                     //机柜被切换了
};

#endif // MYBASEQWIDGET_H
