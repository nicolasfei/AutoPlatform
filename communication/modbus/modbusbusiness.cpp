#include "common/commonclass.h"
#include "modbusbusiness.h"
#include "common/devicemanager.h"
#include "common/cabinetmanagement.h"

modbusBusiness::modbusBusiness(QObject *parent) : BusinessHandler(parent)
{
    //绑定IP地址改变
    this->level = BusinessClass::MODBUS_HANDLER;        //指定业务level
    CabinetManagement *manager = CabinetManagement::instance();
    //ip地址改变
    connect(manager,&CabinetManagement::netConfigChanged,this,&modbusBusiness::on_serverAddressUpdate);
    //机柜被切换
    connect(manager,&CabinetManagement::cabinetIdSwitch,this,&modbusBusiness::on_cabinetIdSwitch);
    //    this->start();
}

modbusBusiness::~modbusBusiness()
{
    BusinessHandler::~BusinessHandler();

    this->quit();
}

void modbusBusiness::run()
{
    while(!this->isThreadQuit)
    {
        if(this->m_modbus!=NULL)
        {
            modbus_close(m_modbus);
            modbus_free(m_modbus);
            m_modbus = NULL;
        }
        CabinetManagement *manager = CabinetManagement::instance();
        CabinetManagement::CabinetDevice::NetConfig *config =  manager->getNetConfig(manager->getCurrentCabinetID());
        if(config == Q_NULLPTR){
            MY_DEBUG << "NetConfig IS NULL";
            return;
        }
        QByteArray ba = config->m_masterIP.toLatin1();
        char *ip = ba.data();
        int port = config->m_masterModbusPort;
        MY_DEBUG<< "ip is :" << ip << "port is " << port;
        this->m_modbus = modbus_new_tcp(ip, port);
        if (m_modbus == NULL) {
            qDebug() << "Unable to allocate libmodbus context" << endl;
            thread()->msleep(10);
            continue;
        }
        qDebug() << "create libmodbus context success!" << endl;
        if (modbus_connect(m_modbus) == -1) {
            MY_DEBUG<< "libmodbus Connection failed: " << modbus_strerror(errno);
            modbus_free(m_modbus);
            m_modbus = NULL;
            emit netLinkStatusChange(false);
            thread()->msleep(10);
            continue;
        }
        if(modbus_set_slave(m_modbus,1) == -1){
            MY_DEBUG<< "modbus_set_slave id 1 falied: " << modbus_strerror(errno);
            modbus_free(m_modbus);
            m_modbus = NULL;
            emit netLinkStatusChange(false);
            thread()->msleep(10);
            continue;
        }
        qDebug()<<__LINE__<<__FUNCTION__<<"modbus connected.";
        isLink=true;
        emit netLinkStatusChange(true);
        int modbusRet=0;
        //设置isThreadQuit可退出线程
        while(!this->isThreadQuit && !this->ipPortChange && !this->cabinetSwitch && !netError)
        {
            sync.lock();
            if(is_pause){
                pauseCond.wait(&sync); // in this place, your thread will stop to execute until someone calls resume
            }
            sync.unlock();
            //查看队列中是否有新的指令
            this->m_currentBis = this->outQueue();
            if(this->m_currentBis.getLevel()!=BusinessClass::NONE_HANDLER)
            {
                BusinessResponse resp;
                resp.type = BusinessClass::REGISTER_RW;
                QList<ParameterClass::RegisterData> dataList = this->m_currentBis.getData();

                for(int i=0; i<dataList.size();i++)
                {
                    ParameterClass::RegisterData data = dataList.at(i);
                    uint16_t buf[2]={0};
                    if(data.rw == 0){
                        //读
                        //MY_DEBUG << "modbus read src is " << data.addr << "fpga id is " << data.fpgaID << "nb is " <<data.nb;
                        //memset(data.des.valueBuf,0,4);
                        if(data.rwFlags==1)     //03功能码
                            modbusRet = modbus_read_registers(m_modbus, data.addr, data.nb, buf);
                        else        //04功能吗
                            modbusRet = modbus_read_input_registers(m_modbus, data.addr, data.nb, buf);
//                        if(data.addr==1930){
//                            MY_DEBUG << buf[0] << "---" << buf[1];
//                        }
                        if(data.type==ParameterClass::INT32_Type ||
                                data.type==ParameterClass::UINT32_Type ||
                                data.type==ParameterClass::FLOAT32_Type){
                            data.des.valueBuf[0] = buf[1];
                            data.des.valueBuf[1] = buf[0];
                        }else{
                            data.des.valueBuf[0] = buf[0];
                            data.des.valueBuf[1] = buf[1];
                        }
                    }else{

                        MY_DEBUG << "modbus write addr is " << data.addr << "nb is " <<data.nb <<
                                    "vlaue is " << data.src.valueBuf[0] << data.src.valueBuf[1] <<
                                    "value folat is " << data.src.floatValue;
                        if(data.rwFlags==1){
                            //先写
                            if(data.type==ParameterClass::INT32_Type ||
                                    data.type==ParameterClass::UINT32_Type ||
                                    data.type==ParameterClass::FLOAT32_Type){
                                buf[1] = data.src.valueBuf[0];
                                buf[0] = data.src.valueBuf[1];
                            }else{
                                buf[0] = data.src.valueBuf[0];
                                buf[1] = data.src.valueBuf[1];
                            }
                            if(data.addr == 2000){
                                MY_DEBUG<<"this is save master control file";
                                modbusRet = modbus_write_register(m_modbus, 2001, 1);
                            }else if(data.addr == 2010){
                                MY_DEBUG<<"this is reset master control";
                                modbusRet = modbus_write_registers(m_modbus, data.addr, data.nb, buf);
                            }else{
                                if(data.nb==1){
                                    //单个寄存器采用modbus_write_register
                                    modbusRet = modbus_write_register(m_modbus, data.addr, buf[0]);
                                }else{
                                    //多个寄存器采用modbus_write_registers
                                    modbusRet = modbus_write_registers(m_modbus, data.addr, data.nb, buf);
                                }
                            }
                            //在读
                            //MY_DEBUG << "modbus read src is " << data.addr << "nb is " <<data.nb;
                            memset(buf,0,sizeof(buf));
                            modbusRet = modbus_read_registers(m_modbus, data.addr, data.nb, buf);      //在回读
                            if(data.type==ParameterClass::INT32_Type ||
                                    data.type==ParameterClass::UINT32_Type ||
                                    data.type==ParameterClass::FLOAT32_Type){
                                data.des.valueBuf[0] = buf[1];
                                data.des.valueBuf[1] = buf[0];
                            }else{
                                data.des.valueBuf[0] = buf[0];
                                data.des.valueBuf[1] = buf[1];
                            }
                            if(data.src.valueBuf[0] == data.des.valueBuf[0]){
                                if(data.nb>1){
                                    if(data.src.valueBuf[1] == data.des.valueBuf[1]){
                                        MY_DEBUG << "write registers ->" << data.addr <<
                                                    data.src.valueBuf[0] << data.src.valueBuf[1] << "success!";
                                    }
                                }else{
                                    MY_DEBUG << "write registers ->" << data.addr <<
                                                data.src.valueBuf[0] << data.src.valueBuf[1] << "success!";
                                }
                            }
                        }
                    }
                    if(modbusRet==-1){      //modbus 读写失败
                        netError=true;
                        emit this->netLinkStatusChange(false);
                        break;
                    }
                    resp.addRegisterResponse(data);
                }
                //响应
//                if(netError){
//                    MY_DEBUG << "modbus read or write failed";
//                    resp.modbusRWSuccess=
//                    emit this->errorBusiness(NET_ERROE,"MODBUS read or write error");
//                }else{
//                    emit this->ResponseBusiness(resp);
//                }
                resp.result=!netError;
                resp.errorMsg=QString::fromLocal8Bit("Modbus 读写失败");
                emit this->responseBusiness(resp);
            }
            thread()->msleep(10);
        }//end of while(!this->isThreadQuit && !this->ipPortChange && !this->cabinetSwitch && !netError)
        this->ipPortChange = false;
        this->cabinetSwitch=false;
        this->netError=false;
        this->isLink=false;
        emit netLinkStatusChange(false);
        thread()->msleep(10);
    }
}
