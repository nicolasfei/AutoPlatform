#include "armcommunicationform.h"
#include "ui_armcommunicationform.h"

ArmCommunicationForm::ArmCommunicationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArmCommunicationForm)
{
    ui->setupUi(this);
}

ArmCommunicationForm::~ArmCommunicationForm()
{
    delete ui;
}
