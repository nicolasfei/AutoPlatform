#ifndef CABINETMANAGEMENT_H
#define CABINETMANAGEMENT_H

#include <QObject>
#include <QMutex>
#include "parameterclass.h"
#include "parametertable.h"

#define FAULT_DIR   "/faultUpload"
#define PARAMETER_DIR   "/parameterDownload"
#define PARA_NAME   "para"
#define VAR_NAME    "var"
#define MASTER      "master"

/**
 * @brief The CabinetManagement class
 * 对多个机柜的管理
 */
class CabinetManagement : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 描述一个机柜所需的属性变量
     */
    class CabinetDevice
    {
    public:
        QMap<QString, ParameterTable*> *m_fileConfig;     //配置文件的描述
        QList<ParameterClass*> *m_oscilloscopeList;       //示波器列表
        QList<ParameterClass*> *m_customList;             //自定义参数列表
        QString m_oscillDataDir;                          //示波器波形数据保存目录
        QString m_waveDataDir;                            //故障录波数据保存目录
        QString m_describeDir;                            //参数bit描述文件保存目录
        QList<ParameterClass*> *m_mainFormParameterList;  //main页面需要的ParameterClass list
        QList<ParameterClass*> *m_systemFormParameterList;//系统调试页面需要的ParameterClass list
        QList<ParameterClass*> *m_waterFormParameterList; //水冷页面需要的ParameterClass list
        QList<ParameterClass*> *m_diFormParameterList;    //数字io页面需要的ParameterClass list
        QList<ParameterClass*> *m_doFormParameterList;    //数字io页面需要的ParameterClass list
        QList<ParameterClass*> *m_alarmParameterList;     //告警页面需要的ParameterClass list
        QList<ParameterClass*> *m_faultParameterList;     //故障页面需要的ParameterClass list
        int m_deviceID;        //机柜编号
        ParameterClass* resetMaterControl;                  //主控设备重启
        QStringList m_paramterList;
        QStringList *m_waveChannelNameList;                 //故障录波通道名

        typedef struct Net_config {
            QString m_masterIP = "127.0.0.1";
            QString m_masterMASK = "255.255.255.0";
            QString m_masterGATEWAY = "127.0.0.1";
            int m_masterModbusPort = 502;   //modbusTCP 端口号
            int m_masterTcpPort = 8090;     //动态示波器  端口号
            int m_masterLogPort = 9090;     //日志  端口号
            QString m_ftpIP = "192.168.0.232";  //ftp ip
            int m_ftpPort=10021;
            QString m_ftpUser="root";
            QString m_ftpLogin="123456";
            QString m_ftpRootDir="/";
            QString m_ftpParameterDownloadDir;  //ftp参数文件下载目录
            QString m_ftpFaultUploadDir;        //ftp故障录波上传目录
        }NetConfig;

        NetConfig *netConfig;       //网络配置
        ParameterClass* m_lowerMachinePC;       //文件保存寄存器
        QList<ParameterClass*> *m_lmNetConfig;       //下位机网络配置
        CabinetDevice(int deviceID)
        {
            this->m_fileConfig = new QMap<QString, ParameterTable*>();
            this->m_oscilloscopeList = new QList<ParameterClass*>();
            this->m_customList = new QList<ParameterClass*>();
            this->m_mainFormParameterList = new QList<ParameterClass*>();
            this->m_systemFormParameterList = new QList<ParameterClass*>();
            this->m_waterFormParameterList = new QList<ParameterClass*>();
            this->m_diFormParameterList = new QList<ParameterClass*>();
            this->m_doFormParameterList = new QList<ParameterClass*>();
            this->m_alarmParameterList = new QList<ParameterClass*>();
            this->m_faultParameterList = new QList<ParameterClass*>();
            this->m_lmNetConfig = new QList<ParameterClass*>();
            this->netConfig = new NetConfig;
            this->m_deviceID = deviceID;
            this->m_paramterList << PARA_NAME << VAR_NAME << MASTER;
            this->resetMaterControl=Q_NULLPTR;
            this->m_waveChannelNameList = new QStringList;
        }
        ~CabinetDevice()
        {
            QMap<QString, ParameterTable*>::Iterator it = this->m_fileConfig->begin();
            while (it!=this->m_fileConfig->end())
            {
                delete it.value();
            }
            delete this->m_fileConfig;

            for(int i=0; i<m_oscilloscopeList->size(); i++)
            {
                delete this->m_oscilloscopeList->at(i);
            }
            delete this->m_oscilloscopeList;

            for(int i=0; i<m_mainFormParameterList->size(); i++)
            {
                delete this->m_mainFormParameterList->at(i);
            }
            delete this->m_mainFormParameterList;

            for(int i=0; i<m_systemFormParameterList->size(); i++)
            {
                delete this->m_systemFormParameterList->at(i);
            }
            delete this->m_systemFormParameterList;

            for(int i=0; i<m_waterFormParameterList->size(); i++)
            {
                delete this->m_waterFormParameterList->at(i);
            }
            delete this->m_waterFormParameterList;

            for(int i=0; i<m_diFormParameterList->size(); i++)
            {
                delete this->m_diFormParameterList->at(i);
            }
            delete this->m_diFormParameterList;

            for(int i=0; i<m_doFormParameterList->size(); i++)
            {
                delete this->m_doFormParameterList->at(i);
            }
            delete this->m_doFormParameterList;

            for(int i=0; i<m_alarmParameterList->size(); i++)
            {
                delete this->m_alarmParameterList->at(i);
            }
            delete this->m_alarmParameterList;

            for(int i=0; i<m_faultParameterList->size(); i++)
            {
                delete this->m_faultParameterList->at(i);
            }
            delete this->m_faultParameterList;
        }
    };

    QString faultUploadDir = FAULT_DIR;     //故障录波文件上传目录
