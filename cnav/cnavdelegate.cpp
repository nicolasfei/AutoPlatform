#include "cnavdelegate.h"
#include "cnavmodel.h"

#include <QPainter>
#include <QColor>
#include <QDebug>

CNavDelegate::CNavDelegate(QObject *parent) : QStyledItemDelegate(parent)
  , m_pending(false)
{

}

CNavDelegate::~CNavDelegate()
{

}

QSize CNavDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    CNavModel::TreeNode* pTreeNode = (CNavModel::TreeNode*)index.data(Qt::UserRole).value<void*>();
    if(pTreeNode->nLevel == 1)
        return QSize(50, 25);
    else
        return QSize(50, 15);
//    Q_UNUSED(option);
//    Q_UNUSED(index);
//    return QSize(50, 25);
}

void CNavDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //    QStyledItemDelegate::paint(painter, option, index);

    QString se = (option.state & QStyle::State_Selected)?"true":"false";
//    qDebug() << "option.state & QStyle::State_Selected-->" << se << " index-->" << index.row();
    //    QPaintDevice* original_pdev_ptr = painter->device();
    //    painter->end()

    //    painter->begin(original_pdev_ptr);

    //    painter->save();
    CNavModel::TreeNode* pTreeNode = (CNavModel::TreeNode*)index.data(Qt::UserRole).value<void*>();
    painter->setRenderHint(QPainter::Antialiasing); //防走样

    //根据绘制时提供的信息进行背景色绘制
    if ( option.state & QStyle::State_Selected )
    {
        painter->fillRect(option.rect, QColor(28, 52, 98));
    }
    else if ( option.state & QStyle::State_MouseOver )
    {
        painter->fillRect(option.rect, QColor(28, 52, 98));
    }
    else
    {
        painter->fillRect(option.rect, QColor(0,0,0,0));
    }

    //设置图片大小
    QPixmap img(pTreeNode->bg);
    QRect targetRect = option.rect;
    targetRect.setWidth(16);
    targetRect.setHeight(16);

    //设置图片坐标
    QPoint c = option.rect.center();
    c.setX(28);
    targetRect.moveCenter(c);

    //将图片放到对应位置
    if ( option.state & QStyle::State_Selected ){
        painter->drawPixmap(targetRect, pTreeNode->bg, img.rect());
    }else{
        painter->drawPixmap(targetRect, pTreeNode->selectBg, img.rect());
    }

    //添加文字
    QPen textPen( option.state & QStyle::State_Selected ? QColor(217, 102, 39) : QColor(255, 255, 255));
    painter->setPen(textPen);

    int margin = 20;

    if ( pTreeNode->nLevel == 2 )
        margin = 40;

    QRect rect = option.rect;
    rect.setWidth(rect.width() - margin);
    rect.setX(28 + margin);

    QFont normalFont("Microsoft Yahei", 8, QFont::Bold);
    painter->setFont(normalFont);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

    //    painter->restore();
}

QString CNavDelegate::GetImagePath(int nIndex) const
{
    return QString();
}
