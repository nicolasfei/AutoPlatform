#ifndef COMMUNICATIONSET10FORM_H
#define COMMUNICATIONSET10FORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"
#include "communication/businessresponse.h"

namespace Ui {
class CommunicationSet10Form;
}

class CommunicationSet10Form : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit CommunicationSet10Form(QWidget *parent = nullptr);
    ~CommunicationSet10Form();

    void initUiView();      //初始化ui view
    void initData();        //初始化数据
    void startBusiness();   //开始业务逻辑
    void endBusiness();     //结束业务逻辑
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_queryDataResponse(BusinessResponse data);

    void on_ftpRootDir_clicked();

private:
    Ui::CommunicationSet10Form *ui;

private:
    bool saveLowConfig();
    void resetMasterDevice();
};

#endif // COMMUNICATIONSET10FORM_H
