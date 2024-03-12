#ifndef LOADWAVEFILEQTHREAD_H
#define LOADWAVEFILEQTHREAD_H

#include <QThread>

class LoadWaveFileQThread : public QThread
{
    Q_OBJECT
public:
    explicit LoadWaveFileQThread(QString filePath, QObject *parent = nullptr);

    void run() override;
private:
    QString filePath;

signals:
    void waveDataResponse(bool success, bool coverOk, QList<QString> *chanName, QVector<double> *xValue, QList<QVector<double>*>* datas);
};

#endif // LOADWAVEFILEQTHREAD_H
