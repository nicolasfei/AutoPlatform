#include "canopenform.h"
#include "ui_canopenform.h"

CanopenForm::CanopenForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CanopenForm)
{
    ui->setupUi(this);
}

CanopenForm::~CanopenForm()
{
    delete ui;
}
