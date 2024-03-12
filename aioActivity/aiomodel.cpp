#include "aiomodel.h"
#include <QSize>
#include <QDebug>

AioModel::AioModel(QObject *parent) : QAbstractTableModel(parent)
{
    this->m_ClassList = new QList<AioDataClass *>();
    this->m_columnCount = 2;
}

AioModel::~AioModel()
{
    //    for(int i=0;i<m_ClassList->size();i++)
    //    {
    //        delete m_ClassList->at(i);
    //    }
    delete this->m_ClassList;
}

void AioModel::setHorizontalHeaderList(QStringList horizontalHeaderList)
{
    horizontal_header_list = horizontalHeaderList;
}

void AioModel::setVerticalHeaderList(QStringList verticalHeaderList)
{
    vertical_header_list = verticalHeaderList;
}

int AioModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(vertical_header_list.size() > 0)
    {
        return vertical_header_list.size();
    }
    if(NULL == m_ClassList)
    {
        return 0;
    }
    else
    {
        int rowNum = 0;
        qDebug() << "--->" << m_ClassList->size() << AioDataClass::itemCount << this->m_columnCount;
        rowNum = m_ClassList->size() / (this->m_columnCount);
        rowNum += (m_ClassList->size() % (this->m_columnCount) > 0 ? 1: 0);
        return rowNum;
    }
}

int AioModel::columnCount(const QModelIndex &parent) const
{
    if(horizontal_header_list.size() > 0)
        return horizontal_header_list.size();

    if(NULL == m_ClassList)
        return 0;
    else if(m_ClassList->size() < 1)
        return 0;
    else
        return this->m_columnCount;
}

QVariant AioModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    int itemIndex = index.row()*(this->m_columnCount) + index.column();
    if(itemIndex>=m_ClassList->size()){
        return QVariant();
    }

    AioDataClass *item = m_ClassList->at(itemIndex);
    if(item==NULL)
    {
        return QVariant();
    }

    switch (role) {
    case NameRole:
        return item->getName();
    case ValueRole:
        return item->getValue();
    case Qt::SizeHintRole:
        return QSize(0, 130);
    default:
        return QVariant();
    }
    return QVariant();
}

bool AioModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    int itemIndex = index.row()*(this->m_columnCount) + index.column();
    if(itemIndex>=m_ClassList->size()){
        return false;
    }

    if (data(index, role) != value) {
        AioDataClass *item = m_ClassList->at(itemIndex);
        if(item==NULL)
        {
            return false;
        }
        switch(role)
        {
        case NameRole:
            item->setName(value.toString());
            break;
        case ValueRole:
            item->setValue(value.toString());
            break;
        default:
            return false;
        }
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags AioModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flag = QAbstractItemModel::flags(index);
    //    flag = Qt::ItemIsEditable;
    return flag;
}

QModelIndex AioModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || column >= columnCount(parent))
        return QModelIndex();

    return createIndex(row,column);
}

QModelIndex AioModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

void AioModel::refrushModel()
{
    beginResetModel();
    endResetModel();

    emit updateCount(this->rowCount(QModelIndex()));
}

QHash<int, QByteArray> AioModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    return  roles;
}

void AioModel::setColumnCount(int count)
{
    this->m_columnCount = count;
}

void AioModel::setModalDatas(QList<AioDataClass*> *rowlist)
{
    if(this->m_ClassList!=NULL)
    {
        this->m_ClassList->clear();
    }
    this->m_ClassList = rowlist;
}

void AioModel::add()
{
    AioDataClass *item=new AioDataClass;
    item->setName(QString::fromLocal8Bit("网Uab电压瞬时值"));
    item->setValue(QString::fromLocal8Bit("0.2356"));
    m_ClassList->push_back(item);
    emit layoutChanged();
}

void AioModel::add(AioDataClass *data)
{
    m_ClassList->push_back(data);
    emit layoutChanged();
}

//AioDataClass* AioModel::at(int index)
//{
//    if(index<m_ClassList->size())
//    {
//        return m_ClassList->at(index);
//    }
//    return NULL;
//}
