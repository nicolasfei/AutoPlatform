#include "diomodel.h"
#include <QSize>
#include <QDebug>

DioModel::DioModel(QObject *parent) : QAbstractListModel(parent)
{

}

DioModel::~DioModel()
{
    for(int i=0;i<m_DioClassList.size();i++)
    {
        delete m_DioClassList.at(i);
    }
}

int DioModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_DioClassList.size();
}

int DioModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DioModel::data(const QModelIndex &index, int role) const
{
    DioDataClass *dioData = m_DioClassList.at(index.row());
    if(dioData==NULL)
    {
        return QVariant();
    }
    else
    {
        switch (role) {
        case StatusRole:
            return dioData->getStatus();
        case NameRole:
            return dioData->getName();
        case ForceRole:
            return dioData->getForceDio();
        case Qt::SizeHintRole:
            return QSize(0, 30);
        case CheckRole:
            return dioData->getCheckDio();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool DioModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "DioModel setData role:" << role << " value:" << value;
    DioDataClass *dioData = m_DioClassList.at(index.row());
    if (data(index, role) != value) {
        switch(role)
        {
        case StatusRole:
            dioData->setStatus(value.toBool());
            break;
        case NameRole:
            dioData->setName(value.toString());
            break;
        case ForceRole:
            dioData->setForceDio(value.toBool());
            break;
        case CheckRole:
            dioData->setCheckDio(value.toBool());
            break;
        }
        qDebug() << "DioModel send dataChanged ";
        emit dataChanged(index, index, QVector<int>() << role);
        //        emit dataChanged(createIndex(0,0), createIndex(0,0));
        return true;
    }
    return false;
}

QVariant DioModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            if(section == 0)
            {
                return QString("全选");
            }else if(section == 1)
            {
                return QString("时间");
            }else if(section == 2)
            {
                return QString("地址");
            }
            return "";
            break;
//        case Qt::FontRole:
//            return QFont("SimSun", 12);
//            break;
//        case Qt::TextAlignmentRole:
//            return Qt::AlignCenter;
//            break;
//        case Qt::TextColorRole:
//            return QColor(Qt::black);
//            break;
//        case Qt::SizeHintRole:
//            return QSize(100,40);
//            break;
//        case Qt::BackgroundRole:
//            return QBrush(Qt::black);
//            break;
        default:
            break;
        }
        return QVariant();
}

bool DioModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if(role == HeaderRole)
    {
        this->headTitle = value.toString();
        return true;
    }
    return false;
}

Qt::ItemFlags DioModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> DioModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[StatusRole] = "status";
    roles[NameRole] = "name";
    roles[ForceRole] = "force";
    roles[CheckRole] = "check";
    return  roles;
}

void DioModel::add()
{
    DioDataClass *student=new DioDataClass;
    student->setForceDio(m_Number%2==0?true:false);
    student->setName(QString::fromLocal8Bit("数字io测试项"));
    student->setStatus(m_Number%2==0?true:false);
    m_DioClassList.push_back(student);
    emit layoutChanged();
    m_Number++;
}

void DioModel::add(DioDataClass *data)
{
    m_DioClassList.push_back(data);
    emit layoutChanged();
    m_Number++;
}

DioDataClass* DioModel::at(int index)
{
    if(index<m_DioClassList.size())
    {
        return m_DioClassList.at(index);
    }
    return NULL;
}
