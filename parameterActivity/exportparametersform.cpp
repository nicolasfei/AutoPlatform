#include "exportparametersform.h"
#include "ui_exportparametersform.h"

ExportParametersForm::ExportParametersForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::ExportParametersForm)
{
    ui->setupUi(this);
}

ExportParametersForm::~ExportParametersForm()
{
    delete ui;
}
