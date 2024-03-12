#include "cnavmodel.h"
#include "common/commonclass.h"
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QString>

CNavModel::CNavModel(QObject *parent)
    : QAbstractListModel(parent)
{
    initDefaultConfig();
}

CNavModel::~CNavModel()
{
    QVector<TreeNode*>  m_vecTreeNode;
    QVector<ListNode>   m_vecListNode;
    for(QVector<TreeNode*>::iterator it = m_vecTreeNode.begin(); it != m_vecTreeNode.end(); it++)
    {
        if((*it)->listChildren.size())
        {
            for (QList<TreeNode*>::iterator itChild = (*it)->listChildren.begin(); itChild != (*it)->listChildren.end(); it++)
                delete (*itChild);
        }
        delete (*it);
    }

    m_vecListNode.clear();
    m_vecTreeNode.clear();
}

int CNavModel::rowCount(const QModelIndex &parent) const	//返回行数
{
    return m_vecListNode.size();
}

QVariant CNavModel::data(const QModelIndex &index, int role) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= m_vecListNode.size() || index.row() < 0 )
        return QVariant();

    if ( role == Qt::DisplayRole )      //显示文字
        return m_vecListNode[index.row()].qsLabelName;
    else if ( role == Qt::UserRole )    //用户定义起始位置
        return QVariant::fromValue((void*)m_vecListNode[index.row()].pTreeNode);

    return QVariant();
}

void CNavModel::initDefaultConfig()
{
    const QByteArray navTitle[] = {"主界面","系统调试","数字量输入输出"/*,"模拟量输入输出"*/,"动态示波器","水冷系统","参数监控与管理",
                                   "波形分析"/*,"通信数据"*/,"通信设置","固件升级","故障与日志","其他设置"};
    const QByteArray navBG[] = {":/images/images/1-1.png",
                                ":/images/images/2-1.png",
                                ":/images/images/3-1.png",
//                                ":/images/images/4-1.png",
                                ":/images/images/5-1.png",
                                ":/images/images/6-1.png",
                                ":/images/images/7-1.png",
                                ":/images/images/8-1.png",
//                                ":/images/images/9-1.png",
                                ":/images/images/10-1.png",
                                ":/images/images/11-1.png",
                                ":/images/images/12-1.png",
                                ":/images/images/13-1.png"
                               };
    const QByteArray navSelectBG[] = {":/images/images/1-2.png",
                                ":/images/images/2-2.png",
                                ":/images/images/3-2.png",
//                                ":/images/images/4-2.png",
                                ":/images/images/5-2.png",
                                ":/images/images/6-2.png",
                                ":/images/images/7-2.png",
                                ":/images/images/8-2.png",
//                                ":/images/images/9-2.png",
                                ":/images/images/10-2.png",
                                ":/images/images/11-2.png",
                                ":/images/images/12-2.png",
                                ":/images/images/13-2.png"
                                     };
    for(int i=0;i<11;i++){
        TreeNode* pTreeNode = new TreeNode;
        pTreeNode->qsLableName = QString::fromLocal8Bit(navTitle[i]);//获取一级节点的lable
        pTreeNode->bg = navBG[i];
        pTreeNode->selectBg = navSelectBG[i];
        pTreeNode->nLevel = 1;  //标志一级节点
        pTreeNode->collapse =  false; //标志是否展开
        pTreeNode->nIndex = i;  //获取标志
//        if(i==5){
//            const QByteArray navSecTitle[] = {"参数列表","导入下载参数","导出下载参数","参数对比"};
//            for(int j=0;j<3;j++){
//                TreeNode* pSecNode = new TreeNode;
//                pSecNode->qsLableName = QString::fromLocal8Bit(navSecTitle[j]);
//                pSecNode->nLevel = 2;
//                pSecNode->nIndex = j;
//                pTreeNode->collapse = true;
//                pTreeNode->listChildren.push_back(pSecNode);
//            }
//        }
        m_vecTreeNode.push_back(pTreeNode);
    }
    RefreshList();  //刷新界面标题栏展示数据
    beginInsertRows(QModelIndex(), 0, m_vecListNode.size());    //插入所有节点
    endInsertRows(); //结束插入
}

