#include "communicationdataform.h"
#include "ui_communicationdataform.h"

CommunicationDataForm::CommunicationDataForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::CommunicationDataForm)
{
    ui->setupUi(this);
}

CommunicationDataForm::~CommunicationDataForm()
{
    delete ui;
}
