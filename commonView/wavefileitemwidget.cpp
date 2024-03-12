#include "wavefileitemwidget.h"
#include "ui_wavefileitemwidget.h"

WaveFileItemWidget::WaveFileItemWidget(int id, QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaveFileItemWidget)
{
    ui->setupUi(this);
    this->id = id;
    this->name = name;
    ui->label->setText(this->name);
}

WaveFileItemWidget::~WaveFileItemWidget()
{
    delete ui;
}

void WaveFileItemWidget::on_checkBox_stateChanged(int arg1)
{
    emit this->itemSelect(id, arg1);
}

