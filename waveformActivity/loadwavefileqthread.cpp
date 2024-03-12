#include "loadwavefileqthread.h"
#include "common/commonclass.h"
#include <QFile>
#include <QTextCodec>

LoadWaveFileQThread::LoadWaveFileQThread(QString filePath, QObject *parent)
    : QThread(parent),
      filePath(filePath)
{

}

/**
 * 波形文件为64列，20000行的文件，每一列表示一个通道的一个数据，一行表示所有通道的一次采集数据
 */
void LoadWaveFileQThread::run()
{
    bool ok=false;
    bool conversion=true;
    QList<QString> *chanName = new QList<QString>();
    QVector<double> *xValue = new QVector<double>();
    QList<QVector<double>*> *yValueList = new QList<QVector<double>*>();
    QFile wf(filePath);
    if(!wf.open(QFile::ReadOnly)){
        emit waveDataResponse(false, false, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
        return;
    }

    QTextStream * out = new QTextStream(&wf);       //文本流
    out->setCodec(QTextCodec::codecForName("UTF-8"));

    double xV=0;
    bool addCh=false;
    while(1){
        QString line = out->readLine().trimmed();
        if(line.isEmpty()){
            break;
        }

        if(line.at(0) == '#'){      //注释行
            continue;
        }

        QStringList items = line.split(',');
        if(!addCh){
            for(int j=0; j<items.size(); j++){
                //添加通道号
                if(!chanName->contains("channel"+QString::number(j))){
                    chanName->append("channel"+QString::number(j));
                }
            }
            addCh=true;
        }
        for(int j=0; j<items.size(); j++){
            //添加数据保存容器
            if(yValueList->size()<j+1){
                QVector<double> *v = new QVector<double>();
                yValueList->append(v);
            }
            //添加数据
            QVector<double> *v = yValueList->at(j);
            float value = items.at(j).toFloat(&ok);
            if(ok){
                v->append(value);
            }else{
                v->append(0);
                conversion=false;
            }
        }
        //x轴坐标
        xV+=0.2f;
        xValue->append(xV);    //0.2ms为一个采集间隔
    }
    wf.close();
    emit waveDataResponse(true, conversion, chanName, xValue, yValueList);
}
