#include "loadparameterfileqthread.h"
#include "common/cabinetmanagement.h"

LoadParameterFileQThread::LoadParameterFileQThread(QString filePath, QObject *parent) : QThread(parent),
    filePath(filePath)
{

}

void LoadParameterFileQThread::run()
{
    CabinetManagement *manager = CabinetManagement::instance();
    manager->loadParameterFile(this->filePath);
}
