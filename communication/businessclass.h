#ifndef BUSINESSCLASS_H
#define BUSINESSCLASS_H

#include <QByteArray>
#include "common/parameterclass.h"

class BusinessClass
{
public:
    typedef enum
    {
        MODBUS_HANDLER=0,
        TCP_IP_HANDLER,
        FTP_HANDLER,
        NONE_HANDLER
    } LEVEL;


    typedef enum
    {
        //寄存器读写
        REGISTER_RW=0,
        //动态示波器
        OSCILL_CH_CONFIG,
        OSCILL_SAMPLE,          //动态示波器采样
        OSCILL_SAMPLE_STOP,     //动态示波器停止采样
        //故障录波
        WAVE_QUERY,
        WAVE_OBTAIN,
        //参数文件下发
        PARAMETER_DOWN_START,   //下载启动帧
        PARAMETER_DOWN,
    } COMMAND_TYPE;


    BusinessClass();

private:
    LEVEL level;        //业务级别
    COMMAND_TYPE m_commandType;      //指令类型

    QList<ParameterClass::RegisterData> m_Data;       //请求数据
    QList<QString> m_waveData;       //请求数据--故障录波
    QString ftpUser;      //账户名
    QString ftpLogin;     //密码
    QString ftpPath;      //路径
    QString filePath;     //文件路径

public:
    void setLevel(LEVEL level)
    {
        this->level = level;
    }
    LEVEL getLevel()
    {
        return this->level;
    }
    QList<ParameterClass::RegisterData> getData()
    {
        return this->m_Data;
    }
    QList<ParameterClass::RegisterData> getData(int cabinetID)
    {
        QList<ParameterClass::RegisterData> list;
        for(int i=0; i<m_Data.size(); i++)
        {
            if(m_Data.at(i).cabinetID == cabinetID)
            {
                list.append(m_Data.at(i));
            }
        }
        return list;
    }
    void setData(QList<ParameterClass::RegisterData> data)
    {
        this->m_Data = data;
    }
    void addData(ParameterClass::RegisterData data)
    {
        this->m_Data.append(data);
    }
    void setWaveData(QList<QString> list)
    {
        this->m_waveData.clear();
        this->m_waveData.append(list);
    }
    QList<QString> getWaveData()
    {
        return this->m_waveData;
    }
    COMMAND_TYPE getCommandType()
    {
        return m_commandType;
    }
    void setCommandType(COMMAND_TYPE type)
    {
        this->m_commandType = type;
    }
    void setFtpConfig(QString user, QString login, QString path)
    {
        this->ftpUser = user;
        this->ftpLogin = login;
        this->ftpPath = path;
    }
    QString getFtpUser()
    {
        return this->ftpUser;
    }
    QString getFtpLogin()
    {
        return this->ftpLogin;
    }
    QString getFtpPath()
    {
        return this->ftpPath;
    }
    void setFilePath(QString path){
        this->filePath = path;
    }
    QString getFilePath()
    {
        return this->filePath;
    }
};
#endif // BUSINESSCLASS_H
