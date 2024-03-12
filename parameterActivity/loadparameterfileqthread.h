#ifndef LOADPARAMETERFILEQTHREAD_H
#define LOADPARAMETERFILEQTHREAD_H

#include <QThread>

class LoadParameterFileQThread : public QThread
{
    Q_OBJECT
public:
    explicit LoadParameterFileQThread(QString filePath, QObject *parent = nullptr);

    void run() override;

private:
    QString filePath;
};

#endif // LOADPARAMETERFILEQTHREAD_H
