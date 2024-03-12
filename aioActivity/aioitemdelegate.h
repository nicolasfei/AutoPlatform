#ifndef AIOITEMDELEGATE_H
#define AIOITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>
#include "aiomodel.h"

class AioItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit AioItemDelegate(QObject *parent = nullptr);
    ~AioItemDelegate();

    void setAioModel(AioModel* model);
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
//    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    AioModel *m_ListViewModel;

signals:

public slots:

};

#endif // AIOITEMDELEGATE_H
