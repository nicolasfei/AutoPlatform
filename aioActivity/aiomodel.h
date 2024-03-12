#ifndef AIOMODEL_H
#define AIOMODEL_H

#include <QAbstractTableModel>

#include "AioDataClass.h"

class AioModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum datasourceRoles {
        NameRole = Qt::UserRole ,
        ValueRole,
    };
    explicit AioModel(QObject *parent = nullptr);
    ~AioModel();

    void setHorizontalHeaderList(QStringList horizontalHeaderList);
    void setVerticalHeaderList(QStringList verticalHeaderList);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value,int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    QHash<int, QByteArray> roleNames() const override;
    void add();
    void add(AioDataClass *data);
    void setModalDatas(QList<AioDataClass*> *rowlist);
//    AioDataClass *at(int index);
    void setColumnCount(int count);
    void refrushModel();
private:
    QStringList horizontal_header_list;
    QStringList vertical_header_list;
    QList<AioDataClass*>* m_ClassList;
    int m_columnCount;
signals:
    void updateCount(int count);
public slots:
};

#endif // AIOMODEL_H
