#include "faultlogform.h"
#include "ui_faultlogform.h"

FaultLogForm::FaultLogForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::FaultLogForm)
{
    ui->setupUi(this);
}

FaultLogForm::~FaultLogForm()
{
    delete ui;
}
