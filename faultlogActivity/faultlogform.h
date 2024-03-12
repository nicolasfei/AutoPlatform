#ifndef FAULTLOGFORM_H
#define FAULTLOGFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class FaultLogForm;
}

class FaultLogForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit FaultLogForm(QWidget *parent = nullptr);
    ~FaultLogForm();

private:
    Ui::FaultLogForm *ui;
};

#endif // FAULTLOGFORM_H
