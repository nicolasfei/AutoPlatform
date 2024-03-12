#ifndef BUSINESSRESPONSE_H
#define BUSINESSRESPONSE_H

#include <QVector>
#include <QByteArray>
#include "businessclass.h"
#include "common/parameterclass.h"


#define SAMPLE_TIME (50)    //采样时间ms

class BusinessResponse
{
public:
    //动态示波器响应数据结构
    typedef struct oscillResponse{
        uint8_t ch;             //通道
        //        uint8_t data[4*75];     //数据
        //        QList<QPointF> data;    //数据
        QVector<double> x;
        QVector<double> y;
        uint8_t type;           //通道数据类型
    } OscillResponse;

    //响应类型
//    typedef enum{
//        REGISTER_RESPONSE=0,        //寄存器数据
//        OSCILL_CONFIG_RESPONSE,     //示波器通道配置数据
//        OSCILL_WAVE_RESPONSE,       //示波器波形数据
//        //故障录波响应
//        WAVE_QUERY_RESPONSE,        //故障录波查询响应
//        WAVE_OBTAIN_RESPONSE,       //故障录波获取录波文件成功
//        //        WAVE_UPLOAD_RESPONSE,     //故障录波上传录波文件响应
//        //主控发给qt的ftp传输回复
//        //01表示“确认收到启动开始下载帧”；
//        //02表示“账号/密码错误”；
//        //03表示“路径不存在”；
//        //04表示“确认登陆信息帧/确认升级变量正确”
//        //05表示“crc文件校验错误”
//        //06表示“回复下载成功”
//        //08标识“服务器不在线”
//        //09表示“非法文件”
//        FTP_RESPONSE,
//    }BusinessResponseType;

    //响应错误类型
    typedef enum{
        NOt_ERROR=0,
        NOT_HANDLER=1,      //无业务处理者
        NET_ERROE,          //网络错误
        DATA_ERROR,
        FTP_ERROR,          //FTP错误
    } B_ERROR;

    BusinessResponse();

public:
    BusinessClass::COMMAND_TYPE type;      //响应指令类型
    //响应结果
    bool result=false;  //响应结果--是否成功
    uint8_t respType;   //二级响应类型
    //错误类型
    B_ERROR errorID;
    //错误信息
    QString errorMsg;

    //寄存器读取响应数据
    QList<ParameterClass::RegisterData> m_registerData;

    //动态示波器响应数据
    QList<OscillResponse> m_oscillData;     //波形数据
    uint8_t oscSampleFre=0;         //采样频率（单位0.1khz）
    uint32_t timeStamp=0;           //时间戳
    uint16_t oscSampleInterval=0;   //在图表上的x轴采样间隔时间ms
    uint16_t channelDataCount=0;    //每个通道由多少个数据

    //故障录波文件查询响应结果
    QList<QString> m_waveData;     //故障录波文件

public:
    inline void addRegisterResponse(ParameterClass::RegisterData resp)
    {
        this->m_registerData.append(resp);
    }

    inline QList<ParameterClass::RegisterData> getRegisterData()
    {
        return m_registerData;
    }
    inline BusinessClass::COMMAND_TYPE getCommandType()
    {
        return this->type;
    }
    inline QList<OscillResponse> getOscillWaveData()
    {
        return this->m_oscillData;
    }
    inline uint16_t getOscSampleTime()
    {
        return oscSampleInterval;
    }
    inline uint16_t getChannelDataCount()
    {
        return this->channelDataCount;
    }
    bool buildOscillResponseData(char *data, int len, float &xCount);
    bool buildWaveResponseData(QList<QString> data);
};

#endif // BUSINESSRESPONSE_H
