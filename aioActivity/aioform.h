#ifndef AIOFORM_H
#define AIOFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"
#include "aiomodel.h"
#include "aioitemdelegate.h"

namespace Ui {
class AIOForm;
}

class AIOForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit AIOForm(QWidget *parent = nullptr);
    ~AIOForm();

    void initUiView();      //初始化ui view
    void initData();        //初始化数据
    void startBusiness();   //开始业务逻辑
    void endBusiness();     //结束业务逻辑
private:
    Ui::AIOForm *ui;

    AioModel *netModel;
    AioItemDelegate *netDelegate;

    AioModel *engineModel;
    AioItemDelegate *engineDelegate;

    AioModel *armModel;
    AioItemDelegate *armDelegate;

};

#endif // AIOFORM_H
