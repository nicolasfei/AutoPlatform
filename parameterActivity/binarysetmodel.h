#ifndef BINARYSETMODEL_H
#define BINARYSETMODEL_H

#include <QAbstractTableModel>
#include "BinarySetClass.h"

class BinarySetModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BinarySetModel(BinarySetClass bSet, int aColumnNumWithChechBox, bool itemCanSet, bool itemCanFix, QObject *parent = nullptr);

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    uint32_t getBitValue();
    QStringList getDescribe();
    bool isDataUpdate(){
        return binSet.isChange;
    }

signals:
    void BitIsChecked(const int bit);

private:
    BinarySetClass binSet;
    int colNumberWithCheckBox;
    bool itemCanFix;        //用户是否可以修改数据位的描述
    bool itemCanSet;        //用户是否可以修改数据位的值
};

#endif // BINARYSETMODEL_H
