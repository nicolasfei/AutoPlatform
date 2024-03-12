#ifndef OTHERFORM_H
#define OTHERFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class otherForm;
}

class OtherForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit OtherForm(QWidget *parent = nullptr);
    ~OtherForm();
    void initData() override;
    void initUiView() override;
    void startBusiness() override;   //开始业务逻辑
    void endBusiness() override;     //结束业务逻辑
    void release();         //释放资源

private slots:
    void on_resetMaster_clicked();

    void on_setNormalPW_clicked();

    void on_setMasterPW_clicked();

private:
    Ui::otherForm *ui;
};

#endif // OTHERFORM_H
