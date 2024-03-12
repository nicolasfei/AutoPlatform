#ifndef SAVEWORKER_H
#define SAVEWORKER_H
#include "common/cabinetmanagement.h"
#include "communication/businessinvoker.h"
#include "communication/businessclass.h"
#include "common/cabinetmanagement.h"
#include <QThread>

class SaveWorker : public QThread
{
    Q_OBJECT
public:
    explicit SaveWorker(QObject *parent = nullptr);

    void run() override {

        BusinessInvoker *invoker = BusinessInvoker::instance();
        if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
            emit saveFinish(false, QString::fromLocal8Bit("网络未连接"));
            return;
        }
        CabinetManagement *manager = CabinetManagement::instance();
        //先下发保存指令
        BusinessClass bc;
        bc.setLevel(BusinessClass::MODBUS_HANDLER);
        ParameterClass *pc = manager->getSaveLowerMachineREG();
        if(pc==Q_NULLPTR){
            emit saveFinish(false, QString::fromLocal8Bit("获取下位机参数保存modbus地址失败"));
            return;
        }
        ParameterClass::RegisterData reg = pc->getRegisterData();
        reg.rw = 1;
        reg.src.uintValue = 1;
        bc.addData(reg);
        invoker->exec(bc);
        //保存本地配置文件
        manager->saveCabinetParamet(manager->getCurrentCabinetID());
        emit saveFinish(true,"");
    }

signals:
    void saveFinish(bool success, QString errMsg);
};

#endif // SAVEWORKER_H
