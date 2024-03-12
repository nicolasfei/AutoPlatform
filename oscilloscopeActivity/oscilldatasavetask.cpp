#include "oscilldatasavetask.h"
#include "common/cabinetmanagement.h"
#include "common/commonclass.h"

#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QTextCodec>
#include <QDateTime>

OscillDataSaveTask::OscillDataSaveTask(QObject *parent)
    : QThread(parent),
      isQuit(false),
      isCainetSwitch(false)
{
    this->m_dataQueue = new QList<BusinessResponse>();
    CabinetManagement *manager = CabinetManagement::instance();
    connect(manager, &CabinetManagement::cabinetIdSwitch, this, [=](){
        isCainetSwitch=true;
    });
}

OscillDataSaveTask::~OscillDataSaveTask()
{
    MY_DEBUG;
//    this->isQuit=true;
//    this->m_qmutex.lock();
//    this->m_dataQueue->clear();
//    this->m_qmutex.unlock();
//    this->quit();
    delete this->m_dataQueue;
}

void OscillDataSaveTask::taskFinish()
{
     MY_DEBUG;
    this->isQuit=true;
//    this->m_qmutex.lock();
//    this->m_dataQueue->clear();
//    this->m_qmutex.unlock();
}

void OscillDataSaveTask::run()
{
    CabinetManagement *manager = CabinetManagement::instance();
    QString dirPath = manager->getOscillDataDir();

    while(!this->isQuit)
    {
        //创建文件
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyyMMddHHmmss");
        QString path;
        path.append(dirPath).append("/").append(current_date).append(".csv");
//        MY_DEBUG << path;
        QFile file(path);
        if(!(file.open(QIODevice::WriteOnly | QIODevice::Text)))
        {
            MY_DEBUG << file.fileName() << " is not exists?";
            thread()->msleep(10);
            continue;
        }
        QTextStream out(&file);
//        out.setVersion(QDataStream::Qt_5_12);
        int i=0;
        while(!this->isQuit) //一次数据是50ms,一个文件保存30s的数据
        {
            this->m_qmutex.lock();
            if(!this->m_dataQueue->isEmpty())
            {
                BusinessResponse resp = this->m_dataQueue->back();
                QList<BusinessResponse::OscillResponse> oscillData = resp.m_oscillData;
                //文件第一行--写每个通道的通道名
                if(i==0){
                    out<<"#";
                    for(int m=0; m<oscillData.size(); m++){
                        BusinessResponse::OscillResponse resp = oscillData.at(m);
                        out<<"channel"+QString::number(resp.ch);        //写通道
                        if(m!=oscillData.size()-1){
                           out <<",";
                        }
                    }
                    out << "\n";    //换行
                    out << "#\n";    //换行    第二行为空，前两行不是有效数据
                }
                int dataCount = oscillData.at(0).y.size();      //多少个数据
                for(int j=0; j<dataCount;j++){
                    for(int k=0; k<oscillData.size(); k++){
                        BusinessResponse::OscillResponse resp = oscillData.at(k);
                        QVector<double> y=resp.y;
                        out<<QString::number(y[j]);     //写数据
                        if(k!=oscillData.size()-1){
                           out <<",";
                        }
                    }
                    out << "\n";    //换行
                }
//                for(int j=0; j<oscillData.size(); j++)
//                {
//                    BusinessResponse::OscillResponse resp = oscillData.at(j);
//                    out<<QString::number(resp.ch)<<",";        //写通道
//                    out<<QString::number(resp.type)<<",";      //写数据类型
//                    QVector<double> y=resp.y;
//                    for(int k=0; k<y.size(); k++){
//                        out<<QString::number(y[k])<<",";     //写数据
//                    }
//                    out << "\n";    //换行
//                }
                this->m_dataQueue->pop_back();  //弹出最后一个数据
                this->m_qmutex.unlock();
                if(i++>600){
                    break;
                }
            }else{
                this->m_qmutex.unlock();
                thread()->msleep(10); //让出时间片
            }
        }
        file.flush();
        file.close();
        //检查文件数量--保存2个小时的数据，也就是2*60*60/30 = 240个文件
        QDir dir(dirPath);
        dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);  //QDir::NoDotAndDotDot 不列出.文件，即指向当前目录的软链接
        dir.setSorting(QDir::Time | QDir::Reversed);
        QFileInfoList list = dir.entryInfoList();
        //删除多余的文件，删除最早保存的文件
        while(list.size()>=240){
            QFile f(list.at(0).absoluteFilePath());
            f.remove();
            list.removeAt(0);
        }
        thread()->msleep(10);
    }
}

void OscillDataSaveTask::inQueue(BusinessResponse resp)
{
    this->m_qmutex.lock();
    this->m_dataQueue->insert(0, resp);
    this->m_qmutex.unlock();
}
