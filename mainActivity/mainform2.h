#ifndef MAINFORM2_H
#define MAINFORM2_H

#include <QWidget>
#include <QLineEdit>
#include "commonView/mybaseqwidget.h"
#include "common/parameterclass.h"
#include "communication/businessresponse.h"

namespace Ui {
class MainForm2;
}

class MainForm2 : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit MainForm2(QWidget *parent = nullptr);
    ~MainForm2();

    void initUiView();      //初始化ui view
    void initData();        //初始化数据
    void startBusiness();   //开始业务逻辑
    void endBusiness();     //结束业务逻辑

private:
    Ui::MainForm2 *ui;
    QList<QLineEdit*> m_waterCoolList;  //水冷参数项
    QList<QLineEdit*> m_gridPowerList;  //常用变量项

//    bool isMySetControlMode=false;      //是否自己设置的本地远程控制模式
    bool isFaultReset=false;            //是否处于故障复位状态
private slots:
    /**
     * @brief queryMyParameterTask--数据查询任务
     */
    void myQueryParameterTask();
    /**
     * @brief mainFormQueryResponse--数据查询响应
     * @param success
     * @param m_Data
     * @param errorID
     * @param errorMsg
     */
    void mainForm2QueryResponse(BusinessResponse m_Data);
//    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_pressed();
    void on_pushButton_released();
    void on_alarm2_clicked();
    void on_fault2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_controlUpdate(int itemID);
};

#endif // MAINFORM2_H
