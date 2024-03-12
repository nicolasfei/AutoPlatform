#include "metercoolform.h"
#include "ui_metercoolform.h"

MeterCoolForm::MeterCoolForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeterCoolForm)
{
    ui->setupUi(this);
}

MeterCoolForm::~MeterCoolForm()
{
    delete ui;
}
