#include "dioitemdelegate.h"
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

DioItemDelegate::DioItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

DioItemDelegate::~DioItemDelegate()
{

}

void DioItemDelegate::setDioModel(DioModel* model)
{
    this->m_dioListViewModel = model;
}

static QRect CheckBoxRect(const QStyleOptionViewItem &viewItemStyleOptions)/*const*/
{
    //绘制按钮所需要的参数
    QStyleOptionButton checkBoxStyleOption;
    //按照给定的风格参数 返回元素子区域
    QRect checkBoxRect = QApplication::style()->subElementRect( QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
    //返回QCheckBox坐标
    QPoint checkBoxPoint(viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width() / 2 - checkBoxRect.width() / 2,
                         viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - checkBoxRect.height() / 2);
    //返回QCheckBox几何形状
    return QRect(checkBoxPoint, checkBoxRect.size());
}

void DioItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid()){
        return;
    }
    int qrealW = 8;
    int interval = 8;
    DioDataClass *dio=m_dioListViewModel->at(index.row());
    if(dio!=NULL)
    {
        //显示指示灯--开/关
        QPixmap lead;
        if(dio->getStatus())
        {
            lead.load(":/images/images/green.svg");
        }else{
            lead.load(":/images/images/gary.svg");
        }
        painter->drawPixmap(QPointF(option.rect.x()+interval, option.rect.y() + option.rect.height() / 2 - lead.height()/2),lead);

        //name
        painter->setPen(QPen(Qt::white,qrealW,Qt::SolidLine));
        painter->drawText(QPoint(option.rect.x() + lead.width()*2, option.rect.y() + option.rect.height() / 2 + qrealW/2), dio->getName());

        //check box
        if(dio->getForceDio())
        {
            bool checked = dio->getCheckDio();
            QStyleOptionButton checkBoxStyleOption;
            checkBoxStyleOption.state |= QStyle::State_Enabled;
            checkBoxStyleOption.state |= checked? QStyle::State_On : QStyle::State_Off;
            checkBoxStyleOption.rect =  CheckBoxRect(option);  //QCheckBox几何形状
            QApplication::style()->drawControl(QStyle::CE_CheckBox,&checkBoxStyleOption,painter);

            //On/Off
            QPoint onOffPoint(option.rect.x() + option.rect.width() / 2 + checkBoxStyleOption.rect.width(),
                                 option.rect.y() + option.rect.height() / 2 + qrealW/2);
            painter->drawText(onOffPoint, QString::fromLocal8Bit("On/Off"));
        }
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

bool DioItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if((event->type() == QEvent::MouseButtonRelease) ||(event->type() == QEvent::MouseButtonDblClick))
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() != Qt::LeftButton ||
                !CheckBoxRect(option).contains(mouseEvent->pos())){
            return true;
        }
        if(event->type() == QEvent::MouseButtonDblClick){
            return true;
        }
    }
    else if(event->type() == QEvent::KeyPress)
    {
        if(static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    bool checked = index.model()->data(index, DioModel::CheckRole).toBool();
    return model->setData(index, !checked, DioModel::CheckRole);

//    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
