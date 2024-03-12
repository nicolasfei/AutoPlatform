#ifndef DIOMODEL_H
#define DIOMODEL_H

#include <QAbstractListModel>
#include "DioDataClass.h"

class DioModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum datasourceRoles {
        StatusRole = Qt::UserRole ,
        NameRole,
        ForceRole,
        CheckRole,
        HeaderRole
    };

    explicit DioModel(QObject *parent = nullptr);
    ~DioModel();

    int rowCount(const QModelIndex &parent) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value,int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QHash<int, QByteArray> roleNames() const override;

    void add();

    void add(DioDataClass *student);

    DioDataClass *at(int index);
private:
    QList<DioDataClass*> m_DioClassList;
    QString headTitle;
    int m_Number;

signals:

public slots:
};

#endif // DIOMODEL_H
