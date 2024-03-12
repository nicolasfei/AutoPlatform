#include "businessresponse.h"
#include "common/commonclass.h"
#include <QDebug>

BusinessResponse::BusinessResponse()
{
}

bool BusinessResponse::buildOscillResponseData(char *data, int len, float &xCountqqq)
{
    this->result=false;
    if(data==NULL || len==0)
    {
        qDebug() << "buildOscillResponseData data is empty!";
        return false;
    }
    static float xCount=0;
    uint8_t *respData = (uint8_t*)(data);
    switch (respData[1]){    //功能码
    case 0x70:      //这个是动态示波器的数据帧
    {
        this->type = BusinessClass::OSCILL_SAMPLE;
        switch (respData[2])        //子功能码
        {
        case 0x02:
        {
            uint16_t oscRespDataLen=len-8;
            memcpy(&this->timeStamp, respData+3, 4);       //时间戳
            this->oscSampleFre = respData[7];              //采样频率 --- 0.1khz
            this->oscSampleInterval = 1000/this->oscSampleFre; //ms     --- 每个通道间隔多少ms采样一次 *10
            this->channelDataCount = 250;//SAMPLE_TIME*100/this->oscSampleInterval;     //每个通道1秒采样多少个点
            int chNum = oscRespDataLen/1000;//(this->channelDataCount*4);//通道数 = 数据总长度/每个通道的数据个数;每个通道的数据个数=采样时间/采样间隔
            //清空数据
            this->m_oscillData.clear();
            //每个通道采集250个点的数据--50ms
            //模拟测试采用15ms为一个采样周期，采样频率固定为5000hz
            float oscSampleIntervalMS = (float)this->oscSampleInterval/100;     //采样间隔ms
            int i=8;
            for(int j=0/*, i=8*/; j<chNum; j++){
                OscillResponse resp;
                float newX = xCount; //this->timeStamp;
                resp.ch = j;
                for(int k=0; k<this->channelDataCount; k++,i+=4)
                {

                    char testb[4];
                    for(int m=0; m<4; m++){
                        testb[m] = respData[i+(3-m)];
                    }
                    float newY = 20*sin(newX);//*((float *)(testb));
                    resp.x.append(newX);
                    resp.y.append(newY);
                    newX += 1;//0.2;//oscSampleIntervalMS;
                }
                this->m_oscillData.append(resp);
            }
            xCount += 250;//this->oscSampleFre;
            if(xCount<0){
                xCount=0;
            }
            this->result=true;
        }
            break;
        default:
            qDebug() << "buildOscillResponseData error frame";
            this->errorID=DATA_ERROR;
            this->errorMsg = QString::fromLocal8Bit("动态示波器数据格式错误");
            return false;
        }
    }
        break;
    case 0x60:      //这个是配置通道响应帧
    {
        this->type = BusinessClass::OSCILL_CH_CONFIG;
        switch (respData[2])        //子功能码
        {
        case 0x02:
            this->result = respData[3];
            break;
        default:
            qDebug() << "buildOscillResponseData error frame";
            return false;
        }
    }
        break;
    default:
        qDebug() << "buildOscillResponseData error frame";
        this->errorID=DATA_ERROR;
        this->errorMsg = QString::fromLocal8Bit("动态示波器数据格式错误");
        return false;
    }
    return true;
}

bool BusinessResponse::buildWaveResponseData(QList<QString> data)
{
    this->result=false;
    this->type = BusinessClass::WAVE_QUERY;
    if(data.size()>0){
        this->m_waveData.clear();
        this->m_waveData.append(data);
        this->result=true;
        return true;
    }
    return false;
}
