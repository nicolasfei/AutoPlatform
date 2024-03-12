#include "importparametersform.h"
#include "ui_importparametersform.h"

ImportParametersForm::ImportParametersForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::ImportParametersForm)
{
    ui->setupUi(this);
}

ImportParametersForm::~ImportParametersForm()
{
    delete ui;
}
