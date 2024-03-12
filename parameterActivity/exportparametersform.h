#ifndef EXPORTPARAMETERSFORM_H
#define EXPORTPARAMETERSFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class ExportParametersForm;
}

class ExportParametersForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit ExportParametersForm(QWidget *parent = nullptr);
    ~ExportParametersForm();

private:
    Ui::ExportParametersForm *ui;
};

#endif // EXPORTPARAMETERSFORM_H
