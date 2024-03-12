#ifndef DIOITEMDELEGATE_H
#define DIOITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "diomodel.h"

class DioItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DioItemDelegate(QObject *parent = nullptr);
    ~DioItemDelegate();

    void setDioModel(DioModel* model);
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event,QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index);
private:
    DioModel *m_dioListViewModel;

signals:

public slots:
};

#endif // DIOITEMDELEGATE_H
