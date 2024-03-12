#ifndef FIRMWAREUPDATEFORM_H
#define FIRMWAREUPDATEFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class FirmwareUpdateForm;
}

class FirmwareUpdateForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit FirmwareUpdateForm(QWidget *parent = nullptr);
    ~FirmwareUpdateForm();

private:
    Ui::FirmwareUpdateForm *ui;
};

#endif // FIRMWAREUPDATEFORM_H
