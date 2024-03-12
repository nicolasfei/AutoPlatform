//#include "parametermanagement.h"
//#include <QDir>
//#include <iostream>

//ParameterManagement::ParameterManagement(QObject *parent) : QObject(parent)
//{
//    //    this->dirFileMap = new QMap<QString, QList<QString>*>();
//    //    this->tables = new QList<ParameterTable*>();
//    this->fileConfig = new QMap<QString, ParameterTable*>();
//    this->oscilloscopeList = new QList<ParameterClass *>();
//    this->faultRecordingList = new QList<ParameterClass *>();
//    this->mainFormParameterList = new QList<ParameterClass *>();
//    this->initParameterFile(QDir::currentPath()+"/config");
//}

//ParameterManagement::~ParameterManagement()
//{
//    QMap<QString, ParameterTable*>::Iterator it = this->fileConfig->begin();
//    while(it!=this->fileConfig->end())
//    {
//        it.value()->~ParameterTable();
//        delete it.value();
//    }
//    delete this->fileConfig;

//    for(int i=0;i<this->oscilloscopeList->count();i++)
//    {
//        delete this->oscilloscopeList->at(i);
//    }
//    delete this->oscilloscopeList;

//    for(int i=0;i<this->faultRecordingList->count();i++)
//    {
//        delete this->faultRecordingList->at(i);
//    }
//    delete this->faultRecordingList;
//}

/////**
//// * @brief ParameterManagement::getFileTables--获取文件table
//// * @param level1 一级目录
//// * @param level2 二级目录
//// * @return table
//// */
////ParameterTable* ParameterManagement::getFileTables(QString level1, QString level2)
////{
////    for(int i=0;i<this->tables->count();i++)
////    {
////        ParameterTable* table = this->tables->at(i);
////        if(table->level1Name.compare(level1)==0 && table->level2Name.compare(level2) ==0 ){
////            return table;
////        }
////    }
////    return NULL;
////}

///**
// * @brief ParameterManagement::初始化  读取配置文件，并保存为ParameterClass 放在内存中
// * @param fileDirPath 配置文件所在目录
// */
//void ParameterManagement::initParameterFile(QString fileDirPath)
//{
//    qDebug() << fileDirPath << endl;

//    QDir dir(fileDirPath);
//    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);  //QDir::NoDotAndDotDot 不列出.文件，即指向当前目录的软链接
//    dir.setSorting(QDir::Size | QDir::Reversed);

//    QFileInfoList list = dir.entryInfoList();
//    for (int i = 0; i < list.size(); ++i)
//    {
//        QFileInfo fileInfo = list.at(i);
//        if(fileInfo.isFile())
//        {
//            //配置文件
//            qDebug() << "ParameterManagement::initParameterFile file name is " << fileInfo.absoluteFilePath() << fileInfo.baseName() << endl;
//            ParameterTable *table = new ParameterTable(fileInfo.absoluteFilePath());
//            this->fileConfig->insert(fileInfo.baseName(), table);
//            //检查table是否有加入故障录波和示波器的参数
//            QMap<QString, QList<ParameterClass*>*>::iterator iter = table->tableMap->begin();
//            while (iter != table->tableMap->end())
//            {
//                qDebug() << "Iterator " << iter.key() << ":" << iter.value(); // 迭代器
//                QList<ParameterClass*>* list = iter.value();
//                for(int j=0; j<list->size(); j++)
//                {
//                    ParameterClass* pc = list->at(j);
//                    if(pc->isOsc)
//                    {
//                        this->oscilloscopeList->append(pc);
//                    }
//                    if(pc->isFR)
//                    {
//                        this->faultRecordingList->append(pc);
//                    }
//                }
//                iter++;
//            }
//        }
//    }
//}

///**
// * @brief ParameterManagement::addParameterFile 添加配置文件到目录
// * @param dirName 要添加的目录名
// * @param fileName 文件路径
// * @return 成功与否
// */
//bool ParameterManagement::addParameterFile(QString fileName)
//{
//    QFile file(fileName);
//    if(!file.exists())
//    {
//        return false;
//    }
//    if(fileConfig->contains(file.fileName())){
//        fileConfig->remove(file.fileName());
//    }
//    //保存配置文件目录
//    QString newFile = QDir::currentPath()+"/config/"+file.fileName();
//    if (QFile::exists(newFile)){
//        QFile::remove(newFile);
//    }
//    QFile::copy(fileName,newFile);
//    //插入table
//    ParameterTable* table = new ParameterTable(fileName);
//    if(table!=NULL)
//    {
//        this->fileConfig->insert(file.fileName(), table);
//    }
//    return true;
//}

