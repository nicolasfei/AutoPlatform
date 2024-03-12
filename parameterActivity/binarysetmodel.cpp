#include "binarysetmodel.h"

#include "common/commonclass.h"

BinarySetModel::BinarySetModel(BinarySetClass bSet, int aColumnNumWithChechBox, bool itemCanSet, bool itemCanFix, QObject *parent) :  QAbstractTableModel(parent),
    binSet(bSet),colNumberWithCheckBox(aColumnNumWithChechBox),itemCanFix(itemCanFix),itemCanSet(itemCanSet)
{
    //    rowCheckStateMap.clear();
    colNumberWithCheckBox=1;    //第一列显示checkbox
    //    int i=0;
    //    for(bool check:binSet.isSet)
    //    {
    //        rowCheckStateMap.insert(i++, check?Qt::Checked:Qt::Unchecked);
    //    }
}

int BinarySetModel::rowCount(const QModelIndex &parent) const
{
    return this->binSet.validBitCount;
}

int BinarySetModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant BinarySetModel::data(const QModelIndex &index, int role ) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return QString::number(index.row()+1);
        if (index.column() == 2)
            return this->binSet.describe.at(index.row());
    }
    if (role == Qt::CheckStateRole)
    {
        if (index.column() == colNumberWithCheckBox)
        {
            return this->binSet.bitValue.at(index.row())? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

QVariant BinarySetModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    switch(section)
    {
    case 0:
        return QString::fromLocal8Bit("位序");
    case 1:
        return QString::fromLocal8Bit("置位");
    case 2:
        return QString::fromLocal8Bit("描述");
    }
    return QVariant();
}

Qt::ItemFlags BinarySetModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    if (index.column() == colNumberWithCheckBox){
        if(this->itemCanSet){
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }else{
            return Qt::NoItemFlags;//Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }
    }else{
        if(this->itemCanFix){
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }else{
            return  Qt::NoItemFlags;//Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }
}

bool BinarySetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    //设置checkBox状态
    if (role == Qt::CheckStateRole && index.column() == colNumberWithCheckBox){
        MY_DEBUG << "set bit " << index.row() << "--" << value.toUInt();
        this->binSet.bitValue.setBit(index.row(), value==Qt::Checked?true:false);
        this->binSet.isChange=true;
        //        emit BitIsChecked(index.row());
    }
    //设置项文字描述
    if (role == Qt::DisplayRole && index.column() == 2){
        MY_DEBUG << "fix " << index.row() << "--" << value.toString();
        QString des = value.toString();
        if(binSet.describe.at(index.row()) != des){
            binSet.describe.removeAt(index.row());
            binSet.describe.insert(index.row(), value.toString());
            this->binSet.isChange=true;
            data(index, Qt::DisplayRole);
        }
    }
    return true;
}

uint32_t BinarySetModel::getBitValue()
{
    return this->binSet.getBitValue();
}

QStringList BinarySetModel::getDescribe()
{
    return this->binSet.getDescribe();
}
