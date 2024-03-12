#include "oscilldatasavethread.h"
#include "common/commonclass.h"
#include <QDateTime>
#include <QFile>

OscillDataSaveThread::OscillDataSaveThread(QList<BusinessResponse::OscillResponse> &oscillData, QString savePath, QObject *parent)
    : QThread(parent)
    ,oscillData(oscillData)
    ,savePath(savePath)
{

}

void OscillDataSaveThread::run()
{
    //创建文件
    if(oscillData.size()<=0){
        emit this->resultReady(false, QString::fromLocal8Bit("数据为空"));
        return;
    }
    QDateTime current_date_time =QDateTime::currentDateTime();
    QFile file(savePath);
    if(!(file.open(QIODevice::WriteOnly | QIODevice::Text))){
        MY_DEBUG << file.fileName() << " is not exists?";
        emit this->resultReady(false, QString::fromLocal8Bit("创建文件失败"));
        return;
    }
    QTextStream out(&file);
    int dataCount = oscillData.at(0).y.size();      //多少个数据
    for(int i=0; i<dataCount+2;i++){
        if(i==0){
             out<<"#";
            for(int j=0; j<oscillData.size(); j++){
                BusinessResponse::OscillResponse resp = oscillData.at(j);
                out<<"channel"+QString::number(resp.ch);        //写通道
                if(j!=oscillData.size()-1){
                   out <<",";
                }
            }
            out << "\n";    //换行
            continue;
        }
        if(i==1){
            out << "#\n";    //换行
            continue;
        }
        for(int j=0; j<oscillData.size(); j++){
            BusinessResponse::OscillResponse resp = oscillData.at(j);
            QVector<double> y=resp.y;
            out<<QString::number(y[i-2]);     //写数据
            if(j!=oscillData.size()-1){
               out <<",";
            }
        }
        out << "\n";    //换行
    }
//    for(int j=0; j<oscillData.size(); j++)
//    {
//        BusinessResponse::OscillResponse resp = oscillData.at(j);
//        out<<QString::number(resp.ch)<<",";        //写通道
//        out<<QString::number(resp.type)<<",";      //写数据类型
//        QVector<double> y=resp.y;
//        for(int k=0; k<y.size(); k++){
//            out<<QString::number(y[k])<<",";     //写数据
//        }
//        out << "\n";    //换行
//    }
    file.flush();
    file.close();
    emit resultReady(true, QString::fromLocal8Bit("保存数据成功"));
}