void CNavModel::ReadConfig(QString qsPath)
{
    qDebug()<< qsPath;
    QFile xmlFile(qsPath);
    if(!xmlFile.open(QFile::ReadOnly | QFile::Text))
        return;

    QDomDocument docXml;
    QString error;
    if(!docXml.setContent(&xmlFile, false, &error))
    {
        xmlFile.close();
        return;
    }

    QDomElement xmlRoot = docXml.documentElement(); //返回根节点
    QDomNode domNode = xmlRoot.firstChild(); //获取子节点，一级节点
    while (!domNode.isNull())
    {
        if(domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();   //一级节点
            TreeNode* pTreeNode = new TreeNode;

            pTreeNode->qsLableName = domElement.attribute("lable");//获取一级节点的lable
            pTreeNode->nLevel = 1;  //标志一级节点
            pTreeNode->collapse =  domElement.attribute("collapse").toInt(); //标志是否展开
            pTreeNode->nIndex = domElement.attribute("index").toInt();  //获取标志

            QDomNodeList list = domElement.childNodes();    //获取二级节点
            for(int i = 0; i < list.count(); i++)
            {
                QDomElement secNodeInfo = list.at(i).toElement();
                TreeNode* pSecNode = new TreeNode;
                pSecNode->qsLableName = secNodeInfo.attribute("lable");
                pSecNode->nLevel = 2;
                pSecNode->nIndex = secNodeInfo.attribute("index").toInt();
                pTreeNode->collapse = false;
                pTreeNode->listChildren.push_back(pSecNode);
            }
            m_vecTreeNode.push_back(pTreeNode);
        }
        domNode = domNode.nextSibling();    //下一一级节点
    }

    xmlFile.close();
    RefreshList();  //刷新界面标题栏展示数据
    beginInsertRows(QModelIndex(), 0, m_vecListNode.size());    //插入所有节点
    endInsertRows(); //结束插入
}

void CNavModel::RefreshList()
{
    m_vecListNode.clear();
    for(QVector<TreeNode*>::iterator it = m_vecTreeNode.begin(); it != m_vecTreeNode.end(); it++)
    {
        //一级节点
        ListNode node;
        node.qsLabelName = (*it)->qsLableName;
        node.pTreeNode = *it;
        m_vecListNode.push_back(node);

        if(!(*it)->collapse) //如果一级节点未展开，则插入下一一级节点
            continue;

        for(QList<TreeNode*>::iterator child = (*it)->listChildren.begin(); child != (*it)->listChildren.end(); child++)
        {
            ListNode node;
            node.qsLabelName = (*child)->qsLableName;
            node.pTreeNode = *child;
            m_vecListNode.push_back(node);
        }
    }
}

void CNavModel::Collapse(const QModelIndex& index)
{
    MY_DEBUG << "index.row-->" << index.row();
    TreeNode* pTreeNode = m_vecListNode[index.row()].pTreeNode; //获取当前点击节点
    if(pTreeNode->listChildren.size() == 0) //如果该节点没有子节点 则返回
        return;

    pTreeNode->collapse = !pTreeNode->collapse; //刷新是否展开标志

    if(!pTreeNode->collapse)    //如果是不展开，即为展开变成合并，移除合并的
    {
        beginRemoveRows(QModelIndex(), index.row() + 1, pTreeNode->listChildren.size()); //默认起始节点为最初节点
        endRemoveRows();
    }else {
        beginInsertRows(QModelIndex(), index.row() + 1, pTreeNode->listChildren.size());
        endInsertRows();
    }
    RefreshList(); //更新界面显示节点数据
}

void CNavModel::Refresh()
{
    RefreshList();
    beginResetModel();
    endResetModel();
}