///**
// * @brief ParameterManagement::getLevelMap--获取文件级
// * @return
// */
//QMap<QString, ParameterTable*>* ParameterManagement::getFileConfigMap()
//{
//    return fileConfig;
//}

///**
// * @brief ParameterManagement::initQFileSystemWatcher--初始化文件监听
// */
//void ParameterManagement::initQFileSystemWatcher()
//{
//    this->m_FileWatcher = new QFileSystemWatcher();

//    connect(m_FileWatcher, &QFileSystemWatcher::fileChanged, this, &ParameterManagement::on_tableFileChanged);
//    connect(m_FileWatcher, &QFileSystemWatcher::directoryChanged, this, &ParameterManagement::on_dirLevelChanged);
//}

///**
// * @brief on_tableFileChanged--table 文件内容发生改变,重新读取文件
// * @param path--路径
// */
//void ParameterManagement::on_tableFileChanged(const QString &path)
//{
//    QFile file(path);
//    if(!file.open(QIODevice::ReadOnly))
//    {
//        qDebug() << "open file : " << path << "failed!" <<endl;
//        return;
//    }
//    if(fileConfig->contains(file.fileName())){
//        fileConfig->remove(file.fileName());
//    }
//    //插入table
//    ParameterTable* table = new ParameterTable(path);
//    if(table!=NULL)
//    {
//        this->fileConfig->insert(file.fileName(), table);
//        emit tableFileChanged(file.fileName());   //发送信号
//    }
//}
///**
// * @brief on_dirLevelChanged--层级目录发生改变
// * @param path--目录路径
// */
//void ParameterManagement::on_dirLevelChanged(const QString &path)
//{

//}

////添加Parameter到故障录波
//void ParameterManagement::addParameter2FaultRecording(ParameterClass* p)
//{
//    this->faultRecordingList->append(p);
//    emit faultListChanged();

//}
////删除Parameter到故障录波
//void ParameterManagement::removeParameter2FaultRecording(ParameterClass* p)
//{
//    for(int i=0; i<this->faultRecordingList->count(); i++)
//    {
//        if(this->faultRecordingList->at(i)->registerId==p->registerId)
//        {
//            this->faultRecordingList->removeAt(i);
//            emit faultListChanged();
//            break;
//        }
//    }
//}
////添加Parameter到示波器
//void ParameterManagement::addParameter2Oscilloscope(ParameterClass* p)
//{
//    this->oscilloscopeList->append(p);
//    emit oscillListChanged();
//}
////删除Parameter到示波器
//void ParameterManagement::removeParameter2Oscilloscope(ParameterClass* p)
//{
//    for(int i=0; i<this->oscilloscopeList->count(); i++)
//    {
//        if(this->oscilloscopeList->at(i)->registerId==p->registerId)
//        {
//            this->oscilloscopeList->removeAt(i);
//            emit oscillListChanged();
//            break;
//        }
//    }
//}

////是否还能加到示波器
//bool ParameterManagement::canAddOscilloscope()
//{
//    return (this->oscilloscopeList->count()<=18);
//}
////是否还能加到故障录波
//bool ParameterManagement::canAddFaultRecording()
//{
//    return (this->faultRecordingList->count()<=18);
//}

///**
// * @brief ParameterManagement::getMainFormParameterList--获取MainForm所需要的参数
// * @return
// */
//QList<ParameterClass*>* ParameterManagement::getMainFormParameterList()
//{
//    if(this->mainFormParameterList->size()==0)
//    {
//        ParameterTable *para = this->fileConfig->value("para");
//        if(para!=NULL){
//            QMap<QString, QList<ParameterClass*>*>* map = para->tableMap;
//            QMap<QString, QList<ParameterClass*>*>::Iterator itr = map->begin();
//            int count=0;
//            bool isOk = false;
//            while(itr!=map->end()){
//                QList<ParameterClass*>* pList = itr.value();
//                for(int i=3;i<pList->size();i++)
//                {
//                    this->mainFormParameterList->append(pList->at(i));
//                    count++;
//                    if(count==30){
//                        isOk=true;
//                        break;
//                    }
//                }
//                if(isOk){
//                    break;
//                }
//            }
//        }
//    }
//    return this->mainFormParameterList;
//}
