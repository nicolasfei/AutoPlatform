#include "communicationset3_2form.h"
#include "ui_communicationset3_2form.h"

CommunicationSet3_2Form::CommunicationSet3_2Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommunicationSet3_2Form)
{
    ui->setupUi(this);
}

CommunicationSet3_2Form::~CommunicationSet3_2Form()
{
    delete ui;
}
