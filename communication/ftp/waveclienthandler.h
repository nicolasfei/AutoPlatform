#ifndef WAVECLIENTHANDLER_H
#define WAVECLIENTHANDLER_H

#include <QThread>

class WaveClientHandler : public QThread
{
    Q_OBJECT
public:
    explicit WaveClientHandler(QObject *parent = nullptr);
};

#endif // WAVECLIENTHANDLER_H
