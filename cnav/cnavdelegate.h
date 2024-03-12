#ifndef CNAVDELEGATE_H
#define CNAVDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

class CNavDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CNavDelegate(QObject *parent = nullptr);
    ~CNavDelegate();

public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void SetPending(bool pending) { m_pending = pending; }

private:
    QString GetImagePath(int nIndex) const;

private:
    bool m_pending;
};

#endif // CNAVDELEGATE_H
