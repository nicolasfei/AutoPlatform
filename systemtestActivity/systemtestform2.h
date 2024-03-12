#ifndef SYSTEMTESTFORM2_H
#define SYSTEMTESTFORM2_H

#include <QWidget>
#include <QLineEdit>
#include "commonView/mybaseqwidget.h"
#include "common/parameterclass.h"
#include "communication/businessresponse.h"

namespace Ui {
class SystemTestForm2;
}

class SystemTestForm2 : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit SystemTestForm2(QWidget *parent = nullptr);
    ~SystemTestForm2();

    void initUiView();      //初始化ui view
    void initData();        //初始化数据
    void startBusiness();   //开始业务逻辑
    void endBusiness();     //结束业务逻辑

private:
    Ui::SystemTestForm2 *ui;
    QList<QPushButton*> m_gridPowerList;  //常用变量项

    bool isMyModeSelect=false;
    bool pb90Status=false;
    bool pb91Status=false;
    bool pb92Status=false;
    bool pb93Status=false;
    bool pb94Status=false;

    bool tb10Set=false;
    bool tb11Set=false;
    bool tb12Set=false;
    bool tb13Set=false;
    bool tb14Set=false;
    bool tb15Set=false;

private:
    void writeReg(int reg, float value);
    void writeRegUint(int reg, uint32_t value);
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
    void myQueryResponse(BusinessResponse m_Data);
    void on_pushButton_90_clicked();
    void on_pushButton_91_clicked();
    void on_pushButton_92_clicked();
    void on_pushButton_93_clicked();
    void on_pushButton_94_clicked();
    void on_toolButton_10_clicked();
    void on_toolButton_11_clicked();
    void on_toolButton_12_clicked();
    void on_toolButton_13_clicked();
    void on_toolButton_14_clicked();
    void on_toolButton_15_clicked();
//    void on_lineEdit_51_returnPressed();
//    void on_lineEdit_52_returnPressed();
//    void on_lineEdit_53_returnPressed();
//    void on_lineEdit_54_returnPressed();
//    void on_lineEdit_55_returnPressed();
//    void on_lineEdit_56_returnPressed();
//    void on_lineEdit_57_returnPressed();
//    void on_lineEdit_58_returnPressed();

    void on_pushButton_clicked();
    void on_workModelUpdate(int index);
    void on_pushButton_2_clicked();
    void on_systemModelUpdate(int index);
    void on_pushButton_3_clicked();
    void on_netModelUpdate(int index);
    void on_pushButton_4_clicked();
    void on_machineModelUpdate(int index);
    void on_pushButton_5_clicked();
    void on_simulationModelUpdate(int index);

    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_13_clicked();
};

#endif // SYSTEMTESTFORM2_H
