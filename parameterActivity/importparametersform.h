#ifndef IMPORTPARAMETERSFORM_H
#define IMPORTPARAMETERSFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class ImportParametersForm;
}

class ImportParametersForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit ImportParametersForm(QWidget *parent = nullptr);
    ~ImportParametersForm();

private:
    Ui::ImportParametersForm *ui;
};

#endif // IMPORTPARAMETERSFORM_H
