#ifndef PARAMETERCOMPARISONFORM_H
#define PARAMETERCOMPARISONFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class ParameterComparisonForm;
}

class ParameterComparisonForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit ParameterComparisonForm(QWidget *parent = nullptr);
    ~ParameterComparisonForm();

private:
    Ui::ParameterComparisonForm *ui;
};

#endif // PARAMETERCOMPARISONFORM_H
