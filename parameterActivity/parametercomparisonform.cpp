#include "parametercomparisonform.h"
#include "ui_parametercomparisonform.h"

ParameterComparisonForm::ParameterComparisonForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::ParameterComparisonForm)
{
    ui->setupUi(this);
}

ParameterComparisonForm::~ParameterComparisonForm()
{
    delete ui;
}
