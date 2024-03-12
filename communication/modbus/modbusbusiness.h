#ifndef MODBUSBUSINESS_H
#define MODBUSBUSINESS_H

#include "../businesshandler.h"
#include "src/modbus.h"
#include "src/modbus-tcp.h"

class modbusBusiness : public BusinessHandler
{
    Q_OBJECT
public:
    explicit modbusBusiness(QObject *parent = nullptr);
    ~modbusBusiness();

    void run() override;

private:
    modbus_t* m_modbus=NULL;
};

#endif // MODBUSBUSINESS_H
