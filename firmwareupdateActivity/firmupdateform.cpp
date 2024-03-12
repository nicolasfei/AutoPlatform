#include "firmupdateform.h"
#include "ui_firmupdateform.h"

FirmUpdateForm::FirmUpdateForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::FirmUpdateForm)
{
    ui->setupUi(this);
}

FirmUpdateForm::~FirmUpdateForm()
{
    delete ui;
}
