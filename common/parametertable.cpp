#include "parametertable.h"
#include "commonclass.h"
#include <QDebug>
#include <QDir>
#include <iostream>
#include <QTextCodec>
#include <QMessageBox>

/**
 * @brief ParameterTable--获取参数文件到内存
 * @param addrOffset---AddrOffset偏移地址值
 * @param wrOffset---WR寄存器ID偏移地址
 * @param rOffset---R寄存器ID偏移地址
 * @param wrFpgaOffset---FPGA（上位机WR）偏移地址
 * @param rFpgaOffset---FPGA（上位机R）偏移地址
 * @param filePath---参数文件路径
 * @param cabinetID---机柜id
 */
ParameterTable::ParameterTable(int addrOffset, int wrOffset, int rOffset,
                               int wrFpgaOffset,int rFpgaOffset,QString filePath, int cabinetID)
{
    int i=0;
    this->isHaveError=false;
    if(filePath.isEmpty())
    {
        qDebug() << "ParameterTable file path is NULL!" << endl;
        this->isHaveError=true;
        return;
    }

    this->tableFilePath = filePath;
    QFile tableFile(filePath);
    if(!tableFile.open(QIODevice::ReadOnly))
    {
        qDebug() << filePath << "is not exists?" <<endl;
        this->isHaveError=true;
        return;
    }
    this->tableMap = new QMap<QString, QList<ParameterClass*>*>();

    QTextStream * out = new QTextStream(&tableFile);//文本流
    out->setCodec(QTextCodec::codecForName("UTF-8"));
    m_list = new QList<ParameterClass*>();
    QList<ParameterClass*>* pList = new QList<ParameterClass*>();
    forever
    {
        QString line = out->readLine().trimmed();
        if(line.isEmpty()){
            break;
        }
        if(i==0)    //这个是表头格式
        {
            this->headerLabels = line.split(",");
            i++;
            continue;
        }
        ParameterClass *pItem = new ParameterClass(addrOffset, wrOffset, rOffset,
                                                   wrFpgaOffset, rFpgaOffset, line);
        //解析错误的参数不加入参数表
        if(pItem->chName.isEmpty()){
            MY_DEBUG << "pItem->chName is empty";
            this->isHaveError=true;
            break;
        }
        pItem->setCabinetID(cabinetID);                     //add by bruce 07-23
        m_list->append(pItem);
        //检测pItem的组ID是否与pList中元素的组ID相同
        if(pList->size()>0 && (pList->at(0)->group.compare(pItem->group)!=0))
        {
            //组ID不同，则说明一个组的数据已经读取完毕，加入到tableMap中
            QString groupName = pList->at(0)->group;
            //先判断map中是否已经包含了相同key的组，如果有这加到尾部
            if(this->tableMap->contains(groupName)){
                this->tableMap->value(groupName)->append(*pList);
            }else{
                //如果没有，这插入最新的
                this->tableMap->insert(pList->at(0)->group, pList);
            }
            //创建新的组来保存pItem
            pList = new QList<ParameterClass*>();
            pList->append(pItem);
        }else{
            pList->append(pItem);
        }
    }
    if(!isHaveError){
        //组ID不同，则说明一个组的数据已经读取完毕，加入到tableMap中
        QString groupName = pList->at(0)->group;
        //先判断map中是否已经包含了相同key的组，如果有这加到尾部
        if(this->tableMap->contains(groupName)){
            this->tableMap->value(groupName)->append(*pList);
        }else{
            //如果没有，这插入最新的
            this->tableMap->insert(pList->at(0)->group, pList);
        }
    }
    tableFile.close();//操作完成后记得关闭文件
}

ParameterTable::~ParameterTable()
{
    if(this->tableMap!=NULL)
    {
        QMap<QString, QList<ParameterClass*>*>::iterator iter = tableMap->begin();
        while (iter != tableMap->end())
        {
            delete iter.value();
            iter++;

        }
        delete this->tableMap;
        this->tableMap = NULL;
    }
}

bool ParameterTable::updateTableMap2File()
{
    QFile tableFile(this->tableFilePath);
    if(!tableFile.open(QIODevice::WriteOnly))
    {
        MY_DEBUG << tableFilePath << "is not exists?" <<endl;
        return false;
    }
    QTextStream out(&tableFile);//文本流
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    QString head;
    //写头名称标识
    for(int i=0;i<this->headerLabels.size();i++)
    {
        head.append( this->headerLabels.at(i));
        if(i!=(this->headerLabels.size()-1))
        {
            head.append(",");
        }
    }
    out<<head<<endl;
    MY_DEBUG << "write head to file " << tableFilePath << "finish";
    //写数据
    for(int i=0; i<this->m_list->size();i++){
        ParameterClass* p = m_list->at(i);
        QString text = p->getText();
        out<<text;
    }
    out.flush();
    MY_DEBUG << "write data to file " << tableFilePath << "finish";
    tableFile.close();//操作完成后记得关闭文件
    return true;
}

/**
 * @brief getHeaderLabels 获取头
 * @return
 */
QStringList ParameterTable::getHeaderLabels()
{
    return this->headerLabels;
}
