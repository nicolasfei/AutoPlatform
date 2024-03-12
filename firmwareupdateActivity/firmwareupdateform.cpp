#include "firmwareupdateform.h"
#include "ui_firmwareupdateform.h"

FirmwareUpdateForm::FirmwareUpdateForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::FirmwareUpdateForm)
{
    ui->setupUi(this);
}

FirmwareUpdateForm::~FirmwareUpdateForm()
{
    delete ui;
}