//        QString parameterDownloadDir = PARAMETER_DIR;     //参数配置文件下载目录
//    }FtpConfig;
private:
    explicit CabinetManagement(QObject *parent = nullptr);      //禁止构造函数。
    CabinetManagement(const CabinetManagement &);               //禁止拷贝构造函数。
    CabinetManagement & operator=(const CabinetManagement &);   //禁止赋值拷贝函数。

    QList<CabinetDevice*> m_cabinetList;
    int currentCabinetID;       //当前机柜ID
//    FtpConfig *ftpConfig;

public:
    static CabinetManagement* instance(){
        static QMutex mutex;
        static QScopedPointer<CabinetManagement> inst;
        if (Q_UNLIKELY(!inst)) {
            mutex.lock();
            if (!inst) {
                inst.reset(new CabinetManagement);
            }
            mutex.unlock();
        }
        return inst.data();
    }
    ~CabinetManagement();

    //加载配置文件
    void loadParameterFile(QString filePath);
    //保存参数数据到文件
    void updateParameterConfigFile(int cabinetID, QString fileName);
    //保存配置数据到文件--网络配置
    void updateDeviceNetConfigFile(int cabinetID);
    //保存配置数据到文件--FTP配置
    void updateDeviceFtpConfigFile(int cabinetID);
    //添加Parameter到示波器
    bool addParameter2Oscilloscope(ParameterClass* p);
    //删除Parameter到示波器
    void removeParameter2Oscilloscope(ParameterClass* p);
    //删除所有Parameter到示波器
    void removeAllParameter2Oscilloscope();
    //添加Parameter到自定义列表
    void addParameter2Custom(ParameterClass* p);
    //删除Parameter从自定义列表
    void removeParameter2Custom(ParameterClass* p);
    //加载参数文件
    ParameterTable * readParameterFile(CabinetDevice *device, QString file, QString name);
    //检查参数文件
    bool checkParameterFile(QString filePath);
    //更新故障录波通道名
    void updateWaveChannel(int id, QString newName);
    //保存水冷参数
    void saveWaterCoolingParamet();
    //保存Dio参数
    void saveDioCoolingParamet();
    //获取故障录波通道名List
    QStringList *getWaveChannelNameList();
    //重启主控设备
    ParameterClass* getMasterContrlReset();
    //获取fpga地址对于的参数文件的bit描述
    QStringList getParameterBitDescribe(uint32_t fpga);
    //更新参数描述文件
    bool updateParameterBitDescribe(uint32_t fpga, QStringList descList);
    //恢复出厂设置
    void reset();

    QStringList getParamterList()
    {
        return this->m_cabinetList.at(this->currentCabinetID)->m_paramterList;
    }

    //获取mainForm所需点表变量数据
    QList<ParameterClass*> *getMainFormParameters()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_mainFormParameterList;
    }

    //获取mainForm所需点表变量数据
    QList<ParameterClass*> *getSystemFormParameters()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_systemFormParameterList;
    }

    //获取alarmForm所需点表变量数据
    QList<ParameterClass*> *getAlarmFormParameters()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_alarmParameterList;
    }

    //获取faultForm所需点表变量数据
    QList<ParameterClass*> *getFaultFormParameters()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_faultParameterList;
    }

    //获取waterForm所需点表变量数据
    QList<ParameterClass*> *getWaterFormParameters()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_waterFormParameterList;
    }

    //获取dioForm所需点表变量数据
    QList<ParameterClass*> *getDiFormParameters()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_diFormParameterList;
    }

    //获取dioForm所需点表变量数据
    QList<ParameterClass*> *getDoFormParameters()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_doFormParameterList;
    }

    //获取示波器列表
    inline QList<ParameterClass*>* getOscilloscopeList(int cabinetID)
    {
        foreach(CabinetDevice *device,this->m_cabinetList)
        {
            if(device->m_deviceID==cabinetID){
                return device->m_oscilloscopeList;
            }
        }
        return Q_NULLPTR;
    }
    //获取示波器列表
    inline QList<ParameterClass*>* getOscilloscopeList()
    {
        QList<ParameterClass*>* list = new QList<ParameterClass*>();

        for(int i=0; i<this->m_cabinetList.size(); i++)
        {
            CabinetDevice *device = this->m_cabinetList.at(i);
            if(device!=NULL && device->m_oscilloscopeList!=NULL){
                for(int j=0; j<device->m_oscilloscopeList->size(); j++){
                    list->append(device->m_oscilloscopeList->at(j));
                }
            }
        }
        return list;
    }
    //获取自定义列表
    inline QList<ParameterClass*>* getCustomList(int cabinetID)
    {
        foreach(CabinetDevice *device,this->m_cabinetList)
        {
            if(device->m_deviceID==cabinetID){
                return device->m_customList;
            }
        }
        return NULL;
    }
    //获取fileConfig
    inline QMap<QString, ParameterTable*>* getParameterConfigMap()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_fileConfig;
    }
    //获取网络IP地址,端口等
    inline CabinetDevice::NetConfig* getNetConfig(int cabinetID)
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(cabinetID)->netConfig;
    }
    //更新网络配置
    void updateNetConfig(int cabinetID);
    //更新ftp配置
    void updateFtpConfig(int cabinetID);
    //获取示波器数据保存目录
    QString getOscillDataDir()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_oscillDataDir;
    }
    /**
     * @brief saveCabinetParamet -- 保存参数文件
     * @param cabinetID -- 设备ID
     */
    void saveCabinetParamet(int cabinetID)
    {
        foreach(CabinetDevice *device,this->m_cabinetList)
        {
            if(device->m_deviceID==cabinetID){
               QMap<QString, ParameterTable*> *fileConfig = device->m_fileConfig;
               QMap<QString, ParameterTable*>::Iterator it = fileConfig->begin();
               while(it!=fileConfig->end())
               {
                   it.value()->updateTableMap2File();
                   it++;
               }
            }
        }
    }
    void setCurrentCabinetID(int id)
    {
        if(this->currentCabinetID!=id){
            this->currentCabinetID = id;
            emit cabinetIdSwitch();
        }
    }
    int getCurrentCabinetID()
    {
        return this->currentCabinetID;
    }
    //获取下位机保存参数pc
    ParameterClass* getSaveLowerMachineREG()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_lowerMachinePC;
    }
    //获取下位机网络设置寄存器
    QList<ParameterClass*>* getLmNetConfig()
    {
        if(this->m_cabinetList.size()==0){
            return Q_NULLPTR;
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_lmNetConfig;
    }
    //获取参数表标题头
    QStringList getParamterTitle()
    {
        if(this->m_cabinetList.size()==0){
            return QStringList();
        }
        if(!this->m_cabinetList.at(this->currentCabinetID)->m_fileConfig->contains("para")){
            return QStringList();
        }
        return this->m_cabinetList.at(this->currentCabinetID)->m_fileConfig->value("para")->getHeaderLabels();
    }
signals:
    void oscillListChanged();      //示波器列表发生改变
    void netConfigChanged(int cabinetID );  //网络配置发生改变
    void cabinetIdSwitch();                 //机柜被切换了
    void cabinetSwitchFinish();             //机柜切换完成
    void parameterLoadResult(bool success, QString msg);       //参数加载结果

private slots:
    void on_activitySwitchFinish(int activityID);   //机柜切换后，对应的页面切换完成
};

#endif // CABINETMANAGEMENT_H
