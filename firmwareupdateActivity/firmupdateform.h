#ifndef FIRMUPDATEFORM_H
#define FIRMUPDATEFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class FirmUpdateForm;
}

class FirmUpdateForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit FirmUpdateForm(QWidget *parent = nullptr);
    ~FirmUpdateForm();

private:
    Ui::FirmUpdateForm *ui;
};

#endif // FIRMUPDATEFORM_H
