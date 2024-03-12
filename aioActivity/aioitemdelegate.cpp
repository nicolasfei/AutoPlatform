#include "aioitemdelegate.h"
#include <QPainter>
#include <QDebug>
#include <QFont>

AioItemDelegate::AioItemDelegate(QObject *parent) : QItemDelegate(parent)
{

}

AioItemDelegate::~AioItemDelegate()
{

}

void AioItemDelegate::setAioModel(AioModel* model)
{
    this->m_ListViewModel = model;
}

void AioItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid() || !(index.model()->data(index,AioModel::ValueRole).isValid())){
        return;
    }

    int row = index.row();
    int x = option.rect.x();
    int y = option.rect.y();
    int width = option.rect.width();
    int height = option.rect.height();
    qDebug() << "row" << row << "x:" << x << "y" << y << "width" << width << "height" << height;
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(Qt::gray));
    QStyleOptionViewItem myOption = option;
    int wordPixelSize = 14;     //字像素
    int margin = 10;            //name和value间距
    int nameStartX=0;
    int wordWidth = width*3/5;
//    int valueWidth = width - wordWidth;

    //name
    QString name = index.model()->data(index,AioModel::NameRole).toString();
    QFont m_qFont;
    m_qFont.setBold(true);
    m_qFont.setPixelSize(wordPixelSize);
    m_qFont.setFamily("Microsoft JhengHei");
    painter->setFont(m_qFont);  //给画家设置字体、大小
    painter->setPen(QPen(Qt::white,wordPixelSize,Qt::SolidLine));
    QFontMetrics fontMetrics(m_qFont);
    int nameLength = fontMetrics.width(name);       //字符串宽度
    if(nameLength > wordWidth)
    {
        name = name.mid(0, wordWidth/(nameLength/name.length())-3).append("...");    //name太长，截取一部分
        nameStartX = nameLength/name.length();
    }
    else
    {
         nameStartX = wordWidth - nameLength;
    }
    painter->drawText(QPoint(x + nameStartX, y + height / 2 + wordPixelSize/2), name);
    //value
    QString value = index.model()->data(index,AioModel::ValueRole).toString();
    QRectF rect;
    rect.setTopLeft(QPointF(x + wordWidth + margin,y+5));
    rect.setBottomRight(QPointF(x + width-5,y + height-5));
    painter->setPen(pen);
    painter->drawRect(rect);
    //        QTextOption textOption;
    //        textOption.setAlignment(Qt::AlignCenter);
    painter->drawText(QPoint(x + wordWidth + margin +5 , y + height / 2 + wordPixelSize/2), value);
    //    QStyledItemDelegate::paint(painter, option, index);
}
