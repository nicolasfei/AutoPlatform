#include "listchooisedialog.h"
#include "ui_listchooisedialog.h"
#include "commonView/wavefileitemwidget.h"
#include "common/commonclass.h"

ListChooiseDialog::ListChooiseDialog(QList<QString> list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListChooiseDialog)
{
    ui->setupUi(this);
    this->list = list;
    this->selectList = new QList<QString>();

    for(int i=0; i<this->list.size(); i++)
    {
        MY_DEBUG << this->list.at(i);
        WaveFileItemWidget *item = new WaveFileItemWidget(i, this->list.at(i));
        QListWidgetItem* pItem = new QListWidgetItem();
        pItem->setSizeHint(QSize(350, 40));
        ui->listWidget->addItem(pItem);
        ui->listWidget->setItemWidget(pItem, item);
        connect(item, &WaveFileItemWidget::itemSelect, this, &ListChooiseDialog::on_itemSelect);
    }
}

ListChooiseDialog::~ListChooiseDialog()
{
    delete ui;
}

void ListChooiseDialog::on_itemSelect(int id, bool select)
{
    QString item = this->list.at(id);
    if(select){
        if(!this->selectList->contains(item)){
            this->selectList->append(item);
        }
    }else{
        for(int i=0; i<this->selectList->size(); i++){
            if(this->selectList->at(i) == item){
                this->selectList->removeAt(i);
                break;
            }
        }
    }
}

void ListChooiseDialog::on_buttonBox_accepted()
{
    emit this->selectResult(selectList);
}

